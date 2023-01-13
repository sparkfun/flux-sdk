

#pragma once

#include "flxFlux.h"
#include "flxCore.h"
#include "flxDevice.h"

#define kReadBufferTimeoutExpired 255
#define kReadBufferExit 254
#define kReadBufferEscape 253
#define kReadBufferReturn 252
#define kReadBufferNoMatch 251


class flxSettingsSerial : public flxActionType<flxSettingsSerial>
{

  public:
    flxSettingsSerial() : _systemRoot{nullptr}
    {

        setName("Serial System Settings", "Set system settings via the Serial Console");

        // Default root is our system
        setSystemRoot(&flux);
    }

    void setSystemRoot(flxObjectContainer *theRoot)
    {
        _systemRoot = theRoot;
    }
    // Draw Settings Page entries -- this is the entry pont for this menu

    bool drawPage(flxObject *);
    bool drawPage(flxObject *, flxProperty *);
    bool drawPage(flxOperation *);
    bool drawPage(flxOperation *, flxParameter *);
    bool drawPage(flxOperation *, flxParameterIn *);
    bool drawPage(flxObject *, flxParameterIn *, flxDataLimit * );
    bool drawPage(flxObject *, flxProperty *, flxDataLimit * );
    bool drawPage(flxObjectContainer *);
    bool drawPage(flxOperationContainer *);
    bool drawPage(flxActionContainer *);
    bool drawPage(flxDeviceContainer *);

    // Our output event
    flxSignalVoid on_finished;

    bool loop();

  protected:
    bool drawPageParamInVoid(flxOperation *, flxParameterIn *);

    // Draw menu entries
    int drawMenu(flxObject *, uint);
    int drawMenu(flxOperation *, uint);
    int drawMenu(flxObjectContainer *, uint);
    int drawMenu(flxOperationContainer *, uint);
    int drawMenu(flxActionContainer *, uint);
    int drawMenu(flxDeviceContainer *, uint);
    int drawMenu(std::vector<std::string> &, uint );
    
    // Select menu entries
    int selectMenu(flxObject *, uint);
    int selectMenu(flxOperation *, uint);
    int selectMenu(flxObjectContainer *, uint);
    int selectMenu(flxOperationContainer *, uint);
    int selectMenu(flxActionContainer *, uint);
    int selectMenu(flxDeviceContainer *, uint);

    // get the selected menu item
    uint8_t getMenuSelection(uint max, uint timeout = 30);

    uint8_t getMenuSelectionYN(uint timeout = 30);

  private:

    // after set message timeout in ms
    static constexpr uint16_t  kMessageDelayTimeout = 700;

    uint8_t getMenuSelectionFunc(uint max, bool isYN, uint timeout = 30);

    void drawEntryBanner(void);
    
    //-----------------------------------------------------------------------------
    // drawPage()  - flxContainer version
    //

    template <class T> bool drawPage(flxContainer<T> *pCurrent)
    {

        if (!pCurrent)
            return false;

        uint8_t selected = 0;
        bool returnValue = false;
        int nMenuItems;

        while (true)
        {

            drawPageHeader(pCurrent);

            nMenuItems = drawMenu<T>(pCurrent, 0);

            if (nMenuItems == 0)
                Serial.printf("\tNo Entries\n\r");
            else if (nMenuItems < 0)
            {
                Serial.println("Error generating menu entries.");
                flxLog_E("Error generating menu entries");
                return false;
            }

            drawPageFooter(pCurrent);

            // Get the menu item selected by the user

            selected = getMenuSelection((uint)nMenuItems);

            // done?
            if (selected == kReadBufferTimeoutExpired || selected == kReadBufferEscape)
            {
                Serial.println("Escape");
                returnValue = false;
                break;
            }
            else if (selected == kReadBufferExit)
            {
                Serial.println((pCurrent->parent() != nullptr ? "Back" : "Exit")); // exit
                returnValue = true;
                break;
            }

            selectMenu<T>(pCurrent, selected);
        }
        return returnValue;
    };

    //-----------------------------------------------------------------------------
    // drawMenu()  - flxContainer version
    //
    // Draws the menu portion specific to the object.
    //   - The object name and description - pre-amble
    //   - Entries for the object properties
    //
    // Return Values
    //     -1 = Error
    //
    //      N = The current menu entry -- (the last entry number used)

    template <class T> int drawMenu(flxContainer<T> *pCurrent, uint level)
    {

        if (!pCurrent)
            return -1;

        // First, cascade to the flxObject portion of the menu

        int returnLevel = drawMenu((flxObject *)pCurrent, level);

        if (returnLevel < 0)
            return returnLevel; // error happened

        level = returnLevel;

        // Loop over each item in the container and draw a menu entry
        for (auto item : *pCurrent)
        {
            level++;
            drawMenuEntry(level, item);
        }

        // return the current level
        return level;
    };

    //-----------------------------------------------------------------------------
    // selectMenu() - container version
    //
    // Called with a menu item is selected.
    template <class T> int selectMenu(flxContainer<T> *pCurrent, uint level)
    {

        if (!pCurrent)
            return -1;

        // First, cascade to the flxObject portion of the menu

        int returnLevel = selectMenu((flxObject *)pCurrent, level);

        // returnLevel < 0 = ERROR
        if (returnLevel < 0)
            return returnLevel; // error happened

        // Was the item handled?
        if (level == returnLevel)
            return returnLevel;

        // Okay, we need to page out to the next page using the selected item.
        // switch from 1's base, to - based index
        uint item = level - returnLevel - 1;

        if (item >= pCurrent->size())
        {
            // item is not at this level, return
            return returnLevel + pCurrent->size();
        }

        auto pNext = pCurrent->at(item);

        // Dispatch the item to the next drawPage() call. This
        // overloaded method needs pNext to be of the correct type,
        // but all objects in the container are pointers to the base
        // class. soo...
        //
        // Find the class type and "downcast it"

        if (flxIsType<flxObjectContainer>(pNext))
        {
            drawPage(reinterpret_cast<flxObjectContainer *>(pNext));
        }
        else if (flxIsType<flxDeviceContainer>(pNext))
        {
            drawPage(reinterpret_cast<flxDeviceContainer *>(pNext));
        }
        else if (flxIsType<flxActionContainer>(pNext))
        {
            drawPage(reinterpret_cast<flxActionContainer *>(pNext));
        }
        else if (flxIsType<flxOperationContainer>(pNext))
        {
            drawPage(reinterpret_cast<flxOperationContainer *>(pNext));
        }
        else
            drawPage(pNext);

        // return the current level
        return level;
    };

    //-----------------------------------------------------------------------------
    // drawPage()  - property with a limit edition

    template <class T>
    bool drawPage(flxObject *pCurrent, T *pEntity, flxDataLimit *propLimit, bool showValue = false)
    {
        if (!pCurrent || !pEntity || !propLimit)
            return false;

        bool returnValue = false;
        uint8_t selected = 0;
        int nMenuItems;

        flxDataLimitList limitTags = propLimit->limits();

        while (true)
        {
            drawPageHeader(pCurrent, pEntity->name());

            if (showValue)
                Serial.printf("Current Value of `%s` =  %s\n\r\n\r", pEntity->name(), pEntity->to_string().c_str());
            Serial.printf("Select from the following values:\n\r\n\r");

            nMenuItems = 0;

            for (auto item : limitTags)
            {
                nMenuItems++;
                drawMenuEntry(nMenuItems, (item.name + " = " + item.data.to_string()).c_str());
            }

            if (nMenuItems == 0)
                Serial.printf("\tNo Entries\n\r");
            else if (nMenuItems < 0)
            {
                Serial.println("Error generating menu entries.");
                flxLog_E("Error generating menu entries");
                return false;
            }

            drawPageFooter(pCurrent);

            selected = getMenuSelection((uint)nMenuItems);

            // done?
            if (selected == kReadBufferTimeoutExpired || selected == kReadBufferEscape)
            {
                Serial.println("Escape");
                returnValue = false;
                break;
            }
            else if (selected == kReadBufferExit)
            {
                Serial.println((pCurrent->parent() != nullptr ? "Back" : "Exit")); // exit
                returnValue = true;
                break;
            }

            Serial.println(selected);

            bool result = pEntity->setValue(limitTags.at(selected - 1).data);

            if (result)
                Serial.printf("\t[The value of %s was updated to %s = %s ]\n\r", pEntity->name(),
                              limitTags.at(selected - 1).name.c_str(),
                              limitTags.at(selected - 1).data.to_string().c_str());
            else
                Serial.printf("\t[%s is unchanged]\n\r", pEntity->name());

            delay(kMessageDelayTimeout); // good UX here I think

            break;
        }

        return returnValue;
    };
    void drawMenuEntry(uint item, flxDescriptor *pDesc);
    void drawMenuEntry(uint item, const char *);
    void drawPageHeader(flxObject *, const char *szItem = nullptr);
    void drawPageFooter(flxObject *);
    // root for the system

    flxObjectContainer *_systemRoot;
};
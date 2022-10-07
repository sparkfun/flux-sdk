

#pragma once

#include "spCore.h"
#include "spDevice.h"

#define kReadBufferTimoutExpired 255
#define kReadBufferExit 254

class spSettingsSerial : public spActionType<spSettingsSerial>
{

  public:
    spSettingsSerial() : _systemRoot{nullptr}
    {

        setName("Serial System Setttings");
        setDescription("Set system settings via the Serial Console");
    }

    void setSystemRoot(spObjectContainer *theRoot)
    {
        _systemRoot = theRoot;
    }
    // Draw Settings Page entres -- this is the entry pont for this menu

    bool drawPage(spObject *);
    bool drawPage(spObject *, spProperty *);
    bool drawPage(spOperation *);
    bool drawPage(spOperation *, spParameter *);

    bool drawPage(spObjectContainer *);
    bool drawPage(spOperationContainer *);
    bool drawPage(spActionContainer *);
    bool drawPage(spDeviceContainer *);

    bool loop();

  protected:
    // Draw menu entries
    int drawMenu(spObject *, uint);
    int drawMenu(spOperation *, uint);
    int drawMenu(spObjectContainer *, uint);
    int drawMenu(spOperationContainer *, uint);
    int drawMenu(spActionContainer *, uint);
    int drawMenu(spDeviceContainer *, uint);

    // Select menu entries
    int selectMenu(spObject *, uint);
    int selectMenu(spOperation *, uint);
    int selectMenu(spObjectContainer *, uint);
    int selectMenu(spOperationContainer *, uint);
    int selectMenu(spActionContainer *, uint);
    int selectMenu(spDeviceContainer *, uint);

    // get the selected menu item
    uint8_t getMenuSelection(uint max, bool hasParent = true, uint timeout = 30);

  private:
    // was the entered value a "escape" value -- lev this page!
    // note 27 == escape key
    inline bool isEscape(uint8_t ch)
    {
        return (ch == 'x' || ch == 'X' || ch == 'b' || ch == 'B' || ch == 27 || ch == kReadBufferTimoutExpired);
    }

    //-----------------------------------------------------------------------------
    // drawPage()  - spContainer version
    //

    template <class T> bool drawPage(spContainer<T> *pCurrent)
    {

        if (!pCurrent)
            return false;

        uint8_t selected = 0;

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
                spLog_E("Error generating menu entries");
                return false;
            }

            drawPageFooter(pCurrent);

            // Get the menu item selected by the user

            selected = getMenuSelection((uint)nMenuItems, pCurrent->parent() != nullptr);

            // done?
            if (selected == kReadBufferTimoutExpired || selected == kReadBufferExit)
                break;

            selectMenu<T>(pCurrent, selected);
        }
        return true;
    };

    //-----------------------------------------------------------------------------
    // drawMenu()  - spContainer version
    //
    // Draws the menu portion specific to the object.
    //   - The object name and description - pre-amble
    //   - Entries for the object properties
    //
    // Return Values
    //     -1 = Error
    //
    //      N = The current menu entry -- (the last entry number used)

    template <class T> int drawMenu(spContainer<T> *pCurrent, uint level)
    {

        if (!pCurrent)
            return -1;

        // First, cascade to the spObject portion of the menu

        int returnLevel = drawMenu((spObject *)pCurrent, level);

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
    template <class T> int selectMenu(spContainer<T> *pCurrent, uint level)
    {

        if (!pCurrent)
            return -1;

        // First, cascade to the spObject portion of the menu

        int returnLevel = selectMenu((spObject *)pCurrent, level);

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

        if (spIsType<spObjectContainer>(pNext))
        {
            drawPage(reinterpret_cast<spObjectContainer *>(pNext));
        }
        else if (spIsType<spDeviceContainer>(pNext))
        {
            drawPage(reinterpret_cast<spDeviceContainer *>(pNext));
        }
        else if (spIsType<spActionContainer>(pNext))
        {
            drawPage(reinterpret_cast<spActionContainer *>(pNext));
        }
        else if (spIsType<spOperationContainer>(pNext))
        {
            drawPage(reinterpret_cast<spOperationContainer *>(pNext));
        }
        else
            drawPage(pNext);

        // return the current level
        return level;
    };

    void drawMenuEntry(uint item, spDescriptor *pDesc);
    void drawMenuEntry(uint item, const char *);
    void drawPageHeader(spObject *, const char *szItem = nullptr);
    void drawPageFooter(spObject *);
    // root for the system

    spObjectContainer *_systemRoot;
};
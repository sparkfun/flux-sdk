

#pragma once

#include "spCore.h"
#include "spDevice.h"

#define kReadBufferTimoutExpired 255

class spSettingsSerial : public spAction {

  public:

  	// Draw Settings Page entries

  	bool drawPage(spObject*);
  	bool drawPage(spObject*, spProperty*);
  	bool drawPage(spOperation*); 
  	bool drawPage(spOperation*, spParameter*);


	bool drawPage(spObjectContainer *);
    bool drawPage(spOperationContainer *);
    bool drawPage(spActionContainer *);
    bool drawPage(spDeviceContainer *);  	

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
    uint8_t getMenuSelection(uint max, uint timeout = 30);

  private:

  	inline bool isEscape(uint8_t ch){

  		return ( ch == 'x' || ch == 'X' || ch == 'b' || ch == 'B' || ch == kReadBufferTimoutExpired);
  	}
	//-----------------------------------------------------------------------------
    // drawPage()  - spContainer version
    //

    template <class T> bool drawPage(spContainer<T> *pCurrent) {

        if (!pCurrent)
            return false;

        uint8_t selected = 0;

        int nMenuItems;

        while ( true ){

			drawPageHeader(pCurrent);

			nMenuItems = drawMenu<T>(pCurrent, 0);

			if( nMenuItems ==0 )
				Serial.println("No Entries");
			else if (nMenuItems < 0)
			{
				Serial.println("Error generating menu entries.");
				spLog_E("Error generating menu entries");
				return false;
			}

			drawPageFooter(pCurrent);

			// Get the menu item selected by the user

			selected = getMenuSelection((uint)nMenuItems);

			// done?
			if ( isEscape(selected))
				break;
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

    template <class T> int drawMenu(spContainer<T> *pCurrent, uint level) {

        if (!pCurrent)
            return -1;

        // First, cascade to the spObject portion of the menu

        int returnLevel = drawMenu((spObject *)pCurrent, level);

        if (returnLevel < 0)
            return returnLevel; // error happened

        level = returnLevel;

        // Loop over each item in the container and draw a menu entry
        for (auto item : *pCurrent) {
            level++;
            drawMenuEntry(level, item);
        }

        // return the current level
        return level;
    };

    //-----------------------------------------------------------------------------
    // selectMenu()
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
        if (level < returnLevel)
            return returnLevel;

        // Okay, we need to page out to the next page using the selected item

        uint item = level - returnLevel;
        if (item >= pCurrent->size()) {
            // item is not at this level, return
            return returnLevel + pCurrent->size();
        }

        auto pNext = pCurrent->at(item);

        // Call next page with this ...

        drawPage(pNext);

        // return the current level
        return level;
    };
    void drawMenuEntry(uint item, spDescriptor *pDesc);

    void drawPageHeader(spObject *);
    void drawPageFooter(spObject *);
};

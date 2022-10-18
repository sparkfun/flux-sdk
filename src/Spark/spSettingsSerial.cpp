

#include "spSettingsSerial.h"
#include "spSerialField.h"
#include "spUtils.h"
#include <ctype.h>

#define kOutputBufferSize 128

//-----------------------------------------------------------------------------
// System settings user experience - via the serial console
//
// Allows the user to walk the system hierarchy and change property values
// and enable/disable parameters.
//
// The entire thing is dynamic and based on the structure of the system
//-----------------------------------------------------------------------------
// How this works:
//
// The overall intent is to navigate the heirarchy of the application. To do this
// the following steps take place.
//   - A current object is passed in to the drawPage() method for the object type
//   - The page is rendered, often calling the "drawMenu()" method for the
//     current object type.
//        - Note, the drawMenu() calls cascade up to the objects base classes
//   - Once the menu is drawn, getMenuSelection() is called to determine/wait for
//     the users input.
//   - Once the user selects an item, "selectMenu()" is called to determine
//     what was selected.
//        -- this leverages an objects base class in a similar manner as drawMenu()
//        -- selectMenu() will move the current menu page to the page for the
//           selected object.
//   - Note: some pages are rendered differently, based on content.
//        -- Property and Parameter pages are custom
//
//   Key Notes:
//      - This system relies on method overloading to traverse the object heirarchy
//        of the framework
//      - The general sequence of method calls are:
//              drawPage() -> drawMenu() -> selectMenu()-> drawPage() ...
//

//-----------------------------------------------------------------------------
// Draw Page
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// drawPage()  - Generic Object edition.

bool spSettingsSerial::drawPage(spObject *pCurrent)
{
    if (!pCurrent)
        return false;

    bool returnValue = false;
    uint8_t selected = 0;
    int nMenuItems;

    while (true)
    {
        drawPageHeader(pCurrent);

        nMenuItems = drawMenu(pCurrent, 0);
        if (nMenuItems == 0)
            Serial.printf("\tNo Entries\n\r");
        else if (nMenuItems < 0)
        {
            Serial.println("Error generating menu entries.");
            spLog_E("Error generating menu entries");
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
        selectMenu(pCurrent, selected);
    }

    return returnValue;
}

//-----------------------------------------------------------------------------
// drawPage() - Property Editing edition

bool spSettingsSerial::drawPage(spObject *pCurrent, spProperty *pProp)
{
    if (!pCurrent || !pProp)
        return false;

    // Any value limits set? - use in prompt loop
    bool bHasLimits = false;
    char limitRange[64] = {'\0'};

    spDataLimit *propLimit = pProp->dataLimit();   
    if (propLimit )
    {
        // limits sets are handled in another routine
        if ( propLimit->type() == spDataLimitTypeSet)
            return drawPage<spProperty>(pCurrent, pProp, propLimit, true);

        if ( propLimit->type() == spDataLimitTypeRange)
        {
            bHasLimits=true;
            const spDataLimitList limitTags = propLimit->limits();
            if(limitTags.size() > 1)
                snprintf(limitRange, sizeof(limitRange), "[%s to %s]", limitTags.at(0).name.c_str(), limitTags.at(1).name.c_str());
        }
    }
    
    // The data editor we're using - serial field
    spSerialField theDataEditor;

    // let's edit the property value
	spEditResult_t result;
    
    // Header
    drawPageHeader(pCurrent, pProp->name());

    // Editing Intro
    Serial.printf("\tEdit the value of `%s` - data type <%s>\n\r\n\r", pProp->name(),
                  spGetTypeName(pProp->type()));

    Serial.printf("\tWhen complete, press <Return> to accept, <ESC> to discard\n\r\n\r");

    while (true)
    {
    	if (bHasLimits && strlen(limitRange) > 0 )
    	    Serial.printf("\tRange for %s is %s\n\r", pProp->name(), limitRange);

    	// prompt
    	Serial.printf("\t%s = ", pProp->name());

    	// Call the property editValue() method with our editor
    	result = pProp->editValue(theDataEditor);

    	Serial.printf("\n\r\n\r");

    	if (result == spEditOutOfRange)
    	{
        	Serial.printf("\tERROR: The entered value is out of range %s \n\r\n\r", limitRange);
        	theDataEditor.beep();
        	delay(kMessageDelayTimeout/3);
    	}
       	else 
       		break;
    }
       		 	
    if (result == spEditSuccess)
        Serial.printf("\t[The value of %s was updated]\n\r", pProp->name());
    else
        Serial.printf("\t[%s is unchanged]\n\r", pProp->name());

    delay(kMessageDelayTimeout); // good UX here I think

    return result == spEditSuccess;
}

//-----------------------------------------------------------------------------
// drawPage()  - Operation/Action edition

bool spSettingsSerial::drawPage(spOperation *pCurrent)
{
    if (!pCurrent)
        return false;

    uint8_t selected = 0;

    bool returnValue = false;
    int nMenuItems;

    while (true)
    {
        drawPageHeader(pCurrent);

        nMenuItems = drawMenu(pCurrent, 0);

        if (nMenuItems == 0)
            Serial.printf("\tNo Entries\n\r");
        else if (nMenuItems < 0)
        {
            Serial.println("Error generating menu entries.");
            spLog_E("Error generating menu entries");
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

        selectMenu(pCurrent, selected);
    }

    return returnValue;
}
//-----------------------------------------------------------------------------
// drawPage() Parameter edition -- just enable/disable it

bool spSettingsSerial::drawPage(spOperation *pCurrent, spParameter *pParam)
{
    if (!pCurrent || !pParam)
        return false;

    uint8_t selected = 0;
    char szBuffer[kOutputBufferSize];

    bool returnValue = false;

    while (true)
    {
        drawPageHeader(pCurrent, pParam->name());

        Serial.printf("Enable/Disable Parameter\n\r\n\r");
        Serial.printf("\t%s is %s\n\r\n\r", pParam->name(), pParam->enabled() ? "Enabled" : "Disabled");

        snprintf(szBuffer, kOutputBufferSize, "Enable %s", pParam->name());
        drawMenuEntry(1, szBuffer);
        snprintf(szBuffer, kOutputBufferSize, "Disable %s", pParam->name());
        drawMenuEntry(2, szBuffer);

        drawPageFooter(pCurrent);

        selected = getMenuSelection((uint)2);

        // done?
        if (selected == kReadBufferTimeoutExpired || selected == kReadBufferEscape)
        {
            Serial.println("Escape");
            returnValue = false;
            break;
        }
        else if (selected == kReadBufferExit)
        {
            returnValue = true;
            break;
        }

        pParam->setEnabled(selected == 1);
    }

    return returnValue;
}
//-----------------------------------------------------------------------------
// drawPage() - Input Parameter Editing edition
//
// The user has selected an input parameter.
//
// Get inputs from the user and call the parameter with the provided data.

bool spSettingsSerial::drawPage(spOperation *pCurrent, spParameterIn *pParam)
{
    if (!pCurrent || !pParam)
        return false;

    // Void type input parameter?

    if (pParam->type() == spTypeNone)
        return drawPageParamInVoid(pCurrent, pParam);

    // Any value limits set? - use in prompt loop
    bool bHasLimits = false;
    char limitRange[64] = {'\0'};

    spDataLimit *propLimit = pParam->dataLimit();   
    if (propLimit )
    {
        // limits sets are handled in another routine
        if ( propLimit->type() == spDataLimitTypeSet)
            return drawPage<spParameterIn>(pCurrent, pParam, propLimit );
            
        if ( propLimit->type() == spDataLimitTypeRange)
        {
            bHasLimits=true;
            const spDataLimitList limitTags = propLimit->limits();
            if(limitTags.size() > 1)
                snprintf(limitRange, sizeof(limitRange), "[%s to %s]", limitTags.at(0).name.c_str(), limitTags.at(1).name.c_str());
        }
    }
    // The data editor we're using - serial field
    spSerialField theDataEditor;
   
    spEditResult_t result;

    // Header
    drawPageHeader(pCurrent, pParam->name());

    // if the parameter is a void type (spTypeNone),
    // Editing Intro
    Serial.printf("\tEnter the value to pass into `%s`(<%s>)\n\r\n\r", pParam->name(),
                  spGetTypeName(pParam->type()));

    Serial.printf("\tWhen complete, press <Return> to accept, <ESC> to discard\n\r\n\r");

    while (true)
    {
        if (bHasLimits && strlen(limitRange) > 0 )
    	    Serial.printf("\tRange for %s is %s\n\r", pParam->name(), limitRange);

        // prompt
        Serial.printf("\t%s = ", pParam->name());

        // Call the property editValue() method with our editor
        result = pParam->editValue(theDataEditor);

        Serial.printf("\n\r\n\r");

        if (result == spEditOutOfRange)
        {
            Serial.printf("\tERROR: The entered value is out of range %s \n\r\n\r", limitRange);
            theDataEditor.beep();
            delay(kMessageDelayTimeout/3);
        }
        else 
            break;
    }

    if (result == spEditSuccess)
        Serial.printf("\t[`%s` was called with the provided value.]\n\r", pParam->name());
    else
        Serial.printf("\t[`%s` was not called]\n\r", pParam->name());

    delay(kMessageDelayTimeout); // good UX here I think

    return result == spEditSuccess;
}


//-----------------------------------------------------------------------------
// drawPage() - VOID Input Parameter Editing edition
//
// The user has selected an input parameter of type Void
//

bool spSettingsSerial::drawPageParamInVoid(spOperation *pCurrent, spParameterIn *pParam)
{
    if (!pCurrent || !pParam || pParam->type() != spTypeNone)
        return false;

    // let's get a value for the parameter

    // Header
    drawPageHeader(pCurrent, pParam->name());

    // if the parameter is a void type (spTypeNone),
    // Editing Intro
    Serial.printf("\tCall `%s`() [Y/n]? ", pParam->name());

    uint8_t selected = getMenuSelectionYN();

    if (selected == kReadBufferTimeoutExpired || selected == kReadBufferExit)
        return false;

    Serial.printf("\n\r\n\r");
    if (selected == 'y')
    {
        reinterpret_cast<spParameterInVoidType *>(pParam)->set();
        Serial.printf("\t[`%s` was called]\n\r", pParam->name());
    }
    else
        Serial.printf("\t[`%s` was not called]\n\r", pParam->name());

    delay(kMessageDelayTimeout); // good UX here I think

    return selected == 'y';
}

// Container typed wrappers that use the container template for all the work
bool spSettingsSerial::drawPage(spObjectContainer *pCurrent)
{
    return drawPage<spObject *>(pCurrent);
}
//-----------------------------------------------------------------------------
bool spSettingsSerial::drawPage(spOperationContainer *pCurrent)
{
    return drawPage<spOperation *>(pCurrent);
}
//-----------------------------------------------------------------------------
bool spSettingsSerial::drawPage(spDeviceContainer *pCurrent)
{
    return drawPage<spDevice *>(pCurrent);
}
//-----------------------------------------------------------------------------
bool spSettingsSerial::drawPage(spActionContainer *pCurrent)
{
    return drawPage<spAction *>(pCurrent);
}
//-----------------------------------------------------------------------------
// drawPaeHeader()
//
// Generic header for all settings pages.
//
void spSettingsSerial::drawPageHeader(spObject *pCurrent, const char *szItem)
{

    char szBuffer[kOutputBufferSize] = {0};
    char szOutput[kOutputBufferSize] = {0};

    Serial.println();

    // Do we have a start item to add?
    if (szItem)
        snprintf(szOutput, kOutputBufferSize, "/%s", szItem);

    // build a "breadcrumb" string...
    while (pCurrent)
    {
        snprintf(szBuffer, kOutputBufferSize, "/%s", pCurrent->name());
        if (strlen(szOutput) > 0)
            strlcat(szBuffer, szOutput, kOutputBufferSize);

        strlcpy(szOutput, szBuffer, kOutputBufferSize);

        pCurrent = pCurrent->parent();
    }

    Serial.println();
    Serial.println();
    Serial.print("Settings for: ");
    Serial.println(szOutput);
    Serial.println();
}
//-----------------------------------------------------------------------------
// drawPageFooter()
//
// Generic footer for most pages -- mostly writes out the exit/back menu entry
//
void spSettingsSerial::drawPageFooter(spObject *pCurrent)
{
    Serial.println();

    if (pCurrent->parent())
        Serial.printf("\t b)  Back\n\r");
    else
        Serial.printf("\t x)  Exit\n\r");

    Serial.printf("\n\rSelect Option: ");
}

//-----------------------------------------------------------------------------
// Draw Menu  routines
//-----------------------------------------------------------------------------
// drawMenuEntry()
//
// Draw the entry in the menu for the give item
//
void spSettingsSerial::drawMenuEntry(uint item, spDescriptor *pDesc)
{

    if (!pDesc)
        return;

    Serial.printf("\t%2d)  %s - %s\n\r", item, pDesc->name(), pDesc->description());
}
//-----------------------------------------------------------------------------
// drawMenuEntry()  -- non-object edition
void spSettingsSerial::drawMenuEntry(uint item, const char *szTitle)
{

    if (!szTitle)
        return;

    Serial.printf("\t%2d)  %s\n\r", item, szTitle);
}

//-----------------------------------------------------------------------------
// drawMenu()  - spObject version
//
// Draws the menu portion specific to the object.
//   - The object name and description - pre-amble
//   - Entries for the object properties
//
// Return Values
//     -1 = Error
//
//      N = The current menu entry -- (the last entry number used)

int spSettingsSerial::drawMenu(spObject *pCurrent, uint level)
{

    if (!pCurrent)
        return -1;

    // Draw the menu for the spObject - loop over the properties for the object
    // and draw menu entries.

    if (pCurrent->nProperties() == 0)
        return level;

    Serial.println("Settings:");
    for (auto prop : pCurrent->getProperties())
    {
        level++;
        drawMenuEntry(level, prop);
    }

    // return the current level
    return level;
}

//-----------------------------------------------------------------------------
// selectMenu()  - spObject version
//
// Selects the menu portion specific to the object.
//
// Return Values
//     -1 = Error

int spSettingsSerial::selectMenu(spObject *pCurrent, uint level)
{

    if (!pCurrent)
        return -1;

    // Is the selected item here - a property?
    if (level > pCurrent->nProperties())
        return pCurrent->nProperties(); // no

    // Get the property targeted and start a new page on it.

    spProperty *theProp = pCurrent->getProperties().at(level - 1);

    // Call a new page
    drawPage(pCurrent, theProp);

    // return the current level
    return level;
}



int spSettingsSerial::drawMenu(std::vector<std::string> &entries, uint level)
{
    if (entries.size() == 0)
        return level;

    
    for (auto item : entries )
    {
        level++;
        drawMenuEntry(level, item.c_str());
    }

    // return the current level
    return level;
}

//-----------------------------------------------------------------------------
// selectMenu()  - spOperation version
//
// Selects the menu portion specific to the object.
//
// Return Values
//     -1 = Error

int spSettingsSerial::selectMenu(spOperation *pCurrent, uint level)
{

    if (!pCurrent)
        return -1;

    // First, cascade to the spObject portion of the menu

    int returnLevel = selectMenu((spObject *)pCurrent, level);

    if (returnLevel < 0)
        return returnLevel; // error happened

    // was this handled ?
    if (returnLevel == level)
        return returnLevel;

    // Output Param?

    if (level <= returnLevel + pCurrent->nOutputParameters())
    {
        auto outParam = pCurrent->getOutputParameters().at(level - returnLevel - 1);

        drawPage(pCurrent, outParam);

        return level;
    }
    else
        returnLevel += pCurrent->nOutputParameters();

    if (level <= returnLevel + pCurrent->nInputParameters())
    {
        auto inParam = pCurrent->getInputParameters().at(level - returnLevel - 1);

        drawPage(pCurrent, inParam);

        return level;
    }
    else
        returnLevel += pCurrent->nInputParameters();

    // return the current level
    return returnLevel;
}
//-----------------------------------------------------------------------------
// drawMenu()  - spOperation version
//
// Draws the menu portion specific to the object.
//   - The object name and description - pre-amble
//   - Entries for the object properties
//
// Return Values
//     -1 = Error
//
//      N = The current menu entry -- (the last entry number used)

int spSettingsSerial::drawMenu(spOperation *pCurrent, uint level)
{

    if (!pCurrent)
        return -1;

    // First, cascade to the spObject portion of the menu

    int returnLevel = drawMenu((spObject *)pCurrent, level);

    if (returnLevel < 0)
        return returnLevel; // error happened

    level = returnLevel;

    // Loop over our input and output parameters

    if (pCurrent->nOutputParameters() > 0)
    {
        Serial.println();
        Serial.println("Output Parameters:");

        for (auto outParam : pCurrent->getOutputParameters())
        {
            level++;
            drawMenuEntry(level, outParam);
        }
    }

    if (pCurrent->nInputParameters() > 0)
    {
        Serial.println();
        Serial.println("Input Parameters:");

        for (auto inParam : pCurrent->getInputParameters())
        {
            level++;
            drawMenuEntry(level, inParam);
        }
    }

    // return the current level
    return level;
}

//-----------------------------------------------------------------------------
// Container typed wrappers that use the container template for all the work
int spSettingsSerial::drawMenu(spObjectContainer *pCurrent, uint level)
{

    return drawMenu<spObject *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::drawMenu(spOperationContainer *pCurrent, uint level)
{

    return drawMenu<spOperation *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::drawMenu(spDeviceContainer *pCurrent, uint level)
{

    return drawMenu<spDevice *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::drawMenu(spActionContainer *pCurrent, uint level)
{

    return drawMenu<spAction *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
// Select Menus
//-----------------------------------------------------------------------------
// Container typed wrappers that use the container template for all the work
int spSettingsSerial::selectMenu(spObjectContainer *pCurrent, uint level)
{

    return selectMenu<spObject *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::selectMenu(spOperationContainer *pCurrent, uint level)
{

    return selectMenu<spOperation *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::selectMenu(spDeviceContainer *pCurrent, uint level)
{

    return selectMenu<spDevice *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::selectMenu(spActionContainer *pCurrent, uint level)
{

    return selectMenu<spAction *>(pCurrent, level);
}

//-----------------------------------------------------------------------------
// Helpful navigation functions
//
// was the entered value a "escape" value -- lev this page!
// note 27 == escape key
#define kpCodeEscape 27
#define kpCodeCR 13
#define kCodeBell 7

#define kNextDigitTimeout 1500

static bool isGoBack(uint8_t ch)
{
    return (ch == 'x' || ch == 'X' || ch == 'b' || ch == 'B');
}
//-----------------------------------------------------------------------------
static uint8_t menuEventNormal(uint8_t chIn, uint maxEntry, uint minEntry)
{

    uint value;

    // if it's a number, or an escape letter(set by this app) drop out of loop
    if (chIn == kpCodeEscape)
        return kReadBufferEscape;

    if (isGoBack(chIn))
        return kReadBufferExit;

    if (chIn == kpCodeCR)
        return kReadBufferReturn;

    if (isdigit(chIn))
    {
        value = chIn - '0';
        if (value + minEntry > 0 && value + minEntry <= maxEntry)
            return chIn - '0';
    }

    return kReadBufferNoMatch; // no match.
}
//-----------------------------------------------------------------------------
static uint8_t menuEventYN(uint8_t chIn)
{

    switch (chIn)
    {
    case kpCodeEscape:
        return kReadBufferEscape;

    case kpCodeCR:
    case 'y':
    case 'Y':
        return 'y';

    case 'n':
    case 'N':
        return 'n';
    }

    return kReadBufferNoMatch; // no match.
}
//-----------------------------------------------------------------------------

uint8_t spSettingsSerial::getMenuSelectionFunc(uint maxEntry, bool isYN, uint timeout)
{

    // TODO - abstract out serial calls.
    Serial.flush();

    // delay from open log Artemis

    delay(500);

    // clear buffer
    while (Serial.available() > 0)
        Serial.read();

    timeout = timeout * 1000;
    unsigned long startTime = millis();

    uint8_t chIn;
    uint    current=0;
    uint8_t number;

    while (true)
    {
        if (Serial.available() > 0)
        {
            chIn = Serial.read();

            if (isYN)
            {
                number = menuEventYN(chIn);
                if( number )
                    break;
            }
            else
            {
                number = menuEventNormal(chIn, maxEntry, current * 10);

                // Jump out of this menu
                if ( number == kReadBufferEscape || number == kReadBufferExit)
                    break;

                // user hit return - do we have pending daa.
                if ( number == kReadBufferReturn )
                {
                    // Pending selection? 
                    if ( current )
                    {
                        number = current;
                        break;
                    }
                    // nothing, just continue
                    continue;
                }   // no match?
                else if ( number == kReadBufferNoMatch )
                {
                    // Invalid entry 
                    Serial.write(kCodeBell);
                    // reset timeout
                    startTime = millis();
                    continue;
                }

                // Add up the curent digits - for multi digit entries
                current = current * 10 + number; 

                // Is there room for a possible othe digit? If not, return this number
                if ( current * 10 >  maxEntry)
                {
                    number = current;
                    break;
                }
                // print out the number that was entered as a prompt ...
                Serial.printf("%u", number);

                // the user could enter another digit
                // adjust the timeout to give them a chance to do this....
                startTime = millis();
                timeout = kNextDigitTimeout;
            }
        }

        // Timeout?
        if ((millis() - startTime) > timeout)
        {
            // number in the queue?
            if ( current )
                number = current;
            else
            {
                Serial.println("No user input received.");
                number = kReadBufferTimeoutExpired;
            }
            break;
        }
        delay(100);
    }
    Serial.flush();
    return number;
}
//--------------------------------------------------------------------------
// get the selected menu item
uint8_t spSettingsSerial::getMenuSelection(uint max, uint timeout)
{
    return getMenuSelectionFunc(max, false, timeout);
}
//--------------------------------------------------------------------------
uint8_t spSettingsSerial::getMenuSelectionYN(uint timeout)
{
    return getMenuSelectionFunc(0, true, timeout);
}
//--------------------------------------------------------------------------
// Loop call
//
// If we see input on the serial console, and we have a menu root set, start
// a settings session

bool spSettingsSerial::loop(void)
{

    if (_systemRoot && Serial.available())
    {
        bool doSave = drawPage(_systemRoot);
        Serial.printf("\n\r\n\rEnd Settings\n\r");

        // clear buffer
        while (Serial.available() > 0)
            Serial.read();

        // send out a done event if the changes were successful.

        if (doSave)
            on_finished.emit();
    }

    return true;
}

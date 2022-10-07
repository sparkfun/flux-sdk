

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

        selected = getMenuSelection((uint)nMenuItems, pCurrent->parent() != nullptr);

        // done?
        if (selected == kReadBufferTimoutExpired || selected == kReadBufferExit)
            break;

        selectMenu(pCurrent, selected);
    }

    return true;
}

//-----------------------------------------------------------------------------
// drawPage() - Property Editing edition

bool spSettingsSerial::drawPage(spObject *pCurrent, spProperty *pProp)
{
    if (!pCurrent)
        return false;

    // The data editor we're using - serial field
    spSerialField theDataEditor;

    // let's edit the property value

    // Header
    drawPageHeader(pCurrent, pProp->name());

    // Editing Intro
    Serial.printf("\tEdit the value of `%s` - data type <%s>\n\r\n\r", pProp->name(),
                  sp_utils::spTypeName(pProp->type()));

    Serial.printf("\tWhen complete, press <Return> to accept, <ESC> to discard\n\r\n\r");

    Serial.printf("\t%s = ", pProp->name());

    // Call the property editValue() method with our editor
    bool bSuccess = pProp->editValue(theDataEditor);

    Serial.printf("\n\r\n\r");
    if (bSuccess)
        Serial.printf("\t[The value of %s was updated]\n\r", pProp->name());
    else
        Serial.printf("\t[%s is unchanged]\n\r", pProp->name());

    delay(1000); // good UX here I think

    return true;
}
//-----------------------------------------------------------------------------
// drawPage()  - Operation/Action edition

bool spSettingsSerial::drawPage(spOperation *pCurrent)
{
    if (!pCurrent)
        return false;

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

        selected = getMenuSelection((uint)nMenuItems, pCurrent->parent() != nullptr);

        // done?
        if (selected == kReadBufferTimoutExpired || selected == kReadBufferExit)
            break;
        // done?
        if (isEscape(selected))
            break;

        selectMenu(pCurrent, selected);
    }

    return true;
}
//-----------------------------------------------------------------------------
// drawPage() Parameter edition -- just enable/disable it

bool spSettingsSerial::drawPage(spOperation *pCurrent, spParameter *pParam)
{
    if (!pCurrent)
        return false;

    uint8_t selected = 0;
    char szBuffer[kOutputBufferSize];

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
        if (selected == kReadBufferTimoutExpired || selected == kReadBufferExit)
            break;

        pParam->setEnabled(selected == 1);
    }

    return true;
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
uint8_t spSettingsSerial::getMenuSelection(uint maxEntry, bool hasParent, uint timeout)
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
    uint value;
    while (true)
    {
        if (Serial.available() > 0)
        {

            chIn = Serial.read();

            // if it's a number, or an escape letter(set by this app) drop out of loop
            if (isEscape(chIn))
            {
                Serial.print((hasParent ? 'b' : 'x'));
                // Serial.print("\u2588"); // kdb block
                // Serial.print('\a');  // kdb bell
                chIn = kReadBufferExit;
                break;
            }

            else if (isdigit(chIn))
            {
                value = chIn - '0';
                if (value > 0 && value <= maxEntry)
                {
                    chIn -= '0';
                    Serial.print(chIn);

                    break;
                }
            }
        }

        // Timeout?
        if ((millis() - startTime) > timeout)
        {
            Serial.println("No user input recieved.");
            chIn = kReadBufferTimoutExpired;
            break;
        }
        delay(100);
    }
    Serial.flush();
    return chIn;
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
        drawPage(_systemRoot);
        Serial.printf("\n\r\n\rEnd Settings\n\r");

        // clear buffer
        while (Serial.available() > 0)
            Serial.read();
    }

    return true;
}

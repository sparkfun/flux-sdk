/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#include "flxSettingsSerial.h"
#include "flxFlux.h"
#include "flxSerialField.h"
#include "flxUtils.h"
#include <ctype.h>
#include <time.h>

const uint16_t kOutputBufferSize = 256;

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
// The overall intent is to navigate the hierarchy of the application. To do this
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
//      - This system relies on method overloading to traverse the object hierarchy
//        of the framework
//      - The general sequence of method calls are:
//              drawPage() -> drawMenu() -> selectMenu()-> drawPage() ...
//

//-----------------------------------------------------------------------------
// Draw Page
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// drawPage()  - Generic Object edition.

bool flxSettingsSerial::drawPage(flxObject *pCurrent)
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
            // Serial.println("Error generating menu entries.");
            flxLog_E(F("Error generating menu entries"));
            return false;
        }

        drawPageFooter(pCurrent);

        selected = getMenuSelection((uint)nMenuItems, menuTimeout());

        // done?
        if (selected == kReadBufferTimeoutExpired || selected == kReadBufferEscape)
        {
            flxSerial.textToYellow();
            Serial.println("Escape");
            flxSerial.textToNormal();
            returnValue = false;
            break;
        }
        else if (selected == kReadBufferExit)
        {
            flxSerial.textToWhite();
            Serial.println((pCurrent->parent() != nullptr ? "Back" : "Exit")); // exit
            flxSerial.textToNormal();
            returnValue = true;
            break;
        }

        // Serial.println(selected);
        selectMenu(pCurrent, selected);
    }

    return returnValue;
}

//-----------------------------------------------------------------------------
// drawPage() - Property Editing edition

bool flxSettingsSerial::drawPage(flxObject *pCurrent, flxProperty *pProp)
{
    if (!pCurrent || !pProp)
        return false;

    // Any value limits set? - use in prompt loop
    bool bHasLimits = false;
    char limitRange[64] = {'\0'};

    flxDataLimit *propLimit = pProp->dataLimit();
    if (propLimit)
    {
        // limits sets are handled in another routine
        if (propLimit->type() == flxDataLimitTypeSet)
            return drawPage<flxProperty>(pCurrent, pProp, propLimit, true);

        if (propLimit->type() == flxDataLimitTypeRange)
        {
            bHasLimits = true;
            const flxDataLimitList limitTags = propLimit->limits();
            if (limitTags.size() > 1)
                snprintf(limitRange, sizeof(limitRange), "[%s to %s]", limitTags.at(0).name.c_str(),
                         limitTags.at(1).name.c_str());
        }
    }

    // The data editor we're using - serial field
    flxSerialField theDataEditor;

    // let's edit the property value
    flxEditResult_t result;

    // Header
    drawPageHeader(pCurrent, pProp->name());

    // Editing Intro

    Serial.printf("\tEdit the value of ");
    flxSerial.textToWhite();
    Serial.printf("%s", pProp->name());
    flxSerial.textToNormal();
    Serial.printf(" - data type <%s>\n\r\n\r", flxGetTypeName(pProp->type()));

    Serial.printf("\tWhen complete, press <Return> to accept, <ESC> to discard\n\r\n\r");

    while (true)
    {
        if (bHasLimits && strlen(limitRange) > 0)
            Serial.printf("\tRange for %s is %s\n\r", pProp->name(), limitRange);

        // prompt
        Serial.printf("\t%s = ", pProp->name());

        // Call the property editValue() method with our editor
        result = pProp->editValue(theDataEditor);

        Serial.printf("\n\r\n\r");

        if (result == flxEditOutOfRange)
        {
            flxSerial.textToRed();
            Serial.printf("\tERROR");
            flxSerial.textToNormal();
            Serial.printf(": The entered value is out of range %s \n\r\n\r", limitRange);
            theDataEditor.beep();
            delay(kMessageDelayTimeout / 3);
        }
        else
            break;
    }

    if (result == flxEditSuccess)
        Serial.printf("\t[The value of %s was updated]\n\r", pProp->name());
    else
        Serial.printf("\t[%s is unchanged]\n\r", pProp->name());

    delay(kMessageDelayTimeout); // good UX here I think

    return result == flxEditSuccess;
}

//-----------------------------------------------------------------------------
// drawPage()  - Operation/Action edition

bool flxSettingsSerial::drawPage(flxOperation *pCurrent)
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
            flxLog_E("Error generating menu entries");
            return false;
        }
        drawPageFooter(pCurrent);

        selected = getMenuSelection((uint)nMenuItems, menuTimeout());

        // done?
        if (selected == kReadBufferTimeoutExpired || selected == kReadBufferEscape)
        {
            flxSerial.textToYellow();
            Serial.println("Escape");
            flxSerial.textToNormal();
            returnValue = false;
            break;
        }
        else if (selected == kReadBufferExit)
        {
            flxSerial.textToWhite();
            Serial.println((pCurrent->parent() != nullptr ? "Back" : "Exit")); // exit
            flxSerial.textToNormal();
            returnValue = true;
            break;
        }

        // Serial.println(selected);
        selectMenu(pCurrent, selected);
    }

    return returnValue;
}
//-----------------------------------------------------------------------------
// drawPage() Parameter edition -- just enable/disable it

bool flxSettingsSerial::drawPage(flxOperation *pCurrent, flxParameter *pParam)
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

        selected = getMenuSelection((uint)2, menuTimeout());

        // done?
        if (selected == kReadBufferTimeoutExpired || selected == kReadBufferEscape)
        {
            flxSerial.textToYellow();
            Serial.println("Escape");
            flxSerial.textToNormal();
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

bool flxSettingsSerial::drawPage(flxOperation *pCurrent, flxParameterIn *pParam)
{
    if (!pCurrent || !pParam)
        return false;

    // Void type input parameter?

    if (pParam->type() == flxTypeNone)
        return drawPageParamInVoid(pCurrent, pParam);

    // Any value limits set? - use in prompt loop
    bool bHasLimits = false;
    char limitRange[64] = {'\0'};

    flxDataLimit *propLimit = pParam->dataLimit();
    if (propLimit)
    {
        // limits sets are handled in another routine
        if (propLimit->type() == flxDataLimitTypeSet)
            return drawPage<flxParameterIn>(pCurrent, pParam, propLimit);

        if (propLimit->type() == flxDataLimitTypeRange)
        {
            bHasLimits = true;
            const flxDataLimitList limitTags = propLimit->limits();
            if (limitTags.size() > 1)
                snprintf(limitRange, sizeof(limitRange), "[%s to %s]", limitTags.at(0).name.c_str(),
                         limitTags.at(1).name.c_str());
        }
    }
    // The data editor we're using - serial field
    flxSerialField theDataEditor;

    flxEditResult_t result;

    // Header
    drawPageHeader(pCurrent, pParam->name());

    // if the parameter is a void type (flxTypeNone),
    // Editing Intro
    Serial.printf("\tEnter the value to pass into `%s`(<%s>)\n\r\n\r", pParam->name(), flxGetTypeName(pParam->type()));

    Serial.printf("\tWhen complete, press <Return> to accept, <ESC> to discard\n\r\n\r");

    while (true)
    {
        if (bHasLimits && strlen(limitRange) > 0)
            Serial.printf("\tRange for %s is %s\n\r", pParam->name(), limitRange);

        // prompt
        Serial.printf("\t%s = ", pParam->name());

        // Call the property editValue() method with our editor
        result = pParam->editValue(theDataEditor);

        Serial.printf("\n\r\n\r");

        if (result == flxEditOutOfRange)
        {
            flxSerial.textToRed();
            Serial.printf("\tERROR");
            flxSerial.textToNormal();
            Serial.printf(": The entered value is out of range %s \n\r\n\r", limitRange);
            theDataEditor.beep();
            delay(kMessageDelayTimeout / 3);
        }
        else
            break;
    }

    if (result == flxEditSuccess)
        Serial.printf("\t[`%s` was called with the provided value.]\n\r", pParam->name());
    else
        Serial.printf("\t[`%s` was not called]\n\r", pParam->name());

    delay(kMessageDelayTimeout); // good UX here I think

    return result == flxEditSuccess;
}

//-----------------------------------------------------------------------------
// drawPage() - VOID Input Parameter Editing edition
//
// The user has selected an input parameter of type Void
//

bool flxSettingsSerial::drawPageParamInVoid(flxOperation *pCurrent, flxParameterIn *pParam)
{
    if (!pCurrent || !pParam || pParam->type() != flxTypeNone)
        return false;

    // let's get a value for the parameter

    // Header
    drawPageHeader(pCurrent, pParam->name());

    uint8_t selected = 'y';
    flxParameterInVoidType *pVoid = reinterpret_cast<flxParameterInVoidType *>(pParam);

    // prompt before calling (some void calls do their own prompt/ux)
    if (pVoid->prompt)
    {
        // if the parameter is a void type (flxTypeNone),
        // Editing Intro
        Serial.printf("\tCall `%s`() [Y/n]? ", pParam->name());

        selected = getMenuSelectionYN(menuTimeout());

        if (selected == kReadBufferTimeoutExpired || selected == kReadBufferExit)
            return false;

        Serial.printf("\n\r\n\r");
    }
    if (selected == 'y')
    {
        pVoid->set();
        Serial.printf("\t[`%s` was called]\n\r", pParam->name());
    }
    else
        Serial.printf("\t[`%s` was not called]\n\r", pParam->name());

    delay(kMessageDelayTimeout); // good UX here I think

    return selected == 'y';
}

// Container typed wrappers that use the container template for all the work
bool flxSettingsSerial::drawPage(flxObjectContainer *pCurrent)
{
    return drawPage<flxObject *>(pCurrent);
}
//-----------------------------------------------------------------------------
bool flxSettingsSerial::drawPage(flxOperationContainer *pCurrent)
{
    return drawPage<flxOperation *>(pCurrent);
}
//-----------------------------------------------------------------------------
bool flxSettingsSerial::drawPage(flxDeviceContainer *pCurrent)
{
    return drawPage<flxDevice *>(pCurrent);
}
//-----------------------------------------------------------------------------
bool flxSettingsSerial::drawPage(flxActionContainer *pCurrent)
{
    return drawPage<flxAction *>(pCurrent);
}
//-----------------------------------------------------------------------------
// drawPaeHeader()
//
// Generic header for all settings pages.
//
void flxSettingsSerial::drawPageHeader(flxObject *pCurrent, const char *szItem)
{

    char szBuffer[kOutputBufferSize];
    char szOutput[kOutputBufferSize];

    memset(szBuffer, '\0', kOutputBufferSize);
    memset(szOutput, '\0', kOutputBufferSize);
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
    flxSerial.textToWhite();
    Serial.printf("\n\r\n\rSettings for:");
    flxSerial.textToNormal();
    Serial.printf(" %s\n\r\n\r", szOutput);
}
//-----------------------------------------------------------------------------
// drawPageFooter()
//
// Generic footer for most pages -- mostly writes out the exit/back menu entry
//
void flxSettingsSerial::drawPageFooter(flxObject *pCurrent)
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
void flxSettingsSerial::drawMenuEntry(uint item, flxDescriptor *pDesc)
{

    if (!pDesc)
        return;

    if (pDesc->title())
    {
        flxSerial.textToWhite();
        Serial.printf("\n\r    %s\n\r", pDesc->title());
        flxSerial.textToNormal();
    }

    Serial.printf("\t%2d)  %s - %s\n\r", item, pDesc->name(), pDesc->description());
}

//-----------------------------------------------------------------------------
// drawMenuEntry()
//
// Draw the entry in the menu for the give item - property version
//
// Will output the value of the property (if it's not secure)
//
void flxSettingsSerial::drawMenuEntry(uint item, flxProperty *pProp)
{

    if (!pProp)
        return;

    if (pProp->title())
    {
        flxSerial.textToWhite();
        Serial.printf("\n\r    %s\n\r", pProp->title());
        flxSerial.textToNormal();
    }

    Serial.printf("\t%2d)  %s - %s  : ", item, pProp->name(), pProp->description());

    // write out the value of the property - but not secure properties

    flxSerial.textToWhite();

    // Secure string?
    if (pProp->secure())
    {
        int sLen = pProp->to_string().length();
        for (int i = 0; i < sLen; i++)
            Serial.printf("*");
    }
    else
    {
        std::string sTmp;

        // if we have a data limit of type set on the property, print out the
        // limit name, not the value
        flxDataLimit *propLimit = pProp->dataLimit();
        if (propLimit && propLimit->type() == flxDataLimitTypeSet)
        {
            // Get the current value - then get the limit name based on the value.
            // Note: Using "variable" to capture all types
            flxDataVariable var = pProp->getValue();
            sTmp = propLimit->getName(var);
        }
        else
            sTmp = pProp->to_string(); // just get the value as a string.

        Serial.printf("%s", sTmp.c_str());
    }

    flxSerial.textToNormal();

    Serial.printf("\n\r");
}
//-----------------------------------------------------------------------------
// drawMenuEntry()
//
// Draw the entry in the menu for the give item - output parameter version
//
void flxSettingsSerial::drawMenuEntry(uint item, flxParameter *pParam)
{

    if (!pParam)
        return;

    if (pParam->title())
        Serial.printf("\n\r    %s\n\r", pParam->title());

    Serial.printf("\t%2d)  %-20s - %-40s  ", item, pParam->name(), pParam->description());
    if (!pParam->enabled())
    {
        flxSerial.textToYellow();
        Serial.printf("{disabled}");
        flxSerial.textToNormal();
    }
    Serial.printf("\n\r");
}
//-----------------------------------------------------------------------------
// drawMenuEntry()  -- non-object edition
void flxSettingsSerial::drawMenuEntry(uint item, const char *szTitle)
{

    if (!szTitle)
        return;

    Serial.printf("\t%2d)  %s\n\r", item, szTitle);
}

//-----------------------------------------------------------------------------
// drawMenu()  - flxObject version
//
// Draws the menu portion specific to the object.
//   - The object name and description - pre-amble
//   - Entries for the object properties
//
// Return Values
//     -1 = Error
//
//      N = The current menu entry -- (the last entry number used)

int flxSettingsSerial::drawMenu(flxObject *pCurrent, uint level)
{

    if (!pCurrent)
        return -1;

    // Draw the menu for the flxObject - loop over the properties for the object
    // and draw menu entries.

    if (pCurrent->nProperties() == 0)
        return level;

    flxSerial.textToWhite();
    Serial.println("    Settings");
    flxSerial.textToNormal();
    for (auto prop : pCurrent->getProperties())
    {
        if (prop->hidden())
            break;
        level++;
        drawMenuEntry(level, prop);
    }

    // return the current level
    return level;
}

//-----------------------------------------------------------------------------
// selectMenu()  - flxObject version
//
// Selects the menu portion specific to the object.
//
// Return Values
//     -1 = Error

int flxSettingsSerial::selectMenu(flxObject *pCurrent, uint level)
{

    if (!pCurrent)
        return -1;

    // Is the selected item here - a property?
    if (level > pCurrent->nProperties())
        return pCurrent->nProperties(); // no

    // Get the property targeted and start a new page on it.

    flxProperty *theProp = pCurrent->getProperties().at(level - 1);

    // Call a new page
    drawPage(pCurrent, theProp);

    // return the current level
    return level;
}

int flxSettingsSerial::drawMenu(std::vector<std::string> &entries, uint level)
{
    if (entries.size() == 0)
        return level;

    for (auto item : entries)
    {
        level++;
        drawMenuEntry(level, item.c_str());
    }

    // return the current level
    return level;
}

//-----------------------------------------------------------------------------
// selectMenu()  - flxOperation version
//
// Selects the menu portion specific to the object.
//
// Return Values
//     -1 = Error

int flxSettingsSerial::selectMenu(flxOperation *pCurrent, uint level)
{

    if (!pCurrent)
        return -1;

    // First, cascade to the flxObject portion of the menu

    int returnLevel = selectMenu((flxObject *)pCurrent, level);

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
// drawMenu()  - flxOperation version
//
// Draws the menu portion specific to the object.
//   - The object name and description - pre-amble
//   - Entries for the object properties
//
// Return Values
//     -1 = Error
//
//      N = The current menu entry -- (the last entry number used)

int flxSettingsSerial::drawMenu(flxOperation *pCurrent, uint level)
{

    if (!pCurrent)
        return -1;

    // First, cascade to the flxObject portion of the menu

    int returnLevel = drawMenu((flxObject *)pCurrent, level);

    if (returnLevel < 0)
        return returnLevel; // error happened

    level = returnLevel;

    // Loop over our input and output parameters

    if (pCurrent->nOutputParameters() > 0)
    {
        Serial.println();
        flxSerial.textToWhite();
        Serial.println("    Outputs");
        flxSerial.textToNormal();

        for (auto outParam : pCurrent->getOutputParameters())
        {
            level++;
            drawMenuEntry(level, outParam);
        }
    }

    if (pCurrent->nInputParameters() > 0)
    {
        Serial.println();
        flxSerial.textToWhite();
        Serial.println("    Functions");
        flxSerial.textToNormal();

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
int flxSettingsSerial::drawMenu(flxObjectContainer *pCurrent, uint level)
{

    return drawMenu<flxObject *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int flxSettingsSerial::drawMenu(flxOperationContainer *pCurrent, uint level)
{

    return drawMenu<flxOperation *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int flxSettingsSerial::drawMenu(flxDeviceContainer *pCurrent, uint level)
{

    return drawMenu<flxDevice *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int flxSettingsSerial::drawMenu(flxActionContainer *pCurrent, uint level)
{

    return drawMenu<flxAction *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
// Select Menus
//-----------------------------------------------------------------------------
// Container typed wrappers that use the container template for all the work
int flxSettingsSerial::selectMenu(flxObjectContainer *pCurrent, uint level)
{

    return selectMenu<flxObject *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int flxSettingsSerial::selectMenu(flxOperationContainer *pCurrent, uint level)
{

    return selectMenu<flxOperation *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int flxSettingsSerial::selectMenu(flxDeviceContainer *pCurrent, uint level)
{

    return selectMenu<flxDevice *>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int flxSettingsSerial::selectMenu(flxActionContainer *pCurrent, uint level)
{

    return selectMenu<flxAction *>(pCurrent, level);
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

uint8_t flxSettingsSerial::getMenuSelectionFunc(uint maxEntry, bool isYN, uint timeout)
{

    // TODO - abstract out serial calls.
    Serial.flush();

    // delay from open log Artemis

    delay(200);

    // clear buffer
    while (Serial.available() > 0)
        Serial.read();

    timeout = timeout * 1000;
    unsigned long startTime = millis();

    uint8_t chIn;
    uint current = 0;
    uint8_t number;

    while (true)
    {
        if (Serial.available() > 0)
        {
            chIn = Serial.read();

            if (isYN)
            {
                number = menuEventYN(chIn);
                if (number)
                    break;
            }
            else
            {
                number = menuEventNormal(chIn, maxEntry, current * 10);

                // Jump out of this menu
                if (number == kReadBufferEscape || number == kReadBufferExit)
                    break;

                // user hit return - do we have pending data.
                if (number == kReadBufferReturn)
                {
                    // Pending selection?
                    if (current)
                    {
                        number = current;
                        break;
                    }
                    // nothing, just continue
                    continue;
                } // no match?
                else if (number == kReadBufferNoMatch)
                {
                    // Invalid entry
                    Serial.write(kCodeBell);
                    // reset timeout
                    startTime = millis();
                    continue;
                }

                // print out the number that was entered as a prompt ...
                flxSerial.textToGreen();
                Serial.printf("%u", number);
                flxSerial.textToNormal();
                // Add up the curent digits - for multi digit entries
                current = current * 10 + number;

                // Is there room for a possible additional digit? If not, return this number
                if (current * 10 > maxEntry)
                {
                    number = current;
                    break;
                }

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
            if (current)
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
uint8_t flxSettingsSerial::getMenuSelection(uint max, uint timeout)
{
    return getMenuSelectionFunc(max, false, timeout);
}
//--------------------------------------------------------------------------
uint8_t flxSettingsSerial::getMenuSelectionYN(uint timeout)
{
    return getMenuSelectionFunc(0, true, timeout);
}

void flxSettingsSerial::drawEntryBanner(void)
{
    // Let's draw a header as we enter

    flux.writeBanner();

    Serial.printf("Device ID: %s\n\r", flux.deviceId());

    time_t t_now;
    time(&t_now);
    struct tm *tmLocal = localtime(&t_now);
    char szBuffer[64];

    memset(szBuffer, '\0', sizeof(szBuffer));
    strftime(szBuffer, sizeof(szBuffer), "%G-%m-%dT%T", tmLocal);
    Serial.printf("Time: %s\n\r", szBuffer);

    // uptime
    uint32_t days, hours, minutes, secs, mills;

    flx_utils::uptime(days, hours, minutes, secs, mills);

    Serial.printf("Uptime: %u days, %02u:%02u:%02u.%u\n\r", days, hours, minutes, secs, mills);
}

//--------------------------------------------------------------------------
///
/// @brief Called to launch - run - end an edit settings session
///
/// @retval  -1: Error, 0:Success, 1:Success and Save

int flxSettingsSerial::editSettings(void)
{
    if (!_systemRoot)
    {
        flxLog_E(F("%s: System on provided to edit settings"));
        return -1;
    }

    // Edit time!
    flxSendEvent(flxEvent::kOnEdit, true);

    drawEntryBanner();

    bool doSave = drawPage(_systemRoot);

    flxSerial.textToWhite();
    Serial.printf("\n\r\n\rEnd Settings\n\r");
    flxSerial.textToNormal();

    // clear buffer
    while (Serial.available() > 0)
        Serial.read();

    // send out a done event if the changes were successful.

    // Was the menu returned normally and were changes made - the system root will reflect this?
    if (doSave && _systemRoot->isDirty())
    {
        flxLog_I(F("Saving System Settings"));
        flxSendEvent(flxEvent::kOnEditFinished);
    }
    flxSendEvent(flxEvent::kOnEdit, false);

    return doSave ? 1 : 0;
}

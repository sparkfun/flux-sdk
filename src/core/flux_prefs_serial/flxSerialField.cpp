/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#include <inttypes.h>
#include <string.h>

#include <limits>

#include <Arduino.h>

#include "flxCoreLog.h"
#include "flxSerialField.h"
//
// ----------------------------------------------------------------------
// Serial terminal data entry field - for use with terminal apps.
//
// Terminal Apps Tested
//
//  * minicom   - macOS
//  * screen    - macOS   (note: Control-A doesn't work - its a screen command code)
//
//
// Line text editing supported:
//
//  * Backspace   - Delete previous char
//  * DEL         - Delete next char
//  * Arrow Left  - Move entry cursor to the left
//  * Arrow Right - Move entry cursor to the right
//  * Ctrl-A      - Move entry cursor to start of line
//  * Ctrl-E      - Move entry cursor to the end of line
//  * Ctrl-K      - Kill to EOL (delete all chars to right of cursor)
//  * Escape      - Cancel operation - discard changes
//  * Carrige Ret - Accept changes, return results
//  * "Bell"      - rings terminal bell on error/warning
//
// ----------------------------------------------------------------------
// How this works using the context struct:
//
//    If you have have a line of text, with a cursor in the middle
//
//       This is some text and some more text
//                         ^ cursor pos
//       |---- head[] -----|| ---- tail[] ---|
//
//    The text to the left of the cursor is contained in .head[],
//    and the text to the right of the cursor is contained in .tail[]
//
//    End of line:
//       This is some text and some more text
//                                           ^ cursor pos
//       |---- head[] -----------------------||| (tail is empty)
//
//    Start of line:
//       This is some text and some more text
//                                           ^ cursor pos
//      ||---- tail[] -----------------------| (head is empty)
//
//
// ----------------------------------------------------------------------
// Note: While this edit field exposes interfaces for different data types,
// at the core, it all works in "string" space.
// ----------------------------------------------------------------------
// Context Struct values for the text entry session (see header file for def)
//
// head - text before the cursor - values at:  0 to cursor
//        Values added at the cursor.
//
// tail - The text after the cursor -  values at:  bcursor to kEditBufferMax-1
//        If the cursor is moved left, chars are moved from .head to .tail.
//        If the cursor is moved right, chars are moved from .tail to .head
//
// cursor - The current entry position in the text field. Character adds
//          start at .head[0] and move right.
//
// bcursor - The current position to add characters that are right of the
//           cursor. Char adds start at the end of .tail[] and progress back.
//           Adds move right to left
//
#define kBCursorZero (kEditBufferMax - 1)

// Defines for key codes

const uint8_t kCodeBS = 8;
const uint8_t kCodeDEL = 127;
const uint8_t kCodeSpace = 32;
const uint8_t kCodeBell = 7;
const uint8_t kCodeCR = 13;
const uint8_t kCodeEOL = 5;
const uint8_t kCodeBOL = 1;
const uint8_t kCodeKillEOL = 11;
const uint8_t kCodeESC = 27;
const uint8_t kCodeAsterisk = 42;
const uint8_t kCodeESCExtend = 91;
const uint8_t kCodeArrowRight = 67;
const uint8_t kCodeArrowLeft = 68;
const uint8_t kCodeKPDel = 126;

// Sequence to do a backspace and overwrite old text
const char chBackSpace[] = {kCodeBS, kCodeSpace, kCodeBS};
const char chLeftArrow[] = {kCodeESC, kCodeESCExtend, kCodeArrowLeft};
const char chRightArrow[] = {kCodeESC, kCodeESCExtend, kCodeArrowRight};

const uint16_t kInputBufferSize = 32;

//--------------------------------------------------------------------------
// Validators for editing operations
//--------------------------------------------------------------------------

static bool isInt8(char *value)
{
    if (!value)
        return false;

    char *p;
    int32_t tmp = strtol(value, &p, 10);

    // Did the value convert correctly and fall between max and min for the type

    return (*p == 0) && tmp <= std::numeric_limits<int8_t>::max() && tmp >= std::numeric_limits<int8_t>::min();
}
//--------------------------------------------------------------------------
static bool isInt16(char *value)
{
    if (!value)
        return false;

    char *p;
    int32_t tmp = strtol(value, &p, 10);

    // Did the value convert correctly and fall between max and min for the type

    return (*p == 0) && tmp <= std::numeric_limits<int16_t>::max() && tmp >= std::numeric_limits<int16_t>::min();
}
//--------------------------------------------------------------------------
static bool isInt(char *value)
{
    if (!value)
        return false;

    char *p;
    strtol(value, &p, 10);

    return (*p == 0);
}
//--------------------------------------------------------------------------
static bool isUInt8(char *value)
{
    if (!value)
        return false;

    // check for a negative number
    char *p = value;

    // skip white space
    while (*p && isspace(*p))
        p++;

    // no negative numbers
    if (*p == '-')
        return false;

    uint32_t tmp = strtoul(value, &p, 10);

    return (*p == 0) && tmp <= std::numeric_limits<uint8_t>::max() && tmp >= std::numeric_limits<uint8_t>::min();
}

//--------------------------------------------------------------------------
static bool isUInt16(char *value)
{
    if (!value)
        return false;

    // check for a negative number
    char *p = value;

    // skip white space
    while (*p && isspace(*p))
        p++;

    // no negative numbers
    if (*p == '-')
        return false;

    uint32_t tmp = strtoul(value, &p, 10);

    return (*p == 0) && tmp <= std::numeric_limits<uint16_t>::max() && tmp >= std::numeric_limits<uint16_t>::min();
}

//--------------------------------------------------------------------------
static bool isUInt(char *value)
{
    if (!value)
        return false;

    // check for a negative number
    char *p = value;

    // skip white space
    while (*p && isspace(*p))
        p++;

    // no negative numbers
    if (*p == '-')
        return false;

    strtoul(value, &p, 10);

    return (*p == 0);
}
//--------------------------------------------------------------------------
static bool isFloat(char *value)
{
    if (!value)
        return false;

    char *p;
    strtof(value, &p);

    return (*p == 0);
}
//--------------------------------------------------------------------------
static bool isDouble(char *value)
{
    if (!value)
        return false;

    char *p;
    strtod(value, &p);

    return (*p == 0);
}
//--------------------------------------------------------------------------
// Everything is awesome! Always true.
static bool isTrue(char *value)
{
    return true;
}
//--------------------------------------------------------------------------
// drawTrailing()
//
// Draw the text contained in .tail[] in the context.
//
// If isDelete is set, we also need to overwrite end text that is
// invalid b/c of a deleted char.
//
void flxSerialField::drawTrailing(FieldContext_t &ctx, bool isDelete)
{

    // Draw the text after the insertion point if any exists
    int nBack = 0;

    if (ctx.bcursor < kEditBufferMax - 1)
    {
        if (ctx.hidden)
        {
            for (int i = 0; i <= kBCursorZero - ctx.bcursor; i++)
                Serial.write(kCodeAsterisk);
        }
        else
            Serial.write(ctx.tail + ctx.bcursor, kBCursorZero - ctx.bcursor);
        nBack = kBCursorZero - ctx.bcursor; // number of chars written
    }
    // Is delete set? If so, we need to end the line with a space to
    // take into account a deleted char and the change of our insertion
    // point
    if (isDelete)
    {
        Serial.write(kCodeSpace);
        nBack++;
    }

    // Now we need to move our cursor back to our insertion point
    // Issue left arrow commands to do this
    for (int i = 0; i < nBack; i++)
        Serial.write(chLeftArrow, sizeof(chLeftArrow));
}

//--------------------------------------------------------------------------
// resetContext()
//
// Zero/Clear out the context struct.
//
void flxSerialField::resetContext(FieldContext_t &ctx)
{

    memset(&ctx, '\0', sizeof(FieldContext_t));
    ctx.cursor = 0;
    ctx.bcursor = kBCursorZero;
}
//--------------------------------------------------------------------------
// processArrowKeys()
//
// Take actions in respose to left or right arrow keys.
//
// Arrow keys come in as 3 chars, but this method is only passed in
// the last of these chars -- which indicates which arrow was entered

void flxSerialField::processArrowKeys(FieldContext_t &ctxEdit, char inCode)
{

    // left arrow and we have room to move left?
    if (inCode == kCodeArrowLeft && ctxEdit.cursor > 0)
    {
        // move the cursor
        Serial.write(chLeftArrow, sizeof(chLeftArrow));

        // bookkeeping
        ctxEdit.bcursor--;
        ctxEdit.cursor--;
        ctxEdit.tail[ctxEdit.bcursor] = ctxEdit.head[ctxEdit.cursor];
        ctxEdit.head[ctxEdit.cursor] = '\0';
    }
    else if (inCode == kCodeArrowRight && ctxEdit.bcursor < kBCursorZero)
    {
        // Room in our buffer?
        if (ctxEdit.cursor < kEditBufferMax - 2)
        {

            // move the cursor
            Serial.write(chRightArrow, sizeof(chRightArrow));

            // bookkeeping
            ctxEdit.head[ctxEdit.cursor] = ctxEdit.tail[ctxEdit.bcursor];
            ctxEdit.cursor++;
            ctxEdit.tail[ctxEdit.bcursor] = '\0';
            ctxEdit.bcursor++;
        }
    }
}

//--------------------------------------------------------------------------
// processDELKey()
//
// Keypad Delete key (delete the next char)

void flxSerialField::processDELKey(FieldContext_t &ctxEdit)
{

    // If at end of buffer there is nothing to delete ->bell
    if (ctxEdit.bcursor == kBCursorZero)
        Serial.write(kCodeBell);
    else
    {
        // we need to delete the next char in the left buffer
        ctxEdit.tail[ctxEdit.bcursor] = '\0';
        ctxEdit.bcursor++;

        // redraw
        drawTrailing(ctxEdit, true);
    }
}

//--------------------------------------------------------------------------
// processKillToEOL()
//
// Kill to end of line - ^K - old emacs command?!

void flxSerialField::processKillToEOL(FieldContext_t &ctxEdit)
{

    // If at end of buffer there is nothing kill - return
    if (ctxEdit.bcursor == kBCursorZero)
        return;

    // setup our tail to be all blanks
    memset(ctxEdit.tail + ctxEdit.bcursor, kCodeSpace, kBCursorZero - ctxEdit.bcursor);
    drawTrailing(ctxEdit);

    // we need to delete the next char in the left buffer
    memset(ctxEdit.tail, '\0', kEditBufferMax);
    ctxEdit.bcursor = kBCursorZero;
}
//--------------------------------------------------------------------------
// processBackspaceKey()
//
// Backspace key (delete the previous char)

void flxSerialField::processBackspaceKey(FieldContext_t &ctxEdit)
{
    // Not at position 0?
    // Serial.printf("{%d}", ctxEdit.cursor);
    if (ctxEdit.cursor > 0)
    {
        // Send our BS command to the console
        Serial.write(chBackSpace, sizeof(chBackSpace));

        // Delete the previous char in our buffer
        ctxEdit.cursor--;
        ctxEdit.head[ctxEdit.cursor] = '\0';

        // Draw any text after insertion point - pass in
        // delete flag as true to clear out end char on screen
        drawTrailing(ctxEdit, true);
    }
    else                         // At position 0 - bell time
        Serial.write(kCodeBell); // bell;
}

//--------------------------------------------------------------------------
// processEndOfLineKey()
//
// Move cursor to EOL

void flxSerialField::processEndOfLineKey(FieldContext_t &ctxEdit)
{
    // Is the cursor not at the end of the line already?
    if (ctxEdit.bcursor < kBCursorZero)
    {
        // move all .tail[] text into .head[]

        int nBack = kBCursorZero - ctxEdit.bcursor;

        memcpy(ctxEdit.head + ctxEdit.cursor, ctxEdit.tail + ctxEdit.bcursor, nBack);

        ctxEdit.cursor = ctxEdit.cursor + nBack;
        ctxEdit.head[ctxEdit.cursor] = '\0';

        // move the cursor to the end of the line
        for (int i = 0; i < nBack; i++)
            Serial.write(chRightArrow, sizeof(chRightArrow));

        // The tail buffer is now empty
        ctxEdit.bcursor = kBCursorZero;
        memset(ctxEdit.tail, '\0', kEditBufferMax);
    }
}

//--------------------------------------------------------------------------
// processStartOfLineKey()
//
// Move cursor to start of the line

void flxSerialField::processStartOfLineKey(FieldContext_t &ctxEdit)
{
    // Cursor is not at the start already...

    if (ctxEdit.cursor > 0)
    {
        // Copy all text to the tail buffer
        memcpy(ctxEdit.tail + (ctxEdit.bcursor - ctxEdit.cursor), ctxEdit.head, ctxEdit.cursor);

        ctxEdit.bcursor = ctxEdit.bcursor - ctxEdit.cursor;

        // Move the cursor to the start of the line with Left arrow calls.
        for (int i = 0; i < ctxEdit.cursor; i++)
            Serial.write(chLeftArrow, sizeof(chLeftArrow));

        // Head buffer is empty
        ctxEdit.cursor = 0;
        memset(ctxEdit.head, '\0', kEditBufferMax);
    }
}
//--------------------------------------------------------------------------
// fulltext()
//
// Returns the full text line in the provided buffer

void flxSerialField::fulltext(FieldContext_t &ctxEdit, char *buffer, size_t length)
{

    if (ctxEdit.cursor > 0)
        memcpy(buffer, ctxEdit.head, ctxEdit.cursor);

    if (ctxEdit.bcursor < kBCursorZero)
        memcpy(buffer + ctxEdit.cursor, ctxEdit.tail + ctxEdit.bcursor, kBCursorZero - ctxEdit.bcursor);

    buffer[ctxEdit.cursor + kBCursorZero - ctxEdit.bcursor] = '\0';
}
//--------------------------------------------------------------------------
// processText()
//
// Process text entered
//
// Loops over input text and processes standard ascii/no-control chars
//
// Returns the number of characters processed
//
uint16_t flxSerialField::processText(FieldContext_t &ctxEdit, char *inputBuffer, uint length)
{

    uint16_t nProcessed = 0;

    for (int i = 0; i < length; i++)
    {
        // sensible char?
        if (!isascii(inputBuffer[i]) || iscntrl(inputBuffer[i]))
            return nProcessed; // continue;

        // Are we at the end of the buffer?
        if (ctxEdit.cursor + kBCursorZero - ctxEdit.bcursor >= kBCursorZero)
        {
            Serial.write(kCodeBell);
            break;
        }

        // Add to buffer
        ctxEdit.head[ctxEdit.cursor] = inputBuffer[i];
        ctxEdit.cursor++;
        ctxEdit.head[ctxEdit.cursor] = '\0';

        fulltext(ctxEdit, ctxEdit.all);

        if (!ctxEdit.validator(ctxEdit.all))
        {
            Serial.write(kCodeBell);
            // back out
            ctxEdit.cursor--;
            ctxEdit.head[ctxEdit.cursor] = '\0';

            break;
        }
        // For Chars - just write
        Serial.write(inputBuffer[i]);
        if (ctxEdit.hidden)
        {
            delay(10);
            Serial.write(kCodeBS);
            Serial.write(kCodeAsterisk);
        }
        nProcessed++;
    }
    // And text after the insertion point?
    drawTrailing(ctxEdit, false);

    return nProcessed;
}
//--------------------------------------------------------------------------
// editLoop()
//
// Main processing loop
//
// Return Value
//    - true on success/new value
//    - false on error, aborted entry
//
//  timeout - is in secs
//
bool flxSerialField::editLoop(FieldContext_t &ctxEdit, uint32_t timeout)
{

    char inputBuffer[kInputBufferSize];
    uint nInput, nRead;

    // Loop until the user stops (CR/Enter or ESC key), or timeout
    bool returnValue = false;

    timeout = timeout * 1000; // secs to millis

    // for timeout calc
    uint32_t startTime = millis();

    // if there is an existing value in head, write it out
    if (ctxEdit.cursor > 0)
    {
        if (ctxEdit.hidden)
        {
            for (int i = 0; i <= ctxEdit.cursor; i++)
                Serial.write(kCodeAsterisk);
        }
        else
            Serial.write(ctxEdit.head, ctxEdit.cursor);
    }

    uint16_t iInput;
    bool bBreakOut;

    while (true)
    {
        // input?
        nInput = Serial.available();

        if (nInput == 0) // nope
        {
            // timeout expired?
            if (millis() - startTime > timeout)
                break; // end loop

            delay(100);
            continue;
        }

        // check overflow on read
        if (nInput > kInputBufferSize)
            nInput = kInputBufferSize;

        nRead = Serial.readBytes(inputBuffer, nInput);

        if (nRead == 0)
        {
            flxLog_W(F("Error reading from serial device"));
            delay(100);
            continue;
        }

        // if we are here, there was some activity. start timeout again
        startTime = millis();

        // Serial.printf("%d{%d}", nRead, inputBuffer[0]); // for debugging

        // update 3/24 - input can be multiple chars from an escape command, or just fast entry (a
        //       fast CR entry after text was commonly missed in old method).
        //
        //       Added logic to loop over the input buffer for input processing - to handle fast command entry

        for (iInput = 0, bBreakOut = false; !bBreakOut && iInput < nRead; iInput++)
        {
            // Escape key detected?
            if (inputBuffer[iInput] == kCodeESC)
            {
                if (nRead == 1) // normal escape - abort entry
                {
                    bBreakOut = true; // end loop
                    break;
                }

                // An "Escaped Key"
                if (inputBuffer[iInput + 1] == kCodeESCExtend)
                {
                    // Arrow Keys?
                    if (nRead == 3)
                    {
                        processArrowKeys(ctxEdit, inputBuffer[iInput + 2]);
                        iInput += 2;
                    }
                    else if (nRead == 4 && inputBuffer[iInput + 3] == kCodeKPDel) // Keypad delete
                    {
                        processDELKey(ctxEdit);
                        iInput += 3;
                    }
                }
            }
            else if (inputBuffer[iInput] == kCodeDEL || inputBuffer[iInput] == kCodeBS)
            {
                // backspace
                processBackspaceKey(ctxEdit);
            }
            else if (inputBuffer[iInput] == kCodeCR) // CR was entered?
            {
                // move text to all
                fulltext(ctxEdit, ctxEdit.all);
                returnValue = true;
                bBreakOut = true; // end loop
            }
            else if (inputBuffer[iInput] == kCodeEOL) // Move to end of line.
                processEndOfLineKey(ctxEdit);

            else if (inputBuffer[iInput] == kCodeBOL) // Move to start of line
                processStartOfLineKey(ctxEdit);

            else if (inputBuffer[iInput] == kCodeKillEOL) // Kill to EOL
                processKillToEOL(ctxEdit);

            else
            {
                // enter text. Returns the number of chars processed - note -1 since loops incs count
                iInput = iInput + (processText(ctxEdit, inputBuffer, nRead) - 1);
            }
        }
        // Done?
        if (bBreakOut)
            break;

        Serial.flush();
        delay(10);
    }
    return returnValue;
}
//--------------------------------------------------------------------------
// editFieldString()
//
// Public - main entry point - general data entry
//
// editing operation with the provided character string as input

bool flxSerialField::editFieldCString(char *value, size_t lenValue, bool hidden, uint32_t timeout)
{

    if (!value || lenValue == 0)
        return false;

    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);
    ctxEdit.validator = isTrue; // always true/always blue(SV) for text
    ctxEdit.hidden = hidden;

    // copy in our initial value.
    ctxEdit.cursor = strlen(value);
    if (ctxEdit.cursor > 0)
        strlcpy(ctxEdit.head, value, kEditBufferMax);

    // Okay, setup, lets dispatch to the edit loop

    if (editLoop(ctxEdit, timeout))
    {
        // Editing was successful - copy out entered value
        strlcpy(value, ctxEdit.all, lenValue);
        return true;
    }

    // editing wasn't successful.
    return false;
}
//--------------------------------------------------------------------------
bool flxSerialField::editFieldString(std::string &value, bool hidden, uint32_t timeout)
{
    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);
    ctxEdit.validator = isTrue; // always true for text
    ctxEdit.hidden = hidden;

    // copy in our initial value.
    ctxEdit.cursor = value.length();
    if (ctxEdit.cursor > 0)
        strlcpy(ctxEdit.head, value.c_str(), kEditBufferMax);

    // Okay, setup, lets dispatch to the edit loop

    if (editLoop(ctxEdit, timeout))
    {
        // Editing was successful - copy out entered value
        value = ctxEdit.all;

        return true;
    }

    // editing wasn't successful.
    return false;
}

//--------------------------------------------------------------------------
// Bool editor -
bool flxSerialField::editFieldBool(bool &value, bool hidden, uint32_t timeout)
{
    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);

    // Everything is a bool - :)
    //
    //  false  = "", "0"
    //  true   = <anything else> - ideally 1

    ctxEdit.validator = isTrue;
    ctxEdit.hidden = hidden;

    // jam the current value into context head
    snprintf(ctxEdit.head, sizeof(ctxEdit.head), "%d", value ? 1 : 0);
    ctxEdit.cursor = strlen(ctxEdit.head);

    if (editLoop(ctxEdit, timeout))
    {
        if (strlen(ctxEdit.all) > 0)
        {
            char *p;
            // Editing was successful - copy out entered value
            value = (strtol(ctxEdit.all, &p, 10) != 0);

            if (*p != 0) // conversion failed, so something was in the string.
                value = true;
        }
        else
            value = false;
        return true;
    }

    // editing wasn't successful.
    return false;
}
//--------------------------------------------------------------------------
bool flxSerialField::editFieldInt8(int8_t &value, bool hidden, uint32_t timeout)
{
    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);
    ctxEdit.validator = isInt8;
    ctxEdit.hidden = hidden;

    // jam the current value into context head
    snprintf(ctxEdit.head, sizeof(ctxEdit.head), "%d", value);
    ctxEdit.cursor = strlen(ctxEdit.head);

    if (editLoop(ctxEdit, timeout))
    {
        char *p;
        // Editing was successful - copy out entered value
        value = (int8_t)strtol(ctxEdit.all, &p, 10);
        return true;
    }

    // editing wasn't successful.
    return false;
}

//--------------------------------------------------------------------------
bool flxSerialField::editFieldInt16(int16_t &value, bool hidden, uint32_t timeout)
{
    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);
    ctxEdit.validator = isInt16;
    ctxEdit.hidden = hidden;

    // jam the current value into context head
    snprintf(ctxEdit.head, sizeof(ctxEdit.head), "%d", value);
    ctxEdit.cursor = strlen(ctxEdit.head);

    if (editLoop(ctxEdit, timeout))
    {
        char *p;
        // Editing was successful - copy out entered value
        value = (int16_t)strtol(ctxEdit.all, &p, 10);
        return true;
    }

    // editing wasn't successful.
    return false;
}

//--------------------------------------------------------------------------
bool flxSerialField::editFieldInt(int32_t &value, bool hidden, uint32_t timeout)
{
    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);
    ctxEdit.validator = isInt;
    ctxEdit.hidden = hidden;

    // jam the current value into context head
    snprintf(ctxEdit.head, sizeof(ctxEdit.head), "%d", value);
    ctxEdit.cursor = strlen(ctxEdit.head);

    if (editLoop(ctxEdit, timeout))
    {
        char *p;
        // Editing was successful - copy out entered value
        value = strtol(ctxEdit.all, &p, 10);
        return true;
    }

    // editing wasn't successful.
    return false;
}

//--------------------------------------------------------------------------
bool flxSerialField::editFieldUInt8(uint8_t &value, bool hidden, uint32_t timeout)
{
    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);
    ctxEdit.validator = isUInt8;
    ctxEdit.hidden = hidden;

    // jam the current value into context head

    snprintf(ctxEdit.head, sizeof(ctxEdit.head), "%u", value);
    ctxEdit.cursor = strlen(ctxEdit.head);

    if (editLoop(ctxEdit, timeout))
    {
        char *p;
        // Editing was successful - copy out entered value
        value = (uint8_t)strtoul(ctxEdit.all, &p, 10);
        return true;
    }

    // editing wasn't successful.
    return false;
}

//--------------------------------------------------------------------------
bool flxSerialField::editFieldUInt16(uint16_t &value, bool hidden, uint32_t timeout)
{
    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);
    ctxEdit.validator = isUInt16;
    ctxEdit.hidden = hidden;

    // jam the current value into context head

    snprintf(ctxEdit.head, sizeof(ctxEdit.head), "%u", value);
    ctxEdit.cursor = strlen(ctxEdit.head);

    if (editLoop(ctxEdit, timeout))
    {
        char *p;
        // Editing was successful - copy out entered value
        value = (uint16_t)strtoul(ctxEdit.all, &p, 10);
        return true;
    }

    // editing wasn't successful.
    return false;
}

//--------------------------------------------------------------------------
bool flxSerialField::editFieldUInt(uint32_t &value, bool hidden, uint32_t timeout)
{
    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);
    ctxEdit.validator = isUInt;
    ctxEdit.hidden = hidden;

    // jam the current value into context head

    snprintf(ctxEdit.head, sizeof(ctxEdit.head), "%u", value);
    ctxEdit.cursor = strlen(ctxEdit.head);

    if (editLoop(ctxEdit, timeout))
    {
        char *p;
        // Editing was successful - copy out entered value
        value = strtoul(ctxEdit.all, &p, 10);
        return true;
    }

    // editing wasn't successful.
    return false;
}

//--------------------------------------------------------------------------
bool flxSerialField::editFieldFloat(float &value, bool hidden, uint32_t timeout)
{
    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);
    ctxEdit.validator = isFloat;
    ctxEdit.hidden = hidden;

    // jam the current value into context head

    snprintf(ctxEdit.head, sizeof(ctxEdit.head), "%f", value);
    ctxEdit.cursor = strlen(ctxEdit.head);

    if (editLoop(ctxEdit, timeout))
    {
        char *p;
        // Editing was successful - copy out entered value
        value = strtof(ctxEdit.all, &p);
        return true;
    }

    // editing wasn't successful.
    return false;
}

//--------------------------------------------------------------------------
bool flxSerialField::editFieldDouble(double &value, bool hidden, uint32_t timeout)
{
    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);
    ctxEdit.validator = isDouble;
    ctxEdit.hidden = hidden;

    // jam the current value into context head

    snprintf(ctxEdit.head, sizeof(ctxEdit.head), "%f", value);
    ctxEdit.cursor = strlen(ctxEdit.head);

    if (editLoop(ctxEdit, timeout))
    {
        char *p;
        // Editing was successful - copy out entered value
        value = strtod(ctxEdit.all, &p);
        return true;
    }

    // editing wasn't successful.
    return false;
}

void flxSerialField::beep()
{
    Serial.write(kCodeBell); // bell;
}

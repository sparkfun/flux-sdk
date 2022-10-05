

#include <string.h>

#include <Arduino.h>

#include "spCoreLog.h"
#include "spSerialField.h"
//
// ----------------------------------------------------------------------
// Serial terminal data entry field - for use with terminal apps.
//
// Terminal Apps Tested
// 
//  * minicom   - macOS
//  * screen    - macOS   (note: Control-A doesn't work - its a screen command cocde)
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
//  * Carrage Ret - Accept changes, return results
//  * "Bell"      - rings terminal bell on error/warning
//
// ----------------------------------------------------------------------
// How this works:
//
//    If you have have a line of text, with a cursor in the middle
//
//       This is some text and some more text
//                         ^ cursor pos
//       |---- head[] -----|| ---- tail[] ---|
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
// ----------------------------------------------------------------------
// Context values for the text entry session
//
// head - text before the cursor - values at:  0 to cursor
//        Values added at the cusor.
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
#define kBCursorZero  (kEditBufferMax-1) 

// Defines for keycodes

#define kCodeBS      8
#define kCodeDEL     127
#define kCodeSpace   32
#define kCodeBell    7
#define kCodeCR      13
#define kCodeEOL     5
#define kCodeBOL     1
#define kCodeKillEOL 11
#define kCodeESC     27

#define kCodeESCExtend  91
#define kCodeArrowRight 67
#define kCodeArrowLeft  68
#define kCodeKPDel      126

// Sequence to do a backspace and key out old text
const char chBackSpace[]  = {kCodeBS, kCodeSpace, kCodeBS};
const char chLeftArrow[]  = { kCodeESC, kCodeESCExtend, kCodeArrowLeft};
const char chRightArrow[] = { kCodeESC, kCodeESCExtend, kCodeArrowRight};

#define kInputBufferSize 32



void spSerialField::drawTrailing(FieldContext_t & ctx, bool isDelete) 
{

    // Draw the text after the insertion point if any exists
    int nBack =0;

    if (ctx.bcursor < kEditBufferMax - 1) {

        Serial.write( ctx.tail + ctx.bcursor, kBCursorZero - ctx.bcursor);
        nBack =  kBCursorZero - ctx.bcursor;  // number of chars written
    }
    // Is delete set ? If so, we need to end the line with a space to
    // take into account a deleted char and the change of our insertion
    // point
    if (isDelete){
        Serial.write(kCodeSpace);
        nBack++;
    }

    // Now we need to move our cursor back to our insertion point
    // Issue left arrow commands to do this  
    for (int i = 0; i < nBack; i++)
        Serial.write(chLeftArrow, sizeof(chLeftArrow));
}


//--------------------------------------------------------------------------
void spSerialField::resetContext( FieldContext_t & ctx) 
{

    memset(&ctx, '\0', sizeof(FieldContext_t));
    ctx.cursor=0;
    ctx.bcursor = kBCursorZero;
}
//--------------------------------------------------------------------------
//
void spSerialField::processArrowKeys(FieldContext_t &ctxEdit, char inCode)
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
        if (ctxEdit.cursor < kEditBufferMax - 2) {

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
// 
// Keypad Delete key (delete the next char)

void spSerialField::processDELKey(FieldContext_t &ctxEdit)
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
// 
// Kill to end of line - ^K 

void spSerialField::processKillToEOL(FieldContext_t &ctxEdit)
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
// 
// Backspace key (delete the previous char)

void spSerialField::processBackspaceKey(FieldContext_t &ctxEdit) 
{
    // Not at position 0?
    //Serial.printf("{%d}", ctxEdit.cursor);
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
    else                       // At position 0 - bell time
        Serial.write(kCodeBell); // bell;
}

//--------------------------------------------------------------------------
// 
// EOL key 

void spSerialField::processEndOfLineKey(FieldContext_t &ctxEdit) 
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
// 
// Start of Line key 

void spSerialField::processStartOfLineKey(FieldContext_t &ctxEdit) 
{
    // Cursor is not at the start already...

    if (ctxEdit.cursor > 0) 
    {
        // Copy all text to the tail buffer
        memcpy(ctxEdit.tail + (ctxEdit.bcursor - ctxEdit.cursor), ctxEdit.head,
               ctxEdit.cursor);


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
// 
// Start of Line key 

void spSerialField::processText(FieldContext_t &ctxEdit, char * inputBuffer, uint length) 
{
    
    for ( int i =0; i < length; i++ )
    {
        // sensible char?
        if ( !isascii( inputBuffer[i]) )
            continue;

        // Are we at the end of the buffer?
        if ( ctxEdit.cursor +  kBCursorZero - ctxEdit.bcursor >= kBCursorZero)
        {
            Serial.write(kCodeBell);
            break;
        }
        
        // For Chars - just write
        Serial.write(inputBuffer[i]);

        // Add to buffer
        ctxEdit.head[ctxEdit.cursor] = inputBuffer[i];
        ctxEdit.cursor++;
        ctxEdit.head[ctxEdit.cursor] = '\0';

    }
    // And text after the insertion point?
    drawTrailing(ctxEdit,false);

}
//--------------------------------------------------------------------------
// editLoop()
// 
// Main processing loop
//
// Return Value
//    - true on success/new value
//    - false on error, aboarted entry
//
//  timeout - is in secs
//
bool spSerialField::editLoop(FieldContext_t &ctxEdit, uint32_t timeout)
{

    char inputBuffer[kInputBufferSize];
    uint nInput, nRead;

    // Loop until the user stops (CR/Enter or ESC key)

    bool returnValue = false;

    timeout = timeout * 1000;

    // for timeout cal
    uint32_t startTime = millis();
    while ( true )
    {
        // input?
        nInput = Serial.available();

        if ( nInput == 0 ) // nope
        {
            // timeout expired
            if (millis() - startTime > timeout)
                break;

            delay(100);
            continue;
        }

        if (nInput > kInputBufferSize)
            nInput = kInputBufferSize;

        nRead = Serial.readBytes(inputBuffer, nInput);

        if ( nRead == 0) 
        {
            // TODO - issue warning
            spLog_W("Error reading from serial device");
            delay(100);
            continue;
        }

        // if we are here, there was some activity. 
        startTime = millis();

        //Serial.printf("{%d}", inputBuffer[0]);

        // Escape key detected?
        if ( inputBuffer[0] == kCodeESC)
        {
            if ( nRead == 1 ) // normal escape - abort entry
                break;

            // An "Escaped Key"
            if (inputBuffer[1] == kCodeESCExtend)
            {
                // Arrow Keys?
                if ( nRead == 3 )
                    processArrowKeys(ctxEdit, inputBuffer[2]);

                else if ( nRead == 4 && inputBuffer[3] == kCodeKPDel ) // Keypad delete
                    processDELKey( ctxEdit );
            }        
        }
        else if (inputBuffer[0] == kCodeDEL || inputBuffer[0] == kCodeBS){  // backspace

            processBackspaceKey(ctxEdit);    
        }
        else if ( inputBuffer[0] == kCodeCR)    // CR was entered?
        {
            // move any text after the insertion point into the head buffer
            if ( ctxEdit.bcursor < kBCursorZero)
            {
                memcpy(ctxEdit.head + ctxEdit.cursor, ctxEdit.tail + ctxEdit.bcursor,
                            kBCursorZero - ctxEdit.bcursor);

                ctxEdit.cursor = strlen(ctxEdit.head);
                ctxEdit.bcursor = kBCursorZero;
            }
            returnValue = true;

            break;
        }
        else if ( inputBuffer[0] == kCodeEOL)    // Move to end of line.
            processEndOfLineKey(ctxEdit);

        else if ( inputBuffer[0] == kCodeBOL)    // Move to start of line
            processStartOfLineKey(ctxEdit);

        else if ( inputBuffer[0] == kCodeKillEOL)    // Kill to eol
            processKillToEOL(ctxEdit);            

        else
            processText(ctxEdit, inputBuffer, nRead);

        Serial.flush();    
        delay(10);
    }
    return returnValue;
}
//--------------------------------------------------------------------------
// editField()
//
// editing operation with the provided character string as input

bool spSerialField::editField(char *value, size_t lenValue, uint32_t timeout)
{

    if ( !value || lenValue == 0 )
        return false;

    // setup context
    FieldContext_t ctxEdit;

    resetContext(ctxEdit);

    // copy in our initial value.
    ctxEdit.cursor = strlen(value);    
    if ( ctxEdit.cursor > 0 )
    {
        strlcpy(ctxEdit.head, value, kEditBufferMax);
        Serial.write(ctxEdit.head, ctxEdit.cursor);
    }
    // Okay, setup, lets dispatch to the editloop

    if ( editLoop(ctxEdit, timeout) )
    {
        // Editing was successful 
        strlcpy(value, ctxEdit.head, lenValue);
        return true;
    }

    // editing wasn't successful.
    return false; 
}





#include <ctype.h>
#include "spSettingsSerial.h"

#define kOutputBufferSize 128

//-----------------------------------------------------------------------------
// Draw Page
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool spSettingsSerial::drawPage(spObject* pCurrent)
{
	if ( !pCurrent )
		return false;

	uint8_t selected = 0;
	int nMenuItems;

	while ( true )
	{
		drawPageHeader(pCurrent);

		nMenuItems = drawMenu(pCurrent, 0);
		if( nMenuItems ==0 )
			Serial.println("No Entries");
		else if (nMenuItems < 0)
		{
			Serial.println("Error generating menu entries.");
			spLog_E("Error generating menu entries");
			return false;
		}

		drawPageFooter(pCurrent);

		selected = getMenuSelection((uint)nMenuItems);

		// done?
		if ( isEscape(selected))
			break;

		selectMenu(pCurrent, selected);


	}
	return true;
}
//-----------------------------------------------------------------------------
bool spSettingsSerial::drawPage(spObject* pCurrent, spProperty* pProp)
{
	if ( !pCurrent )
		return false;

	uint8_t selected = 0;

	while ( true )
	{
		drawPageHeader(pCurrent);

		Serial.println("Property Page Test - press 'b' [back]  for now");

		drawPageFooter(pCurrent);

		// done?
		if ( isEscape(selected))
			break;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool spSettingsSerial::drawPage(spOperation* pCurrent)
{
	if ( !pCurrent )
		return false;

	uint8_t selected = 0;

	int nMenuItems;

	while ( true )
	{
		drawPageHeader(pCurrent);

		nMenuItems = drawMenu(pCurrent, 0);
		
		if( nMenuItems ==0 )
			Serial.println("No Entries");
		else if (nMenuItems < 0)
		{
			Serial.println("Error generating menu entries.");
			spLog_E("Error generating menu entries");
			return false;
		}
		drawPageFooter(pCurrent);

		selected = getMenuSelection((uint)nMenuItems);

		// done?
		if ( isEscape(selected))
			break;

	}

	return true;
}
//-----------------------------------------------------------------------------
bool spSettingsSerial::drawPage(spOperation* pCurrent, spParameter* pParam)
{
	if ( !pCurrent )
		return false;

	uint8_t selected = 0;

	while ( true )
	{
		drawPageHeader(pCurrent);

		// Draw a parameter page ....
		Serial.println("Parameter Page Test - press 'b' [back]  for now");

		drawPageFooter(pCurrent);


		// done?
		if ( isEscape(selected))
			break;

	}

	return true;
}
// Container typed wrappers that use the container template for all the work
bool spSettingsSerial::drawPage(spObjectContainer* pCurrent)
{

	return drawPage<spObject*>(pCurrent);
}
//-----------------------------------------------------------------------------
bool spSettingsSerial::drawPage(spOperationContainer* pCurrent)
{

	return drawPage<spOperation*>(pCurrent);
}
//-----------------------------------------------------------------------------
bool spSettingsSerial::drawPage(spDeviceContainer* pCurrent)
{

	return drawPage<spDevice*>(pCurrent);
}
//-----------------------------------------------------------------------------
bool spSettingsSerial::drawPage(spActionContainer* pCurrent)
{

	return drawPage<spAction*>(pCurrent);
}
void spSettingsSerial::drawPageHeader(spObject *pCurrent)
{

	char szBuffer[kOutputBufferSize]={0};
	char szOutput[kOutputBufferSize]={0};

  	Serial.println();

  	// build a "breadcrumb" string...
  	do 
  	{
  		snprintf(szBuffer, kOutputBufferSize, "/%s", pCurrent->name());
  		if ( strlen(szOutput) > 0 )
	  		strlcat( szBuffer, szOutput, kOutputBufferSize);

	  	strlcpy( szOutput, szBuffer, kOutputBufferSize);  		

  		pCurrent = pCurrent->parent();
  	}while(pCurrent);

  	Serial.print("Settings for: ");
  	Serial.println(szOutput);
  	Serial.println();
}

void spSettingsSerial::drawPageFooter(spObject *pCurrent)
{
	Serial.println();

	if ( pCurrent->parent() )
		Serial.printf("\tb) Back\n\r");
	else
		Serial.printf("\tx) Exit\n\r");


	Serial.printf("\n\rSelect Option: ");
	
}

//-----------------------------------------------------------------------------
// Draw Menu  routines
//-----------------------------------------------------------------------------

void spSettingsSerial::drawMenuEntry(uint item, spDescriptor *pDesc)
{

	// TODO: Change in the future
	if ( !pDesc )
		return;

	Serial.printf("\t%2d)  %s - %s\n\r", item, pDesc->name(), pDesc->description());
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

int spSettingsSerial::drawMenu( spObject *pCurrent, uint level){


	if ( !pCurrent )
		return -1;

	// Draw the menu for the spObject - loop over the properties for the object
	// and draw menu entries.

	if ( pCurrent->nProperties() == 0 )
		return level;

	Serial.println("Settings:");
	for ( auto prop : pCurrent->getProperties())
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


int spSettingsSerial::selectMenu( spObject *pCurrent, uint level){


	if ( !pCurrent )
		return -1;

	// Is the selected item here - a property?
	if ( level  > pCurrent->nProperties() ) 
		return level + pCurrent->nProperties();  // no

	// Get the property targeted and start a new page on it.

	spProperty * theProp = pCurrent->getProperties().at(level-1);

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


int spSettingsSerial::selectMenu( spOperation *pCurrent, uint level){


	if ( !pCurrent )
		return -1;

	// First, cascade to the spObject portion of the menu

	int returnLevel = selectMenu( (spObject*)pCurrent, level);

	if ( returnLevel < 0 ) 
		return returnLevel;    // error happened

	// was this handled ?
	if ( returnLevel <= level ) 
		return returnLevel;      

	// Output Param?

	if ( level <= returnLevel + pCurrent->nOutputParameters() )
	{
		auto outParam = pCurrent->getOutputParameters().at(level - returnLevel - 1);

		drawPage(pCurrent, outParam);

		return level;
	} 
	else
		returnLevel += pCurrent->nOutputParameters();

	if ( level <= returnLevel + pCurrent->nInputParameters() )
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

int spSettingsSerial::drawMenu( spOperation *pCurrent, uint level){


	if ( !pCurrent )
		return -1;

	// First, cascade to the spObject portion of the menu

	int returnLevel = drawMenu( (spObject*)pCurrent, level);

	if ( returnLevel < 0 ) 
		return returnLevel;    // error happened

	level = returnLevel;

	// Loop over our input and output parameters

	if ( pCurrent->nOutputParameters() > 0 )
	{
		Serial.println("Output Parameters:");

		for( auto outParam : pCurrent->getOutputParameters())
		{
			level++;
			drawMenuEntry(level, outParam);
		}
	}

	if ( pCurrent->nInputParameters() > 0 )
	{
		Serial.println();
		Serial.println("Input Parameters:");

		for( auto inParam : pCurrent->getInputParameters())
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
int spSettingsSerial::drawMenu(spObjectContainer* pCurrent, uint level)
{

	return drawMenu<spObject*>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::drawMenu(spOperationContainer* pCurrent, uint level)
{

	return drawMenu<spOperation*>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::drawMenu(spDeviceContainer* pCurrent, uint level)
{

	return drawMenu<spDevice*>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::drawMenu(spActionContainer* pCurrent, uint level)
{

	return drawMenu<spAction*>(pCurrent, level);
}
//-----------------------------------------------------------------------------
// Select Menus
//-----------------------------------------------------------------------------
// Container typed wrappers that use the container template for all the work
int spSettingsSerial::selectMenu(spObjectContainer* pCurrent, uint level)
{

	return selectMenu<spObject*>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::selectMenu(spOperationContainer* pCurrent, uint level)
{

	return selectMenu<spOperation*>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::selectMenu(spDeviceContainer* pCurrent, uint level)
{

	return selectMenu<spDevice*>(pCurrent, level);
}
//-----------------------------------------------------------------------------
int spSettingsSerial::selectMenu(spActionContainer* pCurrent, uint level)
{

	return selectMenu<spAction*>(pCurrent, level);
}
//-----------------------------------------------------------------------------
uint8_t spSettingsSerial::getMenuSelection(uint maxEntry, uint timeout)
{

 	// TODO - abstract out serial calls.
 	Serial.flush();

 	// delay from open log Artemis

 	delay(500);

 	// clear buffer
 	while( Serial.available()  > 0 )
 		Serial.read();


 	timeout = timeout * 1000;
 	unsigned long startTime = millis();

 	uint8_t chIn;
 	uint value;
 	while ( true )
 	{
 		if ( Serial.available() > 0)
 		{

 			chIn = Serial.read();

 			Serial.print("CHar is:"); Serial.println((int)(chIn - '0'));
 			// if it's a number, or an escape letter(set by this app) drop out of loop
 			if ( isEscape(chIn) )
 				break;
 			else if ( isdigit(chIn) )
 			{
 				value = chIn - '0';
 				if ( value > 0 && value <= maxEntry )
 				break;
 			}
 		}

 		// Timeout?
 		if ( (millis() - startTime) > timeout)
 		{
 			Serial.println("No user input recieved.");
 			chIn = kReadBufferTimoutExpired;
 			break;
 		}
 		delay(100);
 	}
 	Serial.println("Leaving read");
 	return chIn;
}

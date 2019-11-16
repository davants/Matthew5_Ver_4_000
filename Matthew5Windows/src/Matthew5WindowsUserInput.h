#pragma once

#include <vector>


namespace Matthew5
{
	namespace Windows
	{

		// This shows what is happening at this moment. 
		// This may not be the best for typing because keys could be missed because this 
		// only shows at this moment and if they typed a key and then let go all 
		// before a check was made then it will not show up here.		
		struct Matthew5WindowsUserHoldingKeys 
		{	
			friend class Matthew5Windows;

			// For letters upper or lower case, check the Key upper case. 'A', not 'a'. 
			// Meaning, if user clicks lower 'a' or upper 'A', are both true for CharHit('A') check.
			// CharHit('a') will never return true regardless of user actions.
			// To determine upper you must check the Shift member as well.
			bool CharHit(char c);

			bool UpArrow, LeftArrow, DownArrow, RightArrow;
			bool Control, Escape,SpaceBar, Shift,   Tab;			
			bool CloseProgram = false;
			bool MouseLeft, MouseMiddle, MouseRight;
			int  MouseClickX, MouseClickY;

			// Mouse current position
			int MouseCurrentX;
			int MouseCurrentY;


			// How much has the mouse moved since the start of the click and hold.
			// This will not be reset after letting go of the mouse click.
			// Meaning this can be old data so only valid right after a user 
			// lets up on a mouse button.
			int MouseClickAndHoldMoveX, MouseClickAndHoldMoveY;

			// How much has the mouse moved since last check.
			int GetMouseMoveX(void);
			// How much has the mouse moved since last check.
			int GetMouseMoveY(void);

		private:
			// Keep these private. Logic must update them.
			// These are mouse movement since last last check.
			int MouseMoveX, MouseMoveY;

			// For letters upper or lower case, check  the Key upper case. 'A', not 'a'. 
			// Meaning, if user clicks lower 'a' or upper 'A', are both true for Key['A'] check.
			// To determine upper you must check the Shift member as well.
			bool Key[256];
		};
	}
}


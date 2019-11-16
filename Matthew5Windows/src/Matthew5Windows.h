#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <string>

#include "Matthew5WindowsUserInput.h"

namespace Matthew5
{
	namespace Windows
	{
		// This is a simple wrapper class for Microsoft Windows. This provides one window and a 
		// mouse, keyboard and other window actions.
		// The client should call the GetWindowsMessages() function to empty the windows message 
		// queue and get user input.
		// The only thing a graphics component needs is a target, so this class provides the 
		// GetHWND() method to get the one window HWND (handle).
		class Matthew5Windows
		{
		public:  
			Matthew5Windows(HINSTANCE vHINSTANCE, int vWindowWidth, int vWindowHeight);

			// This will empty the windows message queue and return user input.
			// This should be called continually followed by whatever app logic 
			// is needed.
			Matthew5WindowsUserHoldingKeys & GetWindowsMessages(void);

			// The client will need a target windown.
			HWND GetHWND(void);


			// Not needed by the client, but need to be public for MS Windows reasons.
			static Matthew5Windows* GetMatthew5WindowsApplication();
			virtual LRESULT MessageProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		protected:
			static Matthew5Windows* mpMatthew5Windows;
		private:

			int mWindowWidth;
			int mWindowHeight;

			HINSTANCE mHINSTANCE = nullptr;
			HWND      mHWND = nullptr;

			Matthew5WindowsUserHoldingKeys mMatthew5WindowsUserHoldingKeys;

			void SetHelperUserHoldingKeys(void);

			POINT mLastMousePosition;
			Matthew5WindowsUserHoldingKeys & GetUserHoldingKeys(void);
			bool InitializeWindow(void);

		};
	}

}



/*
//// EXAMPLE CODE of using the Matthew5WindowsUserHoldingKeys that is returned by GetWindowsMessages;
	 bool LClick = false;

	 while (true)
	 {
		 Matthew5WindowsUserHoldingKeys & vUserInput = vMatthew5Windows.GetWindowsMessages();

		 if (vUserInput.CloseProgram == true)
		 {
			 break;
		 }
		 if (vUserInput.MouseLeft)
		 {
			 LClick = true;
		 }

		 if(LClick && vUserInput.MouseLeft == false)
		 {
			 // User clicked mouse and has now stopped.
			 std::string vText = "\nHold - X  moved " + std::to_string(vUserInput.MouseClickAndHoldMoveX)
				 + " Y moved " + std::to_string(vUserInput.MouseClickAndHoldMoveY) + "\n";

			 ::OutputDebugStringA(vText.c_str());

			 LClick = false;
		 }


		 if (vUserInput.MouseLeft)
		 {
			 int tempx = vUserInput.GetMouseMoveX();
			 int tempy = vUserInput.GetMouseMoveY();

			 if (tempx != 0 || tempy != 0)
			 {
				 std::string vText = "\n Left is down (";
				 vText += std::to_string(tempx) + "," + std::to_string(tempy) + ")";
				 ::OutputDebugStringA(vText.c_str());
			 }
		 }

	}
*/
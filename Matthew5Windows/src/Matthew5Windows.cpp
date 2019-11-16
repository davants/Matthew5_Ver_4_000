#include "Matthew5Windows.h"

LRESULT CALLBACK
WindowsProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Matthew5::Windows::Matthew5Windows::GetMatthew5WindowsApplication()->MessageProcedure(hwnd, msg, wParam, lParam);
}

namespace Matthew5
{
	namespace Windows
	{

		Matthew5Windows* Matthew5Windows::mpMatthew5Windows = nullptr;

		Matthew5Windows::Matthew5Windows(HINSTANCE vHINSTANCE, int vWindowWidth, int vWindowHeight):
			mHINSTANCE(vHINSTANCE),
			mWindowWidth(vWindowWidth),
			mWindowHeight(vWindowHeight)
		{
			mpMatthew5Windows = this;	

			mLastMousePosition.x = 0;
			mLastMousePosition.y = 0;

			InitializeWindow();
		}

		Matthew5WindowsUserHoldingKeys & Matthew5Windows::GetWindowsMessages(void)
		{
			MSG msg = { 0 };

			while (true)
			{
				if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT)
					{
						mMatthew5WindowsUserHoldingKeys.CloseProgram = true;
						return GetUserHoldingKeys();
					}

					// There is a windows message, so process it.
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					// No more messages in the windows queue so return to the client so they can call draw.
					return  GetUserHoldingKeys();
				}
			}
		}

		Matthew5WindowsUserHoldingKeys & Matthew5Windows::GetUserHoldingKeys(void)
		{
			return mMatthew5WindowsUserHoldingKeys;
		}

		HWND Matthew5Windows::GetHWND(void)
		{
			return mHWND;
		}

		Matthew5Windows * Matthew5Windows::GetMatthew5WindowsApplication()
		{
			return mpMatthew5Windows;
		}

		LRESULT Matthew5Windows::MessageProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			switch (msg)
			{			

			case WM_KEYDOWN:
				mMatthew5WindowsUserHoldingKeys.Key[wParam] = true;								
				break;
			case WM_KEYUP:
				mMatthew5WindowsUserHoldingKeys.Key[wParam] = false;
				break;


	

			case WM_ACTIVATE:
				if (LOWORD(wParam) == WA_INACTIVE)
				{
				}
				else
				{
				}
				return 0;

			case WM_SIZE:
			case WM_ENTERSIZEMOVE:
				return 0;

			case WM_EXITSIZEMOVE:
				return 0;

				// WM_DESTROY is sent when the window is being destroyed.
			case WM_DESTROY:
				PostQuitMessage(0);
				mMatthew5WindowsUserHoldingKeys.CloseProgram = true;
				return 0;

				// The WM_MENUCHAR message is sent when a menu is active and the user presses 
				// a key that does not correspond to any mnemonic or accelerator key. 
			case WM_MENUCHAR:
				// Don't beep when we alt-enter.
				return MAKELRESULT(0, MNC_CLOSE);

				// Catch this message so to prevent the window from becoming too small.
			case WM_GETMINMAXINFO:
				((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
				((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
				return 0;

			case WM_LBUTTONDOWN:
				mMatthew5WindowsUserHoldingKeys.MouseLeft = true;
				mMatthew5WindowsUserHoldingKeys.MouseClickX = GET_X_LPARAM(lParam);
				mMatthew5WindowsUserHoldingKeys.MouseClickY = GET_Y_LPARAM(lParam);

				// Reset Last Mouse Position
				mLastMousePosition.x = GET_X_LPARAM(lParam);
				mLastMousePosition.y = GET_Y_LPARAM(lParam);
				SetCapture(mHWND);

				break;
			case WM_MBUTTONDOWN:
				mMatthew5WindowsUserHoldingKeys.MouseMiddle = true;
				mMatthew5WindowsUserHoldingKeys.MouseClickX = GET_X_LPARAM(lParam);
				mMatthew5WindowsUserHoldingKeys.MouseClickY = GET_Y_LPARAM(lParam);

				// Reset Last Mouse Position
				mLastMousePosition.x = GET_X_LPARAM(lParam);
				mLastMousePosition.y = GET_Y_LPARAM(lParam);
				SetCapture(mHWND);

				break;
			case WM_RBUTTONDOWN:
				mMatthew5WindowsUserHoldingKeys.MouseRight = true;
				mMatthew5WindowsUserHoldingKeys.MouseClickX = GET_X_LPARAM(lParam);
				mMatthew5WindowsUserHoldingKeys.MouseClickY = GET_Y_LPARAM(lParam);

				// Reset Last Mouse Position
				mLastMousePosition.x = GET_X_LPARAM(lParam);
				mLastMousePosition.y = GET_Y_LPARAM(lParam);
				SetCapture(mHWND);

				break;

				return 0;
			case WM_LBUTTONUP:
				mMatthew5WindowsUserHoldingKeys.MouseLeft = false;
				ReleaseCapture();
				break;

			case WM_MBUTTONUP:
				mMatthew5WindowsUserHoldingKeys.MouseMiddle = false;
				ReleaseCapture();
				break;

			case WM_RBUTTONUP:
				mMatthew5WindowsUserHoldingKeys.MouseRight = false;
				ReleaseCapture();
				break;

			case WM_MOUSEMOVE:

				mMatthew5WindowsUserHoldingKeys.MouseCurrentX = GET_X_LPARAM(lParam);
				mMatthew5WindowsUserHoldingKeys.MouseCurrentY = GET_Y_LPARAM(lParam);

				if ((wParam & MK_LBUTTON) != 0)
				{

					mMatthew5WindowsUserHoldingKeys.MouseClickAndHoldMoveX = GET_X_LPARAM(lParam) - mMatthew5WindowsUserHoldingKeys.MouseClickX;
					mMatthew5WindowsUserHoldingKeys.MouseClickAndHoldMoveY = GET_Y_LPARAM(lParam) - mMatthew5WindowsUserHoldingKeys.MouseClickY;
				}
				else if ((wParam & MK_RBUTTON) != 0)
				{
					mMatthew5WindowsUserHoldingKeys.MouseClickAndHoldMoveX = GET_X_LPARAM(lParam) - mMatthew5WindowsUserHoldingKeys.MouseClickX;
					mMatthew5WindowsUserHoldingKeys.MouseClickAndHoldMoveY = GET_Y_LPARAM(lParam) - mMatthew5WindowsUserHoldingKeys.MouseClickY;
				}

				mLastMousePosition.x = GET_X_LPARAM(lParam);;
				mLastMousePosition.y = GET_Y_LPARAM(lParam);;

				break;
			}

			SetHelperUserHoldingKeys();

			return DefWindowProc(hwnd, msg, wParam, lParam);

		}

		bool Matthew5Windows::InitializeWindow(void)
		{
			WNDCLASS wc;
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WindowsProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = mHINSTANCE;
			wc.hIcon = LoadIcon(0, IDI_APPLICATION);
			wc.hCursor = LoadCursor(0, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
			wc.lpszMenuName = 0;
			wc.lpszClassName = L"MainWnd";

			if (!RegisterClass(&wc))
			{
				MessageBox(0, L"RegisterClass Failed.", 0, 0);
				return false;
			}

			// Compute window rectangle dimensions based on requested client area dimensions.
			RECT R = { 0, 0, mWindowWidth, mWindowHeight };
			AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
			int width = R.right - R.left;
			int height = R.bottom - R.top;

			std::wstring vCaption = L"Matthew5";

			mHWND = CreateWindow(L"MainWnd", vCaption.c_str(),
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mHINSTANCE, 0);

			if (!mHWND)
			{
				MessageBox(0, L"CreateWindow Failed.", 0, 0);
				return false;
			}

			ShowWindow(mHWND, SW_SHOW);
			UpdateWindow(mHWND);

			return true;
		}

		void Matthew5Windows::SetHelperUserHoldingKeys(void)
		{
			// Control Btn
			if (mMatthew5WindowsUserHoldingKeys.Key[VK_CONTROL]) { 
				mMatthew5WindowsUserHoldingKeys.Control = true;
			}
			else {
				mMatthew5WindowsUserHoldingKeys.Control = false;
			}

			// Escape
			if (mMatthew5WindowsUserHoldingKeys.Key[VK_ESCAPE]) {
				mMatthew5WindowsUserHoldingKeys.Escape = true;
			}
			else {
				mMatthew5WindowsUserHoldingKeys.Escape = false;
			}

			// UpArrow
			if (mMatthew5WindowsUserHoldingKeys.Key[VK_UP]) {
				mMatthew5WindowsUserHoldingKeys.UpArrow = true;
			}
			else {
				mMatthew5WindowsUserHoldingKeys.UpArrow = false;
			}

			// DownArrow
			if (mMatthew5WindowsUserHoldingKeys.Key[VK_DOWN]) {
				mMatthew5WindowsUserHoldingKeys.DownArrow = true;
			}
			else {
				mMatthew5WindowsUserHoldingKeys.DownArrow = false;
			}

			// LeftArrow
			if (mMatthew5WindowsUserHoldingKeys.Key[VK_LEFT]) {
				mMatthew5WindowsUserHoldingKeys.LeftArrow = true;
			}
			else {
				mMatthew5WindowsUserHoldingKeys.LeftArrow = false;
			}

			// RightArrow
			if (mMatthew5WindowsUserHoldingKeys.Key[VK_RIGHT]) {
				mMatthew5WindowsUserHoldingKeys.RightArrow = true;
			}
			else {
				mMatthew5WindowsUserHoldingKeys.RightArrow = false;
			}

			// Space Bar
			if (mMatthew5WindowsUserHoldingKeys.Key[VK_SPACE]) {
				mMatthew5WindowsUserHoldingKeys.SpaceBar = true;
			}
			else {
				mMatthew5WindowsUserHoldingKeys.SpaceBar = false;
			}

			// Shift Bar
			if (mMatthew5WindowsUserHoldingKeys.Key[VK_SHIFT]) {
				mMatthew5WindowsUserHoldingKeys.Shift = true;
			}
			else {
				mMatthew5WindowsUserHoldingKeys.Shift = false;
			}		
		

			// Tab
			if (mMatthew5WindowsUserHoldingKeys.Key[VK_TAB]) {
				mMatthew5WindowsUserHoldingKeys.Tab = true;
			}
			else {
				mMatthew5WindowsUserHoldingKeys.Tab = false;
			}

		}

	}
}
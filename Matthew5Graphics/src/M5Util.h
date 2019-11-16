#pragma once

#include <iostream>
#include <fstream>      // std::ifstream
#include <windows.h>
#include <string>
#include "Matthew5StandardMicrosoftHeaders.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		static const float MyPi = 3.1415926536f;

		class M5Util
		{
		public:		

			static DirectX::XMFLOAT4X4 Identity();

			// This will search for a file or path and return the full 
			// path if it can find it. A zero length string is returned 
			// if nothing is found.
			// Example: pass "Shaders/Default.hlsl" and get back 
			//				"c:/modules/Matthew5/Shaders/Default.hlsl"
			// Of course an another system this example path would be different.
			static std::wstring FindFullPath(const std::wstring & vSearch);

			// Overloaded version to support std::string.
			static std::string FindFullPath(const std::string& vSearch);

			// Simple Util function to see if a file exists.
			static bool DoesFileExist(const std::wstring & fileName);

			// Simple Util function that returns the location of the running application.
			static std::wstring ExePath();

			// Simple Util function to output the XMFLOAT4X4 to the debug window.
			static void DebugOutputXMFloat4x4(std::string vName, DirectX::XMFLOAT4X4 & vrXMFLOAT4X4);

			// Convert from a std::wstring string to a std::string.
			static std::string StringConvert(const std::wstring& vrWideString);

			// Convert from a std::string string to a std::wstring.
			static std::wstring StringConvert(const std::string& vrString);
		};
	}
}
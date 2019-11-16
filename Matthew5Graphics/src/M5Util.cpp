#include "M5Util.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{

		DirectX::XMFLOAT4X4 M5Util::Identity()
		{
			static DirectX::XMFLOAT4X4 vIdentity(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			return vIdentity;
		}

		std::wstring M5Util::FindFullPath(const std::wstring & vSearch)
		{
			// Get current path
			std::wstring vCurrentPath = ExePath();

			std::wstring vTryPath = vCurrentPath + L"\\" + vSearch;

			if (DoesFileExist(vTryPath))
			{
				return vTryPath;
			}

			// Go Down One Level and try again.
			size_t vPosition = vCurrentPath.find_last_of(L"\\/");

			vCurrentPath = vCurrentPath.substr(0, vPosition);
			vTryPath = vCurrentPath + L"\\" + vSearch;

			if (DoesFileExist(vTryPath))
			{
				return vTryPath;
			}

			// Go Down One Level and try again.
			vPosition = vCurrentPath.find_last_of(L"\\/");
			vCurrentPath = vCurrentPath.substr(0, vPosition);
			vTryPath = vCurrentPath + L"\\" + vSearch;

			if (DoesFileExist(vTryPath))
			{
				return vTryPath;
			}

			return L"";
		}

		std::string M5Util::FindFullPath(const std::string& vSearch)
		{
			std::wstring vWideSearch = M5Util::StringConvert(vSearch);

			std::wstring  vWideFullPath = M5Util::FindFullPath(vWideSearch);

			return M5Util::StringConvert(vWideFullPath);
		}

		bool M5Util::DoesFileExist(const std::wstring & fileName)
		{
			try
			{
				std::ifstream vInfile(fileName);
				return vInfile.good();
			}
			catch (...)
			{
				return false;
			}
		}

		std::wstring M5Util::ExePath()
		{
			wchar_t vBuffer[MAX_PATH];
			GetModuleFileName(NULL, vBuffer, MAX_PATH);
			std::string::size_type vPosition = std::wstring(vBuffer).find_last_of(L"\\/");
			return std::wstring(vBuffer).substr(0, vPosition);
		}

		void M5Util::DebugOutputXMFloat4x4(std::string vName, DirectX::XMFLOAT4X4 & vrXMFLOAT4X4)
		{
			{

				std::string vStr = "\n" + vName + " "
					+ std::to_string(vrXMFLOAT4X4._11)
					+ "," + std::to_string(vrXMFLOAT4X4._12)
					+ "," + std::to_string(vrXMFLOAT4X4._13)
					+ "," + std::to_string(vrXMFLOAT4X4._14)
					+ "," + std::to_string(vrXMFLOAT4X4._21)
					+ "," + std::to_string(vrXMFLOAT4X4._22)
					+ "," + std::to_string(vrXMFLOAT4X4._23)
					+ "," + std::to_string(vrXMFLOAT4X4._24)
					+ "," + std::to_string(vrXMFLOAT4X4._31)
					+ "," + std::to_string(vrXMFLOAT4X4._32)
					+ "," + std::to_string(vrXMFLOAT4X4._33)
					+ "," + std::to_string(vrXMFLOAT4X4._34)
					+ "," + std::to_string(vrXMFLOAT4X4._41)
					+ "," + std::to_string(vrXMFLOAT4X4._42)
					+ "," + std::to_string(vrXMFLOAT4X4._43)
					+ "," + std::to_string(vrXMFLOAT4X4._44);

				::OutputDebugStringA(vStr.c_str());
			}
		}

		std::string M5Util::StringConvert(const std::wstring& vrWideString)
		{
			std::string vNewString(vrWideString.begin(), vrWideString.end());
			return vNewString;
		}

		std::wstring M5Util::StringConvert(const std::string& vrString)
		{
			std::wstring vNewString(vrString.begin(), vrString.end());
			return vNewString;
		}

	}
}
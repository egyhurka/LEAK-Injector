#pragma once
#include <Windows.h>
#include <string>

namespace injector
{
	class Injector
	{
	public:
		void LoadProcessId(DWORD processId);
		void LoadDllPath(const std::wstring& path);

		[[nodiscard]] bool Inject();
		[[nodiscard]] SIZE_T BytesWritten() const noexcept { return bytesWritten; }
		[[nodiscard]] SIZE_T TotalBytes() const noexcept { return dllPathSize; }

	private:
		DWORD processId = 0;
		SIZE_T dllPathSize = 0;
		SIZE_T bytesWritten = 0;
		std::wstring dllPath;
	};
}

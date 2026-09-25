#pragma once
#include <Windows.h>
#include <string>

namespace injector
{
	enum InjectionMethod
	{
		Standard_LoadLibrary = 0,
		Thread_Hijacking = 1,
		Manual_Mapping = 2
	};

	class Injector
	{
	public:
		void LoadProcessId(DWORD processId);
		void LoadDll(const std::wstring& path);

		[[nodiscard]] bool Inject(InjectionMethod method = Standard_LoadLibrary);
		[[nodiscard]] SIZE_T BytesWritten() const noexcept { return bytesWritten; }
		[[nodiscard]] SIZE_T TotalBytes() const noexcept { return dllPathSize; }

	private:
		DWORD processId = 0;
		SIZE_T dllPathSize = 0;
		SIZE_T bytesWritten = 0;
		std::wstring dllPath;

		[[nodiscard]] bool StandardLoadLibraryInjection();
		[[nodiscard]] bool ThreadHijackingInjection();
		[[nodiscard]] bool ManualMappingInjection();
	};
}

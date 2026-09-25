#include "Mem.h"

uintptr_t leak::mem::GetModuleBase()
{
	return (uintptr_t)GetModuleHandle(NULL);
}

uintptr_t leak::mem::FindDMAAddy(uintptr_t ptr, const std::vector<DWORD>& offsets)
{
	uintptr_t addr = ptr;
	for (size_t i = 0; i < offsets.size(); i++)
	{
		__try
		{
			addr = *(uintptr_t*)addr;
			addr += offsets[i];
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return 0;
		}
	}
	return addr;
}

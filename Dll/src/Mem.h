#pragma once

#include <windows.h>
#include <vector>
#include <cstring>
#include <type_traits>
#include <cstdint>

namespace leak::mem
{
	uintptr_t GetModuleBase();
	uintptr_t FindDMAAddy(uintptr_t ptr, const std::vector<DWORD>& offsets);

	template<typename T>
	bool TryRead(const std::uintptr_t address, T& value)
	{
        static_assert(std::is_trivially_copyable_v<T>);

        if (address == 0)
            return false;

        __try
        {
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(T));
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
	}

    template <typename T>
    bool TryWrite(const std::uintptr_t address, const T& value) noexcept
    {
        static_assert(std::is_trivially_copyable_v<T>);

        if (address == 0)
            return false;

        __try
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(T));
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
    }
}
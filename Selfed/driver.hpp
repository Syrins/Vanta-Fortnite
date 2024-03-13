#pragma once
#include <Windows.h>
#include <TlHelp32.h>
uintptr_t virtualaddy;

#define code_rw CTL_CODE(FILE_DEVICE_UNKNOWN, 0x71, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_ba CTL_CODE(FILE_DEVICE_UNKNOWN, 0x72, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_get_guarded_region CTL_CODE(FILE_DEVICE_UNKNOWN, 0x73, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_security 0x85b3e12

typedef struct _rw {
	INT32 security;
	INT32 process_id;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
	BOOLEAN write;
} rw, * prw;

typedef struct _ba {
	INT32 security;
	INT32 process_id;
	ULONGLONG* address;
} ba, * pba;

typedef struct _ga {
	INT32 security;
	ULONGLONG* address;
} ga, * pga;

namespace mem {
	HANDLE driver_handle;
	INT32 process_id;
	HANDLE process_id1;
	uintptr_t BaseAddy;
	bool find_driver() {
		SPOOF_FUNC
		driver_handle = CreateFileW((L"\\\\.\\\MemoryControl"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
			return false;

		return true;
	}

	void read_physical(PVOID address, PVOID buffer, DWORD size) {
		SPOOF_FUNC
		_rw arguments = { 0 };

		arguments.security = code_security;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = FALSE;

		DeviceIoControl(driver_handle, code_rw, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	void write_physical(PVOID address, PVOID buffer, DWORD size) {
		SPOOF_FUNC
		_rw arguments = { 0 };

		arguments.security = code_security;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = TRUE;

		DeviceIoControl(driver_handle, code_rw, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	void read_physical1(uint64_t address, void* buffer, uint32_t size) {
		SPOOF_FUNC
		_rw arguments = { 0 };

		arguments.security = code_security;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = FALSE;

		DeviceIoControl(driver_handle, code_rw, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	void read_physical2(LPCVOID address, LPVOID* buffer, SIZE_T size) {
		SPOOF_FUNC
			_rw arguments = { 0 };

		arguments.security = code_security;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = FALSE;

		DeviceIoControl(driver_handle, code_rw, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	void ReadPtr(ULONG64 Address, PVOID pBuffer, DWORD Size)
	{
		SPOOF_FUNC
		_rw arguments = { 0 };

		arguments.security = code_security;
		arguments.address = (ULONGLONG)Address;
		arguments.buffer = (ULONGLONG)pBuffer;
		arguments.size = Size;
		arguments.process_id = process_id;
		arguments.write = FALSE;

		DeviceIoControl(driver_handle, code_rw, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}


	inline bool read1(const std::uintptr_t address, void* buffer, const std::size_t size)
	{
		SPOOF_FUNC
		if (buffer == nullptr || size == 0) {
			return false; // Invalid parameters
		}

		read_physical(reinterpret_cast<PVOID>(address), buffer, static_cast<DWORD>(size));

		// You need to add some error checking here based on how your 'readm' function signals success
	}

	uintptr_t find_image() {
		SPOOF_FUNC
		uintptr_t image_address = { NULL };
		_ba arguments = { NULL };

		arguments.security = code_security;
		arguments.process_id = process_id;
		arguments.address = (ULONGLONG*)&image_address;

		DeviceIoControl(driver_handle, code_ba, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return image_address;
	}

	uintptr_t get_guarded_region() {
		SPOOF_FUNC
		uintptr_t guarded_region_address = { NULL };
		_ga arguments = { NULL };

		arguments.security = code_security;
		arguments.address = (ULONGLONG*)&guarded_region_address;

		DeviceIoControl(driver_handle, code_get_guarded_region, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return guarded_region_address;
	}

	INT32 find_process(LPCTSTR process_name) {
		SPOOF_FUNC
		PROCESSENTRY32 pt;
		HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hsnap, &pt)) {
			do {
				if (!lstrcmpi(pt.szExeFile, process_name)) {
					CloseHandle(hsnap);
					process_id = pt.th32ProcessID;
					return pt.th32ProcessID;
				}
			} while (Process32Next(hsnap, &pt));
		}
		CloseHandle(hsnap);

		return { NULL };
	}
	
}

template <typename T>
T read(uint64_t address) {
	SPOOF_FUNC
	T buffer{ };

	mem::read_physical((PVOID)address, &buffer, sizeof(T));
	return buffer;
}

template <typename T>
T write(uint64_t address, T buffer) {
	SPOOF_FUNC

	mem::write_physical((PVOID)address, &buffer, sizeof(T));
	return buffer;
}

template<typename T>
bool read_array(uintptr_t address, T out[], size_t len)

{
	SPOOF_FUNC
	for (size_t i = 0; i < len; ++i)
	{
		out[i] = read<T>(address + i * sizeof(T));
	}
	return true; // you can add additional checks to verify successful reads
}






bool is_valid(const uint64_t adress)
{
	SPOOF_FUNC
	if (adress <= 0x400000 || adress == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(adress) == nullptr || adress >
		0x7FFFFFFFFFFFFFFF) {
		return false;
	}

	return true;
}
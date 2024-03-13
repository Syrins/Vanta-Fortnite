#include <d3d9.h>
#include <dwmapi.h>
#include "SKD.h"
#include "protect\xorstr.h"
#include "protect/lazy.h"
#include "protect/skCrypt.h"
//#include "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\ucrt\string.h"
namespace DirectX9Interface
{
    IDirect3D9Ex* p_object = NULL;
    IDirect3DDevice9Ex* p_device = NULL;
    D3DPRESENT_PARAMETERS p_params = { NULL };
    RECT game_rect = { NULL };
    MSG message = { NULL };
    HWND hijacked_hwnd = NULL;
    HWND game_wnd = NULL;
}

namespace hijack
{
    bool amd = true;
    bool nvidia = false;

    __forceinline void hijack_init()
    {
        if (LI_FN(FindWindowA)(skCrypt("MedalOverlayClass"), skCrypt("MedalOverlay")))
        {
            SPOOF_FUNC
                amd = true;
            DirectX9Interface::hijacked_hwnd = LI_FN(FindWindowA)(skCrypt("MedalOverlayClass"), skCrypt("MedalOverlay"));
            NOTIFYICONDATA nid = {};
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = NULL;
            nid.uID = 1;
            nid.uFlags = NIF_INFO;
            nid.dwInfoFlags = NIIF_INFO;

            Shell_NotifyIcon(NIM_ADD, &nid);
            Shell_NotifyIcon(NIM_DELETE, &nid);
        }
        else
        {
            SPOOF_FUNC
                NOTIFYICONDATA nid = {};
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = NULL;
            nid.uID = 1;
            nid.uFlags = NIF_INFO;
            nid.dwInfoFlags = NIIF_INFO;


            MessageBox(NULL, "MEDAL not found!", NULL, MB_OK);
            Shell_NotifyIcon(NIM_ADD, &nid);
            Shell_NotifyIcon(NIM_DELETE, &nid);
            exit(1);
        }

        LI_FN(ShowWindow)(DirectX9Interface::hijacked_hwnd, SW_SHOW);

        LI_FN(SetWindowLongA)(DirectX9Interface::hijacked_hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
        LI_FN(SetWindowLongA)(
            DirectX9Interface::hijacked_hwnd,
            -20,
            static_cast<LONG_PTR>(
                static_cast<int>(LI_FN(GetWindowLongA)(DirectX9Interface::hijacked_hwnd, -20)) | 0x20
                )
            );

        if (amd) LI_FN(MoveWindow)(DirectX9Interface::hijacked_hwnd, 0, 0, screen_width, screen_height, false); // only when amd

        //transparency
        MARGINS margin = { -1, -1, -1, -1 };
        LI_FN(DwmExtendFrameIntoClientArea)(
            DirectX9Interface::hijacked_hwnd,
            &margin
            );

        LI_FN(SetLayeredWindowAttributes)(
            DirectX9Interface::hijacked_hwnd,
            NULL,
            0xFF,
            0x02
            );

        // top most
        LI_FN(SetWindowPos)(
            DirectX9Interface::hijacked_hwnd,
            HWND_TOPMOST,
            0, 0, 0, 0,
            0x0002 | 0x0001
            );


        LI_FN(UpdateWindow)(DirectX9Interface::hijacked_hwnd);

    }

}

namespace SetUp {

    inline HWND gWnd{}, DrawWnd{};

#define MAX_CLASSNAME 255
#define MAX_WNDNAME 255
#define TRANSPARENCY_COLOR RGB(0, 254, 0)

    struct WindowsFinderParams {
        DWORD pidOwner = NULL;
        std::string wndClassName = "";
        std::string wndName = "";
        RECT pos = { 0, 0, 0, 0 };
        POINT res = { 0, 0 };
        float percentAllScreens = 0.0f;
        float percentMainScreen = 0.0f;
        DWORD style = NULL;
        DWORD styleEx = NULL;
        bool satisfyAllCriteria = false;
        std::vector<HWND> hwnds;
    };

    // Prototypes
    inline std::vector<HWND> WindowsFinder(WindowsFinderParams params);
    inline BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);
    inline HWND HiJackNotepadWindow();
    inline std::vector<DWORD> GetPIDs(std::wstring targetProcessName);

    inline int amain() {
        SPOOF_FUNC;
        HWND hwnd = HiJackNotepadWindow();
        if (!hwnd) {

            return EXIT_FAILURE;
        }

        HDC hdc = GetDC(hwnd);

        // Getting settings of back buffer bitmap
        DEVMODE devMode;
        devMode.dmSize = sizeof(devMode);
        EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
        BITMAPINFO backBufferBmpInfo;
        backBufferBmpInfo = { 0 };
        backBufferBmpInfo.bmiHeader.biBitCount = devMode.dmBitsPerPel;
        backBufferBmpInfo.bmiHeader.biHeight = GetSystemMetrics(SM_CYSCREEN);
        backBufferBmpInfo.bmiHeader.biWidth = GetSystemMetrics(SM_CXSCREEN);
        backBufferBmpInfo.bmiHeader.biPlanes = 1;
        backBufferBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

        void* backBufferPixels = nullptr;
        POINT res = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
        HBRUSH bgTransparencyColor = CreateSolidBrush(TRANSPARENCY_COLOR);

        int i = -1;

        while (true) {

            i = (i > res.x) ? 0 : ++i; // To simulate movement

            // Frame preparation
            HDC hdcBackBuffer = CreateCompatibleDC(hdc); // Create back buffer
            HBITMAP hbmBackBuffer = CreateDIBSection(hdcBackBuffer, (BITMAPINFO*)&backBufferBmpInfo, DIB_RGB_COLORS, (void**)&backBufferPixels, NULL, 0); // Create back buffer bitmap
            DeleteObject(SelectObject(hdcBackBuffer, hbmBackBuffer));
            DeleteObject(SelectObject(hdcBackBuffer, bgTransparencyColor));
            Rectangle(hdcBackBuffer, 0, 0, res.x, res.y);


            // Frame presentation
            BitBlt(hdc, 0, 0, res.x, res.y, hdcBackBuffer, 0, 0, SRCCOPY);

            // Cleanup
            DeleteDC(hdcBackBuffer); // Delete back buffer device context
            DeleteObject(hbmBackBuffer); // Delete back buffer bitmap
            backBufferPixels = nullptr;
        }

        return EXIT_SUCCESS;
    }

    inline HWND HiJackNotepadWindow() {
        SPOOF_FUNC;
        HWND hwnd = NULL;

        // Remove previous windows
        std::vector<DWORD> existingNotepads = GetPIDs(E(L"notepad.exe").decrypt());
        if (!existingNotepads.empty()) {
            for (int i(0); i < existingNotepads.size(); ++i) {
                // Terminating processes
                HANDLE hOldProcess = OpenProcess(PROCESS_TERMINATE, FALSE, existingNotepads[i]);
                TerminateProcess(hOldProcess, 0);
                CloseHandle(hOldProcess);
            }
        }

        LI_FN(system)(E("start notepad")); // Start notepad, and not as child process, so easy :)

        // Finding notepad's window (we could just use FindWindow but then it would be OS language dependent)
        std::vector<DWORD> notepads = GetPIDs(E(L"notepad.exe").decrypt());
        if (notepads.empty() || notepads.size() > 1) // Should check if more than one to be more strict
            return hwnd;
        WindowsFinderParams params;
        params.pidOwner = notepads[0];
        params.style = WS_VISIBLE;
        params.satisfyAllCriteria = true;
        std::vector<HWND> hwnds;
        int attempt = 0; // The process takes a bit of time to initialise and spawn the window, will try during 5 sec before time out
        while (hwnd == NULL || attempt > 50) {
            LI_FN(Sleep)(100);
            hwnds = WindowsFinder(params);
            if (hwnds.size() > 1)
                return hwnd;
            hwnd = hwnds[0];
            ++attempt;
        }
        if (!hwnd)
            return hwnd;

        // Making the window usable for overlay puposes

        SetMenu(hwnd, NULL);
        SetWindowLongPtr(hwnd, GWL_STYLE, WS_VISIBLE);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT); // WS_EX_NOACTIVATE  and WS_EX_TOOLWINDOW removes it from taskbar

        SetWindowPos_Spoofed(hwnd, NULL, 0, 0, GetSystemMetrics_Spoofed(SM_CXSCREEN), GetSystemMetrics_Spoofed(SM_CYSCREEN), SWP_SHOWWINDOW);
        screen_width = GetSystemMetrics_Spoofed(SM_CXSCREEN);
        screen_height = GetSystemMetrics_Spoofed(SM_CYSCREEN);


        return hwnd;
    }

    inline std::vector<DWORD> GetPIDs(std::wstring targetProcessName) {
        SPOOF_FUNC;
        std::vector<DWORD> pids;
        if (targetProcessName == L"")
            return pids;
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W entry;
        entry.dwSize = sizeof entry;
        if (!Process32FirstW(snap, &entry)) {
            CloseHandle(snap);
            return pids;
        }
        do {
            if (std::wstring(entry.szExeFile) == targetProcessName) {
                pids.emplace_back(entry.th32ProcessID);
            }
        } while (Process32NextW(snap, &entry));
        CloseHandle(snap);
        return pids;
    }

    inline BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
        SPOOF_FUNC;
        WindowsFinderParams& params = *(WindowsFinderParams*)lParam;

        unsigned char satisfiedCriteria = 0, unSatisfiedCriteria = 0;

        // If looking for windows of a specific PDI
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (params.pidOwner != NULL)
            if (params.pidOwner == pid)
                ++satisfiedCriteria; // Doesn't belong to the process targeted
            else
                ++unSatisfiedCriteria;

        // If looking for windows of a specific class
        char className[MAX_CLASSNAME] = "";
        GetClassNameA(hwnd, className, MAX_CLASSNAME);
        std::string classNameWstr = className;
        if (params.wndClassName != "")
            if (params.wndClassName == classNameWstr)
                ++satisfiedCriteria; // Not the class targeted
            else
                ++unSatisfiedCriteria;

        // If looking for windows with a specific name
        char windowName[MAX_WNDNAME] = "";
        GetClassNameA(hwnd, windowName, MAX_CLASSNAME);
        std::string windowNameWstr = windowName;
        if (params.wndName != "")
            if (params.wndName == windowNameWstr)
                ++satisfiedCriteria; // Not the class targeted
            else
                ++unSatisfiedCriteria;

        // If looking for window at a specific position
        RECT pos;
        GetWindowRect(hwnd, &pos);
        if (params.pos.left || params.pos.top || params.pos.right || params.pos.bottom)
            if (params.pos.left == pos.left && params.pos.top == pos.top && params.pos.right == pos.right && params.pos.bottom == pos.bottom)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        // If looking for window of a specific size
        POINT res = { pos.right - pos.left, pos.bottom - pos.top };
        if (params.res.x || params.res.y)
            if (res.x == params.res.x && res.y == params.res.y)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        // If looking for windows taking more than a specific percentage of all the screens
        float ratioAllScreensX = res.x / GetSystemMetrics(SM_CXSCREEN);
        float ratioAllScreensY = res.y / GetSystemMetrics(SM_CYSCREEN);
        float percentAllScreens = ratioAllScreensX * ratioAllScreensY * 100;
        if (params.percentAllScreens != 0.0f)
            if (percentAllScreens >= params.percentAllScreens)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        // If looking for windows taking more than a specific percentage or the main screen
        RECT desktopRect;
        GetWindowRect(GetDesktopWindow(), &desktopRect);
        POINT desktopRes = { desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top };
        float ratioMainScreenX = res.x / desktopRes.x;
        float ratioMainScreenY = res.y / desktopRes.y;
        float percentMainScreen = ratioMainScreenX * ratioMainScreenY * 100;
        if (params.percentMainScreen != 0.0f)
            if (percentAllScreens >= params.percentMainScreen)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        // Looking for windows with specific styles
        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
        if (params.style)
            if (params.style & style)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        // Looking for windows with specific extended styles
        LONG_PTR styleEx = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (params.styleEx)
            if (params.styleEx & styleEx)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        if (!satisfiedCriteria)
            return TRUE;

        if (params.satisfyAllCriteria && unSatisfiedCriteria)
            return TRUE;

        // If looking for multiple windows
        params.hwnds.push_back(hwnd);
        return TRUE;
    }

    inline std::vector<HWND> WindowsFinder(WindowsFinderParams params) {
        SPOOF_FUNC;
        EnumWindows(EnumWindowsCallback, (LPARAM)&params);
        return params.hwnds;
    }
}

bool SetupWindow()
{
    SPOOF_FUNC
        while (!DirectX9Interface::hijacked_hwnd) {
            DirectX9Interface::hijacked_hwnd = SetUp::HiJackNotepadWindow();

        }

    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(DirectX9Interface::hijacked_hwnd, &margin);
    SetMenu(DirectX9Interface::hijacked_hwnd, NULL);
    SetWindowLongPtr(DirectX9Interface::hijacked_hwnd, GWL_STYLE, WS_VISIBLE);
    SetWindowLongPtr(DirectX9Interface::hijacked_hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

    ShowWindow_Spoofed(DirectX9Interface::hijacked_hwnd, SW_SHOW);
    UpdateWindow_Spoofed(DirectX9Interface::hijacked_hwnd);

    SetWindowLongA_Spoofed(DirectX9Interface::hijacked_hwnd, GWL_EXSTYLE, GetWindowLongA_Spoofed(DirectX9Interface::hijacked_hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);

    SetWindowPos_Spoofed(DirectX9Interface::hijacked_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    return true;
}
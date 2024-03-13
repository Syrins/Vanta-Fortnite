#include <Windows.h>
#include <iostream>
#include <consoleapi3.h>
#include <direct.h>
#include <d3d9.h>
#include "D3DX/d3dx9tex.h"

#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "skd.h"
#include "imgui/imgui_internal.h"
#include <map>
#include <iostream>
#include "protect/antiDbg.h"
#include "protect/SDK/obsidium64.h"
#include "protect/blowfish/blowfish.h"
#include "main.h"
#include "draw.h"
#include "OVERLAY.h"
#include "esp.hpp"
#include "grdv/binary/bytes.h"
#include "grdv/binary/dropper.h"
#include <urlmon.h>
#include <tchar.h>
#include "imgui/custom.hpp"
#include "auth.h"
#pragma comment(lib, "urlmon.lib")

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)(__DATE__);
const std::string compilation_time = (std::string)(__TIME__);



void ColorPicker(const char* name, ImVec4& color) {
	bool open_popup = ImGui::ColorButton((std::string(name) + std::string("##3b")).c_str(), color);
	if (open_popup) {
		ImGui::OpenPopup(name);
	}
	if (ImGui::BeginPopup(name)) {
		ImGui::ColorPicker4((std::string(name) + std::string("##picker")).c_str(), (float*)&color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs);
		ImGui::EndPopup();
	}
}


auto rndmTitle() -> void {
	constexpr int length = 15;
	const auto characters = TEXT("2s119sf3f59gga60473wxyzABCfDEFGHIJKLMN");
	TCHAR title[length + 1]{};

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(100, 199);

	for (int j = 0; j < length; j++) {
		title[j] += characters[dis(gen)];
	}

	SetConsoleTitle(title);
}
auto SetConsoleSize() -> void {
	rndmTitle();
	HANDLE hOut;
	SMALL_RECT DisplayArea = { 0, 0, 0, 0 };
	int x = 90;
	int y = 20;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DisplayArea.Right = x;
	DisplayArea.Bottom = y;
	SetConsoleWindowInfo(hOut, TRUE, &DisplayArea);
}

DWORD eah(LPCWSTR processName) {
	SPOOF_FUNC
		HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	DWORD procID = NULL;

	if (handle == INVALID_HANDLE_VALUE)
		return procID;

	PROCESSENTRY32W entry = { 0 };
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(handle, &entry)) {
		if (!_wcsicmp(processName, entry.szExeFile)) {
			procID = entry.th32ProcessID;
		}
		else while (Process32NextW(handle, &entry)) {
			if (!_wcsicmp(processName, entry.szExeFile)) {
				procID = entry.th32ProcessID;
			}
		}
	}

	CloseHandle(handle);
	return procID;
}


static void Overlay();
static void D3D();
void RenderLoop();
static void Shutdown();
static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND hwnd = NULL;


RECT GameRect = { NULL };
D3DPRESENT_PARAMETERS d3dpp;

static HWND Window = NULL;
IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;
ImFont* LargeFont;
ImFont* SmallFont;

ImFont* m_pFont;

ImFont* bold;
ImFont* tab_title;
ImFont* combo_arrow;
ImFont* FN;

ImFont* medium;

IDirect3DTexture9* texture4;




ImVec2 p;

static int tabs = 1;

namespace Render
{
	struct vec_2
	{
		int x, y;
	};

	struct Colors
	{
		ImColor red = { 255, 115, 20, 255 };
		ImColor green = { 0, 255, 115, 20 };
		ImColor blue = { 0, 136, 255, 255 };
		ImColor aqua_blue = { 0, 255, 255, 255 };
		ImColor cyan = { 0, 210, 210, 255 };
		ImColor royal_purple = { 102, 0, 255, 255 };
		ImColor dark_pink = { 255, 0, 174, 255 };
		ImColor black = { 0, 0, 0, 255 };
		ImColor white = { 255, 255, 255, 255 };
		ImColor purple = { 255, 115, 20, 255 };
		ImColor yellow = { 255, 255, 115, 20 };
		ImColor orange = { 255, 140, 0, 255 };
		ImColor gold = { 234, 255, 115, 20 };
		ImColor royal_blue = { 0, 30, 255, 255 };
		ImColor dark_red = { 150, 5, 5, 255 };
		ImColor dark_green = { 5, 150, 5, 255 };
		ImColor dark_blue = { 100, 100, 255, 255 };
		ImColor navy_blue = { 0, 73, 168, 255 };
		ImColor light_gray = { 200, 200, 200, 255 };
		ImColor dark_gray = { 150, 15255, 115, 20 };
	};
	Colors color;

	void Text(int posx, int posy, ImColor clr, const char* text)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
	}

	void OutlinedText(int posx, int posy, ImColor clr, const char* text)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx + 1, posy + 1), ImColor(color.black), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx - 1, posy - 1), ImColor(color.black), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx + 1, posy + 1), ImColor(color.black), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx - 1, posy - 1), ImColor(color.black), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
	}

	void ShadowText(int posx, int posy, ImColor clr, const char* text)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx + 1, posy + 2), ImColor(0, 0, 0, 200), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx + 1, posy + 2), ImColor(0, 0, 0, 200), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
	}

	void Rect(int x, int y, int w, int h, ImColor color, int thickness)
	{
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0, thickness);
	}

	void RectFilledGradient(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void RectFilled(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void Checkbox(const char* v, bool* option, float x, float y, const char* title)
	{
		ImGui::SetCursorPos({ x, y });
		ImGui::Checkbox(v, option);

		p = ImGui::GetWindowPos();

		ImGui::SetCursorPos({ x + 2, y - 20 });
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 245, 245)));
		ImGui::Text(title);
		ImGui::PopStyleColor();

		Render::RectFilled(p.x + x + 3, p.y + y + 3, 16, 16, ImColor(200, 200, 200));

		if (*option)
		{
			Render::RectFilled(p.x + x + 7, p.y + y + 7, 8, 8, ImColor(0, 0, 0, 255));
		}
	}


	void Tab(const char* v, float size_x, float size_y, static int tab_name, int tab_next)
	{
		if (ImGui::Button(v, ImVec2{ size_x, size_y })) tab_name = tab_next;
	}
}


bool  ShowMenu = true;




template<class T, class U>
inline static T clamp(const T& in, const U& low, const U& high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}



static const char* bonename[] =
{
	"Head",
	"Neck",
	"Chest"
};

int selectedOption = 0;
bool showSubMenuWindow = false;


float pos = 40;

int item = 0;



namespace Preview
{
	namespace box3d {
		float topleftfront_x = -72.f;
		float topleftfront_y = 82.f;

		float topleftback_x = -54.f;
		float topleftback_y = 61.f;

		float toprightfront_x = 61.f;
		float toprightfront_y = 82.f;

		float toprightback_x = 74.f;
		float toprightback_y = 61.f;

		float bottomleftfront_x = -85.f;
		float bottomleftfront_y = 335.f;

		float bottomleftback_x = -67.f;
		float bottomleftback_y = 305.f;

		float bottomrightfront_x = 49.f;
		float bottomrightfront_y = 342.f;

		float bottomrightback_x = 64.f;
		float bottomrightback_y = 312.f;
	}

	float head_bone_x = 0;
	float head_bone_y = 94;

	float neck_bone_x = 4;
	float neck_bone_y = 114;

	float chest_bone_x = 2;
	float chest_bone_y = 122;

	float left_shoulder_bone_x = -24;
	float left_shoulder_bone_y = 124;

	float left_elbow_bone_x = -37;
	float left_elbow_bone_y = 153;

	float left_hand_bone_x = -51;
	float left_hand_bone_y = 196;

	float right_hand_bone_x = 47;
	float right_hand_bone_y = 190;

	float right_elbow_bone_x = 43;
	float right_elbow_bone_y = 167;

	float right_shoulder_bone_x = 20;
	float right_shoulder_bone_y = 124;

	float pelvis_bone_x = -6;
	float pelvis_bone_y = 184;

	float left_thigh_bone_x = -22;
	float left_thigh_bone_y = 186;

	float right_thigh_bone_x = 12;
	float right_thigh_bone_y = 186;

	float right_knee_bone_x = 25;
	float right_knee_bone_y = 253;

	float left_knee_bone_x = -37;
	float left_knee_bone_y = 245;

	float right_foot_bone_x = 31;
	float right_foot_bone_y = 310;

	float left_foot_bone_x = -47;
	float left_foot_bone_y = 308;

	float right_footfinger_bone_x = 37;
	float right_footfinger_bone_y = 324;

	float left_footfinger_bone_x = -57;
	float left_footfinger_bone_y = 322;

}


void RenderBox(bool ForeGround, float OffsetW, float OffsetH, int X, int Y, int W, int H, ImVec4 Color, int thickness, bool Filled, bool NoCorner) {
	float lineW = (W / OffsetW);
	float lineH = (H / OffsetH);
	ImDrawList* p = ImGui::GetBackgroundDrawList();
	if (ForeGround)
		p = ImGui::GetForegroundDrawList();

	if (Filled) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, 40);
		DrawFilledRect(ForeGround, X, Y, W, H, Converted);
	}
	if (NoCorner) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, Color.w * 255);
		DrawRect(ForeGround, X, Y, W, H, Converted);
	}
	else {
		//oben links nach unten
		p->AddLine(ImVec2(X, Y - 1), ImVec2(X, Y + lineH), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//oben links nach rechts (l-mittig)
		p->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//oben rechts (r-mittig) nach rechts
		p->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W + 2, Y), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//oben rechts nach vert-rechts (oberhalb)
		p->AddLine(ImVec2(X + W, Y - 1), ImVec2(X + W, Y + lineH), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//unten vert-links (unterhalb) nach unten links
		p->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//unten links nach rechts (mittig)
		p->AddLine(ImVec2(X - 1, Y + H), ImVec2(X + lineW + 1, Y + H), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//unten rechts (mittig) nach rechts
		p->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W + 2, Y + H), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//unten rechts nach vert-rechts (unterhalb)
		p->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);
	}
}

void RenderBox2(bool ForeGround, float OffsetW, float OffsetH, int X, int Y, int W, int H, ImVec4 Color, int thickness, bool Filled, bool NoCorner) {
	float lineW = (W / OffsetW);
	float lineH = (H / OffsetH);
	ImDrawList* p = ImGui::GetBackgroundDrawList();
	if (ForeGround)
		p = ImGui::GetForegroundDrawList();

	if (Filled) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, 40);
		DrawFilledRect(ForeGround, X, Y, W, H, Converted);
	}
	if (NoCorner) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, Color.w * 255);
		DrawRect(ForeGround, X, Y, W, H, Converted);
	}
	else {
		ImVec2 topLeft = ImVec2(X, Y);
		ImVec2 bottomRight = ImVec2(X + W, Y + H);

		// Calculate the radius for the rounded corners (adjust this value as needed)
		float cornerRadius = 5.0f;

		// Draw the rounded rectangle box
		p->AddRect(topLeft, bottomRight, ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), cornerRadius, ImDrawCornerFlags_All, thickness);
	}
}

void RenderBox1(bool ForeGround, float OffsetW, float OffsetH, int X, int Y, int W, int H, ImVec4 Color, int thickness, bool Filled, bool NoCorner) {
	float lineW = (W / OffsetW);
	float lineH = (H / OffsetH);
	ImDrawList* p = ImGui::GetBackgroundDrawList();
	if (ForeGround)
		p = ImGui::GetForegroundDrawList();

	if (Filled) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, 40);
		DrawFilledRect(ForeGround, X, Y, W, H, Converted);
	}
	if (NoCorner) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, Color.w * 255);
		DrawRect(ForeGround, X, Y, W, H, Converted);
	}
	else {
		ImVec2 topLeft = ImVec2(X, Y);
		ImVec2 topRight = ImVec2(X + W, Y);
		ImVec2 bottomLeft = ImVec2(X, Y + H);
		ImVec2 bottomRight = ImVec2(X + W, Y + H);

		// Draw the lines to create the box
		p->AddLine(topLeft, topRight, ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);
		p->AddLine(topRight, bottomRight, ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);
		p->AddLine(bottomRight, bottomLeft, ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);
		p->AddLine(bottomLeft, topLeft, ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);
	}
}


inline int MenuTab = 1;

ImFont* VantaFont;
//
inline void DrawMenu() {

	if (GetAsyncKeyState_Spoofed(VK_INSERT))
	{
		menu_open = !menu_open;
		Sleep(300);
	}


	RECT screen_rect;
	::GetWindowRect(::GetDesktopWindow(), &screen_rect);

	ImGui::PushFont(NoramlFOnt);



	ImGuiStyle* Style = &ImGui::GetStyle();
	Style->WindowMinSize = ImVec2(600, 300);
	Style->WindowBorderSize = 0;

	Style->ChildRounding = 4.0f;
	Style->FrameRounding = 0.0f;
	Style->ScrollbarRounding = 4.0f;
	Style->GrabRounding = 4.0f;
	Style->PopupRounding = 0.0f;
	Style->WindowRounding = 4.0f;

	Style->WindowBorderSize = 3.0f;
	Style->WindowRounding = 4.0f;
	Style->ItemSpacing = ImVec2(4, 4);
	Style->ItemInnerSpacing = ImVec2(4, 4);
	Style->IndentSpacing = 25.0f;
	Style->ScrollbarSize = 15.0f;
	Style->ScrollbarRounding = 4.0f;
	Style->GrabMinSize = 4.0f;
	Style->GrabRounding = 4.0f;
	Style->ChildRounding = 4.0f;
	Style->FrameRounding = 0.0f;

	Style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);                // Text color (white)
	Style->Colors[ImGuiCol_TextDisabled] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);        // Disabled text color (white)
	Style->Colors[ImGuiCol_WindowBg] = ImColor(19, 19, 19);      // Window background color (lighter dark grey)
	Style->Colors[ImGuiCol_ChildBg] = ImVec4(0.141f, 0.141f, 0.141f, 1.0f); // Child window background color (lighter dark grey)
	Style->Colors[ImGuiCol_TitleBg] = ImVec4(0.396f, 0.396f, 0.545f, 0.6f);       // Title bar background color (light purple)
	Style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.396f, 0.396f, 0.545f, 0.6f); // Active title bar background color (light purple)
	Style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.396f, 0.396f, 0.545f, 0.6f);
	if (customcolor)
	{
		ImColor Bprercp;;
		Bprercp = ImVec4(accentcolor[0], accentcolor[1], accentcolor[2], 1.0f);
		Style->Colors[ImGuiCol_Border] = ImColor(Bprercp);
	}
	else
	{
		Style->Colors[ImGuiCol_Border] = ImColor(163, 58, 189);
	}// Border color (purple)
	Style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);        // Border shadow color (black)
	Style->Colors[ImGuiCol_Button] = ImVec4(0.176f, 0.176f, 0.176f, 0.7f);        // Button color (lighter dark grey)
	Style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);        // Button hover color (darker grey)
	Style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);         // Button active color (darker grey)
	Style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);           // Slider grab color (dark blue)
	Style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);     // Slider grab active color (dark blue)
	Style->Colors[ImGuiCol_FrameBg] = ImVec4(0.102f, 0.102f, 0.102f, 0.54f);      // Frame background color (semi-transparent dark grey)
	Style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);       // Frame background hover color (darker grey)
	Style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);        // Active title bar background color (darker grey)
	static ImVec4 inactive = ImColor(0, 136, 255, 255);
	static ImVec4 symbol = ImColor(0.150f, 0.361f, 0.696f, 1.000f);

	static int tabs = 0;

	static int selected = 0;
	static int sub_selected = 0;


	if (menu_open)
	{
		ImColor Buttomshi;
		Buttomshi = ImVec4(accentcolor[0], accentcolor[1], accentcolor[2], 1.0f);
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 originalMinSize = style.WindowMinSize;

		style.WindowMinSize = ImVec2(0, 0);

		ImVec2 windowSize(679, 389);

		ImVec2 mainWindowPos((ImGui::GetIO().DisplaySize.x - windowSize.x) * 0.5f, (ImGui::GetIO().DisplaySize.y - windowSize.y) * 0.5f);

		ImGui::SetNextWindowPos(mainWindowPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(windowSize);
		ImGui::Begin("Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetIO().MousePos, ImVec2(ImGui::GetIO().MousePos.x + 5.f, ImGui::GetIO().MousePos.y + 5.f), ImColor(255, 255, 255));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);





		if (MenuTab == 1)
		{
			if (customcolor)
			{
				if (ImGui::CoolButton(ImColor(Buttomshi), TEXT("Aim Assitance"), ImVec2(120, 20)))
				{
					MenuTab = 1;
				}

				ImGui::SameLine();
				if (weaponconfigs)
				{
					if (ImGui::CoolButton(ImColor(Buttomshi), TEXT("Rifle"), ImVec2(120, 20)))
					{
						MenuTab = 8;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Smg"), ImVec2(120, 20)))
					{
						MenuTab = 9;
					}
					ImGui::SameLine();
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Shotgun"), ImVec2(120, 20)))
					{
						MenuTab = 10;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Snipers"), ImVec2(120, 20)))
					{
						MenuTab = 11;
					}
				}
			}
			else
			{
				if (ImGui::CoolButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Aim Assitance"), ImVec2(120, 20)))
				{
					MenuTab = 1;
				}

				ImGui::SameLine();
				if (weaponconfigs)
				{
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Rifle"), ImVec2(120, 20)))
					{
						MenuTab = 8;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Smg"), ImVec2(120, 20)))
					{
						MenuTab = 9;
					}
					ImGui::SameLine();
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Shotgun"), ImVec2(120, 20)))
					{
						MenuTab = 10;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Snipers"), ImVec2(120, 20)))
					{
						MenuTab = 11;
					}
				}

			}
			ImGui::Spacing();
			ImGui::Text("Assistance");


			ImGui::CoolCheckbox("  Enable Aimbot  (Memory Aim off)", &aimbot);
			if (aimbot)
			{
				memory = false;
			}
			ImGui::CoolCheckbox("  Enable Memory Aim (aimbot off) (VERY RISKY)", &memory);
			if (memory)
			{
				aimbot = false;
			}
			ImGui::CoolCheckbox("  Circle FOV", &draw_fov);

			ImGui::CoolCheckbox("  Triggerbot", &triggerbot);
			ImGui::CoolCheckbox("  Target Line", &targetline);
			ImGui::CoolCheckbox("  Target Name", &targetcirlce);
			ImGui::CoolCheckbox("  Target Text", &targettex);






			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Customization");

			ImGui::SliderFloat(smoothing, TEXT("Assist Smooth"), &smoothing, 1.0f, 100);
			ImGui::SliderFloat(fov, TEXT("Field Of View"), &fov, 1.0f, 400);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::SetCursorPos({ 424, 39 });
			ImGui::Text("Target Bone");
			ImGui::SetCursorPos({ 425, 60 });
			ImGui::CoolCheckbox("Head Bone", &HeadBone);
			ImGui::SameLine();
			ImGui::CoolCheckbox("Body Bone", &ChestBone);
			ImGui::SetCursorPos({ 425, 90 });
			ImGui::CoolCheckbox("Neck Bone", &NeckBbone);
			ImGui::SameLine();
			ImGui::CoolCheckbox("Penis Bone", &PelvisBone);
			ImGui::SetCursorPos({ 424, 126 });
			ImGui::Text("Keybind");
			ImGui::SetCursorPos({ 424, 145 });
			ImGui::CoolCheckbox("Right mouse", &RBUTTOM);
			ImGui::SameLine();
			ImGui::CoolCheckbox("Caps", &CAPS);
			ImGui::SetCursorPos({ 424, 172 });
			ImGui::CoolCheckbox("Left mouse", &LBUTTOM);
			ImGui::SameLine();
			ImGui::CoolCheckbox("Shift Key", &SHIFT);

		}
		if (MenuTab == 8)
		{
			if (customcolor)
			{
				if (ImGui::CoolButton(ImColor(Buttomshi), TEXT("Aim Assitance"), ImVec2(120, 20)))
				{
					MenuTab = 1;
				}

				ImGui::SameLine();
				if (weaponconfigs)
				{
					if (ImGui::CoolButton(ImColor(Buttomshi), TEXT("Rifle"), ImVec2(120, 20)))
					{
						MenuTab = 8;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Smg"), ImVec2(120, 20)))
					{
						MenuTab = 9;
					}
					ImGui::SameLine();
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Shotgun"), ImVec2(120, 20)))
					{
						MenuTab = 10;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Snipers"), ImVec2(120, 20)))
					{
						MenuTab = 11;
					}
				}
			}
			else
			{
				if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Aim Assitance"), ImVec2(120, 20)))
				{
					MenuTab = 1;
				}

				ImGui::SameLine();
				if (weaponconfigs)
				{
					if (ImGui::CoolButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Rifle"), ImVec2(120, 20)))
					{
						MenuTab = 8;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Smg"), ImVec2(120, 20)))
					{
						MenuTab = 9;
					}
					ImGui::SameLine();
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Shotgun"), ImVec2(120, 20)))
					{
						MenuTab = 10;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Snipers"), ImVec2(120, 20)))
					{
						MenuTab = 11;
					}
				}

			}
			ImGui::Spacing();
			if (WeaponCfgs)
			{
				ImGui::Text("Rifle Settings");
				ImGui::CoolCheckbox("  Enable Aimbot ", &rifle_aimbot);
				if (rifle_aimbot)
				{
					rifle_memory = false;
				}
				ImGui::CoolCheckbox("  Enable Memory Aim (VERY RISKY)", &rifle_memory);
				if (rifle_memory)
				{
					rifle_aimbot = false;
				}
				ImGui::CoolCheckbox("  Circle FOV", &rifle_draw_fov);

				ImGui::CoolCheckbox("  Triggerbot", &rifle_triggerbot);
				ImGui::CoolCheckbox("  Target Line", &rifle_targetline);
				ImGui::CoolCheckbox("  Target Name", &rifle_targetcirlce);
				ImGui::CoolCheckbox("  Target Text", &rifle_targettex);






				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Text("Customization");

				ImGui::SliderFloat(rifle_smoothing, TEXT("Assist Smooth"), &rifle_smoothing, 1.0f, 100);
				ImGui::SliderFloat(rifle_fov, TEXT("Field Of View"), &rifle_fov, 1.0f, 400);

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::SetCursorPos({ 424, 39 });
				ImGui::Text("Target Bone");
				ImGui::SetCursorPos({ 425, 60 });
				ImGui::CoolCheckbox("Head Bone", &rifle_HeadBone);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Body Bone", &rifle_ChestBone);
				ImGui::SetCursorPos({ 425, 90 });
				ImGui::CoolCheckbox("Neck Bone", &rifle_NeckBbone);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Penis Bone", &rifle_Pelvisbone);
				ImGui::SetCursorPos({ 424, 126 });
				ImGui::Text("Keybind");
				ImGui::SetCursorPos({ 424, 145 });
				ImGui::CoolCheckbox("Right mouse", &rifle_RBUTTOM);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Caps", &rifle_CAPS);
				ImGui::SetCursorPos({ 424, 172 });
				ImGui::CoolCheckbox("Left mouse", &rifle_LBUTTOM);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Shift Key", &rifle_SHIFT);

			}
		}
		if (MenuTab == 9)
		{
			if (customcolor)
			{
				if (ImGui::CoolButton(ImColor(Buttomshi), TEXT("Aim Assitance"), ImVec2(120, 20)))
				{
					MenuTab = 1;
				}

				ImGui::SameLine();
				if (weaponconfigs)
				{
					if (ImGui::CoolButton(ImColor(Buttomshi), TEXT("Rifle"), ImVec2(120, 20)))
					{
						MenuTab = 8;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Smg"), ImVec2(120, 20)))
					{
						MenuTab = 9;
					}
					ImGui::SameLine();
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Shotgun"), ImVec2(120, 20)))
					{
						MenuTab = 10;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Snipers"), ImVec2(120, 20)))
					{
						MenuTab = 11;
					}
				}
			}
			else
			{
				if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Aim Assitance"), ImVec2(120, 20)))
				{
					MenuTab = 1;
				}

				ImGui::SameLine();
				if (weaponconfigs)
				{
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Rifle"), ImVec2(120, 20)))
					{
						MenuTab = 8;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Smg"), ImVec2(120, 20)))
					{
						MenuTab = 9;
					}
					ImGui::SameLine();
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Shotgun"), ImVec2(120, 20)))
					{
						MenuTab = 10;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Snipers"), ImVec2(120, 20)))
					{
						MenuTab = 11;
					}
				}

			}
			ImGui::Spacing();
			if (WeaponCfgs)
			{
				ImGui::Text("Smg Settings");
				ImGui::CoolCheckbox("  Enable Aimbot ", &smg_aimbot);
				if (smg_aimbot)
				{
					smg_memory = false;
				}
				ImGui::CoolCheckbox("  Enable Memory Aim (VERY RISKY)", &smg_memory);
				if (smg_memory)
				{
					smg_aimbot = false;
				}
				ImGui::CoolCheckbox("  Circle FOV", &smg_draw_fov);

				ImGui::CoolCheckbox("  Triggerbot", &smg_triggerbot);
				ImGui::CoolCheckbox("  Target Line", &smg_targetline);
				ImGui::CoolCheckbox("  Target Name", &smg_targetcirlce);
				ImGui::CoolCheckbox("  Target Text", &smg_targettex);






				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Text("Customization");

				ImGui::SliderFloat(smg_smoothing, TEXT("Assist Smooth"), &smg_smoothing, 1.0f, 100);
				ImGui::SliderFloat(smg_fov, TEXT("Field Of View"), &smg_fov, 1.0f, 400);

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::SetCursorPos({ 424, 39 });
				ImGui::Text("Target Bone");
				ImGui::SetCursorPos({ 425, 60 });
				ImGui::CoolCheckbox("Head Bone", &smg_HeadBone);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Body Bone", &smg_ChestBone);
				ImGui::SetCursorPos({ 425, 90 });
				ImGui::CoolCheckbox("Neck Bone", &smg_NeckBbone);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Penis Bone", &smg_Pelvisbone);
				ImGui::SetCursorPos({ 424, 126 });
				ImGui::Text("Keybind");
				ImGui::SetCursorPos({ 424, 145 });
				ImGui::CoolCheckbox("Right mouse", &smg_RBUTTOM);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Caps", &smg_CAPS);
				ImGui::SetCursorPos({ 424, 172 });
				ImGui::CoolCheckbox("Left mouse", &smg_LBUTTOM);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Shift Key", &smg_SHIFT);

			}
		}
		if (MenuTab == 10)
		{
			if (customcolor)
			{
				if (ImGui::CoolButton(ImColor(Buttomshi), TEXT("Aim Assitance"), ImVec2(120, 20)))
				{
					MenuTab = 1;
				}

				ImGui::SameLine();
				if (weaponconfigs)
				{
					if (ImGui::CoolButton(ImColor(Buttomshi), TEXT("Rifle"), ImVec2(120, 20)))
					{
						MenuTab = 8;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Smg"), ImVec2(120, 20)))
					{
						MenuTab = 9;
					}
					ImGui::SameLine();
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Shotgun"), ImVec2(120, 20)))
					{
						MenuTab = 10;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Snipers"), ImVec2(120, 20)))
					{
						MenuTab = 11;
					}
				}
			}
			else
			{
				if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Aim Assitance"), ImVec2(120, 20)))
				{
					MenuTab = 1;
				}

				ImGui::SameLine();
				if (weaponconfigs)
				{
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Rifle"), ImVec2(120, 20)))
					{
						MenuTab = 8;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Smg"), ImVec2(120, 20)))
					{
						MenuTab = 9;
					}
					ImGui::SameLine();
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Shotgun"), ImVec2(120, 20)))
					{
						MenuTab = 10;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Snipers"), ImVec2(120, 20)))
					{
						MenuTab = 11;
					}
				}

			}
			ImGui::Spacing();
			if (WeaponCfgs)
			{
				ImGui::Text("Shotgun Settings");
				ImGui::CoolCheckbox("  Enable Aimbot ", &shotgun_aimbot);
				if (shotgun_aimbot)
				{
					shotgun_memory = false;
				}
				ImGui::CoolCheckbox("  Enable Memory Aim (VERY RISKY)", &shotgun_memory);
				if (shotgun_memory)
				{
					shotgun_aimbot = false;
				}
				ImGui::CoolCheckbox("  Circle FOV", &shotgun_draw_fov);

				ImGui::CoolCheckbox("  Triggerbot", &shotgun_triggerbot);
				ImGui::CoolCheckbox("  Target Line", &shotgun_targetline);
				ImGui::CoolCheckbox("  Target Name", &shotgun_targetcirlce);
				ImGui::CoolCheckbox("  Target Text", &shotgun_targettex);






				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Text("Customization");

				ImGui::SliderFloat(shotgun_smoothing, TEXT("Assist Smooth"), &shotgun_smoothing, 1.0f, 100);
				ImGui::SliderFloat(shotgun_fov, TEXT("Field Of View"), &shotgun_fov, 1.0f, 400);

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::SetCursorPos({ 424, 39 });
				ImGui::Text("Target Bone");
				ImGui::SetCursorPos({ 425, 60 });
				ImGui::CoolCheckbox("Head Bone", &shotgun_HeadBone);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Body Bone", &shotgun_ChestBone);
				ImGui::SetCursorPos({ 425, 90 });
				ImGui::CoolCheckbox("Neck Bone", &shotgun_NeckBbone);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Penis Bone", &shotgun_Pelvisbone);
				ImGui::SetCursorPos({ 424, 126 });
				ImGui::Text("Keybind");
				ImGui::SetCursorPos({ 424, 145 });
				ImGui::CoolCheckbox("Right mouse", &shotgun_RBUTTOM);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Caps", &shotgun_CAPS);
				ImGui::SetCursorPos({ 424, 172 });
				ImGui::CoolCheckbox("Left mouse", &shotgun_LBUTTOM);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Shift Key", &shotgun_SHIFT);

			}
		}

		if (MenuTab == 11)
		{
			if (customcolor)
			{
				if (ImGui::CoolButton(ImColor(Buttomshi), TEXT("Aim Assitance"), ImVec2(120, 20)))
				{
					MenuTab = 1;
				}

				ImGui::SameLine();
				if (weaponconfigs)
				{
					if (ImGui::CoolButton(ImColor(Buttomshi), TEXT("Rifle"), ImVec2(120, 20)))
					{
						MenuTab = 8;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Smg"), ImVec2(120, 20)))
					{
						MenuTab = 9;
					}
					ImGui::SameLine();
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Shotgun"), ImVec2(120, 20)))
					{
						MenuTab = 10;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Snipers"), ImVec2(120, 20)))
					{
						MenuTab = 11;
					}
				}
			}
			else
			{
				if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Aim Assitance"), ImVec2(120, 20)))
				{
					MenuTab = 1;
				}

				ImGui::SameLine();
				if (weaponconfigs)
				{
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Rifle"), ImVec2(120, 20)))
					{
						MenuTab = 8;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Smg"), ImVec2(120, 20)))
					{
						MenuTab = 9;
					}
					ImGui::SameLine();
					ImGui::SameLine();
					if (ImGui::CoolButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Shotgun"), ImVec2(120, 20)))
					{
						MenuTab = 10;
					}
					ImGui::SameLine();
					if (ImGui::CoolButton(ImColor(19, 19, 19), TEXT("Snipers"), ImVec2(120, 20)))
					{
						MenuTab = 11;
					}
				}

			}
			ImGui::Spacing();
			if (WeaponCfgs)
			{
				ImGui::Text("Sniper Settings");
				ImGui::CoolCheckbox("  Enable Aimbot ", &sniper_aimbot);
				if (sniper_aimbot)
				{
					sniper_memory = false;
				}
				ImGui::CoolCheckbox("  Enable Memory Aim (VERY RISKY)", &sniper_memory);
				if (sniper_memory)
				{
					sniper_aimbot = false;
				}
				ImGui::CoolCheckbox("  Circle FOV", &sniper_draw_fov);

				ImGui::CoolCheckbox("  Triggerbot", &sniper_triggerbot);
				ImGui::CoolCheckbox("  Target Line", &sniper_targetline);
				ImGui::CoolCheckbox("  Target Name", &sniper_targetcirlce);
				ImGui::CoolCheckbox("  Target Text", &sniper_targettex);






				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Text("Customization");

				ImGui::SliderFloat(sniper_smoothing, TEXT("Assist Smooth"), &sniper_smoothing, 1.0f, 100);
				ImGui::SliderFloat(sniper_fov, TEXT("Field Of View"), &sniper_fov, 1.0f, 400);

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::SetCursorPos({ 424, 39 });
				ImGui::Text("Target Bone");
				ImGui::SetCursorPos({ 425, 60 });
				ImGui::CoolCheckbox("Head Bone", &sniper_HeadBone);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Body Bone", &sniper_ChestBone);
				ImGui::SetCursorPos({ 425, 90 });
				ImGui::CoolCheckbox("Neck Bone", &sniper_NeckBbone);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Penis Bone", &sniper_Pelvisbone);
				ImGui::SetCursorPos({ 424, 126 });
				ImGui::Text("Keybind");
				ImGui::SetCursorPos({ 424, 145 });
				ImGui::CoolCheckbox("Right mouse", &sniper_RBUTTOM);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Caps", &sniper_CAPS);
				ImGui::SetCursorPos({ 424, 172 });
				ImGui::CoolCheckbox("Left mouse", &sniper_LBUTTOM);
				ImGui::SameLine();
				ImGui::CoolCheckbox("Shift Key", &sniper_SHIFT);

			}
		}


		if (MenuTab == 7)
		{
			if (customcolor)
			{
				ImGui::SetCursorPos({ 30, 12 });
				if (ImGui::EpicButton(ImVec4(Buttomshi), TEXT("Player ESP"), ImVec2(120, 20)))
				{
					MenuTab = 2;
				}
				ImGui::SetCursorPos({ 118, 8 });
				if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Extra Settings"), ImVec2(120, 20)))
				{
					MenuTab = 7;
				}
				ImGui::SameLine();
				ImGui::SetCursorPos({ 259, 12 });
				if (ImGui::EpicButton(ImVec4(Buttomshi), TEXT("Colors"), ImVec2(120, 20)))
				{
					MenuTab = 6;
				}
			}
			else
			{
				ImGui::SetCursorPos({ 30, 12 });
				if (ImGui::EpicButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Player ESP"), ImVec2(120, 20)))
				{
					MenuTab = 2;
				}

				ImGui::SetCursorPos({ 118, 8 });
				if (ImGui::CoolButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Extra Settings"), ImVec2(120, 20)))
				{
					MenuTab = 7;
				}
				ImGui::SetCursorPos({ 259, 12 });
				if (ImGui::EpicButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Colors"), ImVec2(120, 20)))
				{
					MenuTab = 6;
				}
			}


			ImGui::Spacing();
			ImGui::Text("  Extra Settings");
			ImGui::CoolCheckbox("  Player Debug", &testdbg);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Head Esp", &headesp);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Plataform Esp", &draw_platform);
			ImGui::Spacing();

			ImGui::CoolCheckbox("  Player ViewAngle", &viewangle);



			ImGui::Spacing();
			ImGui::CoolCheckbox("  Gay Mode", &rainbow);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Outlined Esp", &Outline);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Custom Accent Color", &customcolor);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Weapon Rarity (for Weapon esp color)", &weapon_rarity);


			ImGui::Spacing();
			ImGui::SliderFloat(box_thickness, TEXT("Box Thickness"), &box_thickness, 0.f, 5);
			ImGui::SliderFloat(skelthick, TEXT("Skeleton Thickness"), &skelthick, 0.f, 5);
			ImGui::SliderFloat(headcirclesize, TEXT("Head Circle size"), &headcirclesize, 0.f, 50);

		}
		if (MenuTab == 2)
		{
			if (customcolor)
			{
				ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Player ESP"), ImVec2(120, 20));
				ImGui::SetCursorPos({ 148, 12 });
				if (ImGui::EpicButton(ImVec4(Buttomshi), TEXT("Extra Settings"), ImVec2(120, 20)))
				{
					MenuTab = 7;
				}
				ImGui::SameLine();
				if (ImGui::EpicButton(ImVec4(Buttomshi), TEXT("Colors"), ImVec2(120, 20)))
				{
					MenuTab = 6;
				}
			}
			else
			{
				ImGui::CoolButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Player ESP"), ImVec2(120, 20));
				ImGui::SetCursorPos({ 148, 12 });
				if (ImGui::EpicButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Extra Settings"), ImVec2(120, 20)))
				{
					MenuTab = 7;
				}
				ImGui::SameLine();
				if (ImGui::EpicButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Colors"), ImVec2(120, 20)))
				{
					MenuTab = 6;
				}
			}
			ImGui::Spacing();
			ImGui::Text("  Visuals");
			ImGui::CoolCheckbox("  Enable ESP", &esp);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Box Esp", &Corner);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Skeleton", &skeleton);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Rounded Box", &box_round);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Non-Rounded Box", &box_esp);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  3D Box", &threed);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Snaplines", &line);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Show Distance", &draw_distance);
			ImGui::Spacing();
			ImGui::CoolCheckbox("  Show Username", &draw_username);



			ImGui::SliderFloat(max_esp_distance, TEXT("Render Distance"), &max_esp_distance, 1.0f, 500);
		}

		if (MenuTab == 5)
		{
			ImGui::Spacing();
			ImGui::Text("  Customization");
			ImGui::Text("");
			ImGui::Text("  UI");
			ImGui::Spacing();
			ImGui::Text("");
			ImGui::Text("  Options");
			ImGui::Spacing();
			if (ImGui::Button("Unload"))
				exit(0);
		}

		if (MenuTab == 6)
		{
			if (customcolor)
			{
				ImGui::SetCursorPos({ 30, 12 });
				if (ImGui::EpicButton(ImVec4(Buttomshi), TEXT("Player ESP"), ImVec2(120, 20)))
				{
					MenuTab = 2;
				}
				ImGui::SetCursorPos({ 148, 12 });
				if (ImGui::EpicButton(ImVec4(Buttomshi), TEXT("Extra Settings"), ImVec2(120, 20)))
				{
					MenuTab = 7;
				}
				ImGui::SameLine();
				ImGui::SetCursorPos({ 280, 8 });
				if (ImGui::CoolButton(ImVec4(Buttomshi), TEXT("Colors"), ImVec2(120, 20)))
				{
					MenuTab = 6;
				}
			}
			else
			{
				ImGui::SetCursorPos({ 30, 12 });
				if (ImGui::EpicButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Player ESP"), ImVec2(120, 20)))
				{
					MenuTab = 2;
				}
				ImGui::SetCursorPos({ 148, 12 });
				if (ImGui::EpicButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Extra Settings"), ImVec2(120, 20)))
				{
					MenuTab = 7;
				}
				ImGui::SameLine();
				ImGui::SetCursorPos({ 280, 8 });
				if (ImGui::CoolButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("Colors"), ImVec2(120, 20)))
				{
					MenuTab = 6;
				}
			}
			auto flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_NoTooltip |
				ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions;
			ImGui::Spacing();
			ImGui::Text("Menu Color or Extra (enable Custom Color in Extra Settings");
			ImGui::ColorEdit3(TEXT("Menu Color "), accentcolor);

			ImGui::Spacing();
			ImGui::Text(TEXT("Visible player"));
			ImGui::ColorEdit3(TEXT("Box"), PlayerColor::BoxVisible);
			ImGui::ColorEdit3(TEXT("Skeleton"), PlayerColor::SkeletonVisible);
			ImGui::ColorEdit3(TEXT("Snapline"), PlayerColor::SnaplineVisible);
			ImGui::ColorEdit3(TEXT("Top text"), PlayerColor::TopTextVisible);
			ImGui::ColorEdit3(TEXT("Bottom text"), PlayerColor::BottomTextVisible);


			ImGui::Text(TEXT("Not visible player"));
			ImGui::ColorEdit3(TEXT("Box "), PlayerColor::BoxNotVisible);
			ImGui::ColorEdit3(TEXT("Skeleton "), PlayerColor::SkeletonNotVisible);
			ImGui::ColorEdit3(TEXT("Snapline "), PlayerColor::SnaplineNotVisible);
			ImGui::ColorEdit3(TEXT("Top text "), PlayerColor::TopTextNotVisible);
			ImGui::ColorEdit3(TEXT("Bottom text "), PlayerColor::BottomTextNotVisible);




		}

		if (MenuTab == 3)
		{
			ImGui::Text(TEXT(""));
			ImGui::Separator();
			ImGui::Text(TEXT(""));

			ImGui::TextColored(ImVec4(0.92f, 0.92f, 0.3f, 1.0f), TEXT("Version"));
			ImGui::SameLine();
			ImGui::Text(TEXT("->")); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.05f, 1.0f), TEXT("1.0"));





			ImGui::Text(TEXT(""));
			ImGui::Text(TEXT("Cheat Status"));

			ImGui::TextColored(ImVec4(0.05f, 0.92f, 0.65f, 1.0f), TEXT("EAC"));
			ImGui::SameLine();
			ImGui::Text(TEXT("->")); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.0f, 0.92f, 0.f, 0.9f), TEXT("DTC"));
			ImGui::TextColored(ImVec4(0.05f, 0.92f, 0.65f, 1.0f), TEXT("BE"));

			ImGui::SameLine();


			ImGui::Text(TEXT("->")); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.0f, 0.92f, 0.0f, 0.9f), TEXT("DTC"));







		}






		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar();

		ImGui::End();

		ImVec2 mainWindowSize = ImGui::GetWindowSize();

		float buttonWidth = 40.0f;
		float buttonHeight = 40.0f;

		ImVec2 detachedWindowSize(71, 191);



		ImVec2 detachedWindowPos(mainWindowPos.x - detachedWindowSize.x - style.WindowBorderSize - buttonWidth * 0.5f + 2, mainWindowPos.y + (mainWindowSize.y - detachedWindowSize.y) * 0.5f - 45.0f);

		detachedWindowPos.x -= 1.5f;

		ImGui::SetNextWindowPos(detachedWindowPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(detachedWindowSize);
		ImGui::Begin("Detached Area", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetIO().MousePos, ImVec2(ImGui::GetIO().MousePos.x + 5.f, ImGui::GetIO().MousePos.y + 5.f), ImColor(255, 255, 255));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Button color
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::GetColorU32({ 0.1f, 0.1f, 0.1f, 1.f })); // Button color
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.0f)); // Button hover color
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Button active color
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f)); // Center-align button text
		ImGui::PushFont(VantaFont);




		if (customcolor)
		{

			ImColor Buttomshi1;
			Buttomshi1 = ImVec4(accentcolor[0], accentcolor[1], accentcolor[2], 1.0f);
			ImGui::SetCursorPos({ 6, 9 });
			ImGui::EpicButton(ImVec4(Buttomshi1), TEXT("Aimbot"));
			if (ImGui::IsItemClicked())
				MenuTab = 1;

			ImGui::EpicButton(ImVec4(Buttomshi1), TEXT("Visuals"));
			if (ImGui::IsItemClicked())
				MenuTab = 2;

			ImGui::EpicButton(ImVec4(Buttomshi1), TEXT("Extras"));
			if (ImGui::IsItemClicked())
				MenuTab = 3;
		}
		else
		{

			ImGui::SetCursorPos({ 6, 9 });
			ImGui::EpicButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("a"));
			if (ImGui::IsItemClicked())
				MenuTab = 1;

			ImGui::EpicButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("c"));
			if (ImGui::IsItemClicked())
				MenuTab = 2;

			ImGui::EpicButton(ImVec4(0.65f, 0.05f, 0.9f, 1.0f), TEXT("b"));
			if (ImGui::IsItemClicked())
				MenuTab = 3;
		}

		ImGui::PopID();

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
		ImGui::End();


		ImVec2 mainWindowSiz1e = ImGui::GetWindowSize();
		ImVec2 detachedWindowSize1(250, 389);

		ImVec2 detachedWindowPos1(mainWindowPos.x + mainWindowSize.x + style.WindowBorderSize + buttonWidth * 0.5f - 2, mainWindowPos.y + (mainWindowSize.y - detachedWindowSize.y) * 0.5f - 45.0f);
		detachedWindowPos1.y -= 59.0f;
		detachedWindowPos1.x += 75.5f;
		//detachedWindowPos1.x += 1.5f;
		ImGui::SetNextWindowPos(detachedWindowPos1, ImGuiCond_Always);
		ImGui::SetNextWindowSize(detachedWindowSize1);
		ImGui::Begin("Detached Area1", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetIO().MousePos, ImVec2(ImGui::GetIO().MousePos.x + 5.f, ImGui::GetIO().MousePos.y + 5.f), ImColor(255, 255, 255));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Button color
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.0f)); // Button hover color
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Button active color
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f)); // Center-align button text
		ImGui::PushFont(VantaFont);
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();



		static int m_size_x = 679;
		static int m_size_y = 389;
		static int m_pos_y = center_y - (m_size_y / 2);
		static int m_pos_x = center_x - (m_size_x / 2);
		int x1 = m_pos_x + m_size_x + 12;
		int y1 = m_pos_y;
		int lenght = 250;
		int x2 = m_pos_x + m_size_x + 15 + lenght;
		int y2 = m_pos_y + m_size_y;
		int x3 = (x1 + lenght) - (lenght / 2);

		y1 = y1 - 20;
		auto __top_x = x3 + Preview::head_bone_x;
		auto __top_y = (y1 + Preview::head_bone_y - 20);
		auto __bottom_x = x3 + Preview::head_bone_x;
		auto __bottom_y = (y1 + Preview::right_footfinger_bone_y) + 20;

		auto ColorBox = ImVec4(PlayerColor::BoxVisible[0], PlayerColor::BoxVisible[1], PlayerColor::BoxVisible[2], 1.0f);
		auto ColorSkeleton = ImVec4(PlayerColor::SkeletonVisible[0], PlayerColor::SkeletonVisible[1], PlayerColor::SkeletonVisible[2], 1.0f);
		auto ColorSnapline = ImVec4(PlayerColor::SnaplineVisible[0], PlayerColor::SnaplineVisible[1], PlayerColor::SnaplineVisible[2], 1.0f);
		auto ColorTopText = ImVec4(PlayerColor::TopTextVisible[0], PlayerColor::TopTextVisible[1], PlayerColor::TopTextVisible[2], 1.0f);
		auto ColorBottomText = ImVec4(PlayerColor::BottomTextVisible[0], PlayerColor::BottomTextVisible[1], PlayerColor::BottomTextVisible[2], 1.0f);
		ImGui::PushFont(NoramlFOnt);

		Style->Colors[ImGuiCol_WindowBg] = ImColor(29, 29, 29);
		//draw_list->AddRectFilled(ImVec2(detachedWindowPos1.x, detachedWindowPos1.y), ImVec2(detachedWindowPos1.x - 1, detachedWindowPos1.y - 1), ImGui::GetColorU32({ 0.1f, 0.1f, 0.1f, 1.f }), 6.5f, 0);
		if (esp)
		{
			if (threed)
			{
				using namespace Preview::box3d;
				//top square
				draw_list->AddLine(ImVec2(x3 + topleftfront_x, y1 + topleftfront_y), ImVec2(x3 + topleftback_x, y1 + topleftback_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));
				draw_list->AddLine(ImVec2(x3 + topleftfront_x, y1 + topleftfront_y), ImVec2(x3 + toprightfront_x, y1 + toprightfront_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));
				draw_list->AddLine(ImVec2(x3 + topleftback_x, y1 + topleftback_y), ImVec2(x3 + toprightback_x, y1 + toprightback_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));
				draw_list->AddLine(ImVec2(x3 + toprightback_x, y1 + toprightback_y), ImVec2(x3 + toprightfront_x, y1 + toprightfront_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));

				//bottom square
				draw_list->AddLine(ImVec2(x3 + bottomleftfront_x, y1 + bottomleftfront_y), ImVec2(x3 + bottomleftback_x, y1 + bottomleftback_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));
				draw_list->AddLine(ImVec2(x3 + bottomleftfront_x, y1 + bottomleftfront_y), ImVec2(x3 + bottomrightfront_x, y1 + bottomrightfront_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));
				draw_list->AddLine(ImVec2(x3 + bottomleftback_x, y1 + bottomleftback_y), ImVec2(x3 + bottomrightback_x, y1 + bottomrightback_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));
				draw_list->AddLine(ImVec2(x3 + bottomrightback_x, y1 + bottomrightback_y), ImVec2(x3 + bottomrightfront_x, y1 + bottomrightfront_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));

				//top-bottom connection
				draw_list->AddLine(ImVec2(x3 + topleftfront_x, y1 + topleftfront_y), ImVec2(x3 + bottomleftfront_x, y1 + bottomleftfront_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));
				draw_list->AddLine(ImVec2(x3 + topleftback_x, y1 + topleftback_y), ImVec2(x3 + bottomleftback_x, y1 + bottomleftback_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));
				draw_list->AddLine(ImVec2(x3 + toprightfront_x, y1 + toprightfront_y), ImVec2(x3 + bottomrightfront_x, y1 + bottomrightfront_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));
				draw_list->AddLine(ImVec2(x3 + toprightback_x, y1 + toprightback_y), ImVec2(x3 + bottomrightback_x, y1 + bottomrightback_y), ImGui::GetColorU32({ ColorBox.x, ColorBox.y, ColorBox.z, 2.0f }));

			}
			if (Corner) {
				float BoxHeight = __bottom_y - __top_y;
				float BoxWidth = BoxHeight / 1.6f;
				RenderBox(true, 4, 4, __bottom_x - (BoxWidth / 2), __top_y,
					BoxWidth, BoxHeight, ColorBox, 2.0f, false, false);
			}
			if (box_esp)
			{

				float BoxHeight = __bottom_y - __top_y;
				float BoxWidth = BoxHeight / 1.6f;
				RenderBox1(true, 4, 4, __bottom_x - (BoxWidth / 2), __top_y,
					BoxWidth, BoxHeight + 3, ColorBox, 2.0f, false, false);
			}
			if (box_round)
			{

				float BoxHeight = __bottom_y - __top_y;
				float BoxWidth = BoxHeight / 1.6f;
				RenderBox2(true, 4, 4, __bottom_x - (BoxWidth / 2), __top_y,
					BoxWidth, BoxHeight + 3, ColorBox, 2.0f, false, false);
			}
		}

		//top text
		{
			int ThisOffset = -3;
			int ThisOffset2 = -19;

			int ThisOffset1 = 23;






			if (draw_distance) {
				auto Size = ImGui::CalcTextSize(TEXT("[4.0]"));
				draw_list->AddText(ImVec2(__bottom_x - (Size.x / 2), __bottom_y - ThisOffset), ImGui::GetColorU32({ ColorBottomText.x, ColorBottomText.y, ColorBottomText.z, 1.0f }), TEXT("[13m]"));
			}



			if (draw_username) {
				auto Size = ImGui::CalcTextSize(TEXT("Mrkz"));
				draw_list->AddText(ImVec2(__top_x - (Size.x / 2), __top_y - ThisOffset1), ImGui::GetColorU32({ ColorTopText.x, ColorTopText.y, ColorTopText.z, 1.0f }), TEXT("Mrkz"));
			}

			if (line) {
				if (line)
					draw_list->AddLine(ImVec2(x3 - 7, m_pos_y), ImVec2(__bottom_x, __top_y - ThisOffset), ImGui::GetColorU32({ ColorSnapline.x, ColorSnapline.y, ColorSnapline.z, 1.0f }), 1.0f);
			}
		}

		{
			int ThisOffset = 3;

			if (line) {
				if (line2)
					draw_list->AddLine(ImVec2(x3 - 7, m_pos_y + m_size_y), ImVec2(__bottom_x, __bottom_y + ThisOffset), ImGui::GetColorU32({ ColorSnapline.x, ColorSnapline.y, ColorSnapline.z, 1.0f }), 1.0f);
			}
		}
		if (headesp)
		{
			draw_list->AddCircle(ImVec2(x3 + Preview::head_bone_x, y1 + Preview::head_bone_y), 20, ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 50, 2.0f);
		}
		if (skeleton)
		{


			//top
			draw_list->AddLine(ImVec2(x3 + Preview::head_bone_x, y1 + Preview::head_bone_y), ImVec2(x3 + Preview::neck_bone_x, y1 + Preview::neck_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::neck_bone_x, y1 + Preview::neck_bone_y), ImVec2(x3 + Preview::chest_bone_x, y1 + Preview::chest_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);

			//left arm
			draw_list->AddLine(ImVec2(x3 + Preview::chest_bone_x, y1 + Preview::chest_bone_y), ImVec2(x3 + Preview::left_shoulder_bone_x, y1 + Preview::left_shoulder_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::left_shoulder_bone_x, y1 + Preview::left_shoulder_bone_y), ImVec2(x3 + Preview::left_elbow_bone_x, y1 + Preview::left_elbow_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::left_elbow_bone_x, y1 + Preview::left_elbow_bone_y), ImVec2(x3 + Preview::left_hand_bone_x, y1 + Preview::left_hand_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);

			//right arm
			draw_list->AddLine(ImVec2(x3 + Preview::chest_bone_x, y1 + Preview::chest_bone_y), ImVec2(x3 + Preview::right_shoulder_bone_x, y1 + Preview::right_shoulder_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::right_shoulder_bone_x, y1 + Preview::right_shoulder_bone_y), ImVec2(x3 + Preview::right_elbow_bone_x, y1 + Preview::right_elbow_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::right_elbow_bone_x, y1 + Preview::right_elbow_bone_y), ImVec2(x3 + Preview::right_hand_bone_x, y1 + Preview::right_hand_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);

			//wtf!
			draw_list->AddLine(ImVec2(x3 + Preview::chest_bone_x, y1 + Preview::chest_bone_y), ImVec2(x3 + Preview::pelvis_bone_x, y1 + Preview::pelvis_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);

			//left foot
			draw_list->AddLine(ImVec2(x3 + Preview::pelvis_bone_x, y1 + Preview::pelvis_bone_y), ImVec2(x3 + Preview::left_thigh_bone_x, y1 + Preview::left_thigh_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::left_thigh_bone_x, y1 + Preview::left_thigh_bone_y), ImVec2(x3 + Preview::left_knee_bone_x, y1 + Preview::left_knee_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::left_knee_bone_x, y1 + Preview::left_knee_bone_y), ImVec2(x3 + Preview::left_foot_bone_x, y1 + Preview::left_foot_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::left_foot_bone_x, y1 + Preview::left_foot_bone_y), ImVec2(x3 + Preview::left_footfinger_bone_x, y1 + Preview::left_footfinger_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);

			//right foot
			draw_list->AddLine(ImVec2(x3 + Preview::pelvis_bone_x, y1 + Preview::pelvis_bone_y), ImVec2(x3 + Preview::right_thigh_bone_x, y1 + Preview::right_thigh_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::right_thigh_bone_x, y1 + Preview::right_thigh_bone_y), ImVec2(x3 + Preview::right_knee_bone_x, y1 + Preview::right_knee_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::right_knee_bone_x, y1 + Preview::right_knee_bone_y), ImVec2(x3 + Preview::right_foot_bone_x, y1 + Preview::right_foot_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
			draw_list->AddLine(ImVec2(x3 + Preview::right_foot_bone_x, y1 + Preview::right_foot_bone_y), ImVec2(x3 + Preview::right_footfinger_bone_x, y1 + Preview::right_footfinger_bone_y), ImGui::GetColorU32({ ColorSkeleton.x, ColorSkeleton.y, ColorSkeleton.z, 1.0f }), 2.0f);
		}
		ImGui::PopID();

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
		ImGui::End();


		// Restore the original WindowMinSize
		style.WindowMinSize = originalMinSize;
	}
	else
	{
	}
}



bool aim;




MSG Message = { NULL };

__forceinline VOID directx_init()
{
	SPOOF_FUNC
		if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::p_object)))
			exit(3);

	ZeroMemory(&DirectX9Interface::p_params, sizeof(DirectX9Interface::p_params));
	DirectX9Interface::p_params.Windowed = TRUE;
	DirectX9Interface::p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	DirectX9Interface::p_params.hDeviceWindow = DirectX9Interface::hijacked_hwnd;
	DirectX9Interface::p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	DirectX9Interface::p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	DirectX9Interface::p_params.BackBufferWidth = screen_width;
	DirectX9Interface::p_params.BackBufferHeight = screen_height;
	DirectX9Interface::p_params.EnableAutoDepthStencil = TRUE;
	DirectX9Interface::p_params.AutoDepthStencilFormat = D3DFMT_D16;
	DirectX9Interface::p_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	if (FAILED(DirectX9Interface::p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, DirectX9Interface::hijacked_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &DirectX9Interface::p_params, 0, &DirectX9Interface::p_device)))
	{
		DirectX9Interface::p_object->Release();
		Sleep(1000);
		exit(1);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(DirectX9Interface::hijacked_hwnd);
	ImGui_ImplDX9_Init(DirectX9Interface::p_device);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	static const ImWchar icons_ranges[] = { 0xf000, 0xf5ff, 0 };
	ImFontConfig icons_config;

	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	ImFontConfig config;

	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 1;
	icons_config.OversampleV = 1;
	ImFontConfig CustomFont;
	CustomFont.FontDataOwnedByAtlas = false;

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	ESPFont = io.Fonts->AddFontFromFileTTF("C:\\fortnite.otf", 14.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	VantaFont = io.Fonts->AddFontFromFileTTF("C:\\vanta.ttf", 45.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	NoramlFOnt = io.Fonts->AddFontFromFileTTF("C:\\verdana.ttf", 15.0f, nullptr, io.Fonts->GetGlyphRangesDefault());


	DirectX9Interface::p_object->Release();
}





VOID render_loop()
{
	SPOOF_FUNC;
	static RECT old_rc;
	RtlSecureZeroMemory(&DirectX9Interface::message, sizeof(MSG));
	while (DirectX9Interface::message.message != WM_QUIT)
	{
		if (PeekMessageA(&DirectX9Interface::message, DirectX9Interface::hijacked_hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&DirectX9Interface::message);
			DispatchMessageA(&DirectX9Interface::message);
		}

		RECT rc;
		POINT xy;
		RtlSecureZeroMemory(&rc, sizeof(RECT));
		RtlSecureZeroMemory(&xy, sizeof(POINT));
		GetClientRect(DirectX9Interface::game_wnd, &rc);
		ClientToScreen(DirectX9Interface::game_wnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = DirectX9Interface::game_wnd;
		screen_width = io.DisplaySize.x;
		screen_height = io.DisplaySize.y;
		center_x = io.DisplaySize.x / 2;
		center_y = io.DisplaySize.y / 2;

		POINT p;
		GetCursorPosA_Spoofed(&p);
		long styleProc = GetWindowLongA_Spoofed(DirectX9Interface::hijacked_hwnd, GWL_EXSTYLE);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;
		if (menu_open)
		{
			styleProc &= ~WS_EX_LAYERED;
			SetWindowLongA_Spoofed(DirectX9Interface::hijacked_hwnd,
				GWL_EXSTYLE, styleProc);
			SetForegroundWindow(DirectX9Interface::hijacked_hwnd);
		}
		else
		{
			styleProc |= WS_EX_LAYERED;
			SetWindowLong(DirectX9Interface::hijacked_hwnd,
				GWL_EXSTYLE, styleProc);
		}
		if (GetAsyncKeyState_Spoofed(0x1))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
		{
			io.MouseDown[0] = false;
		}
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;
			DirectX9Interface::p_params.BackBufferWidth = screen_width;
			DirectX9Interface::p_params.BackBufferHeight = screen_height;
			SetWindowPos_Spoofed(DirectX9Interface::hijacked_hwnd, (HWND)0, xy.x, xy.y, screen_width, screen_height, SWP_NOREDRAW);
			DirectX9Interface::p_device->Reset(&DirectX9Interface::p_params);
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ColorChange();
		gaybow;

		if (GetAsyncKeyState_Spoofed(VK_DELETE))
		{
			exit(0);
		}
		actor_loop();
		DrawMenu();
		ImGui::EndFrame();

		DirectX9Interface::p_device->SetRenderState(D3DRS_ZENABLE, false);
		DirectX9Interface::p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		DirectX9Interface::p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		DirectX9Interface::p_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (DirectX9Interface::p_device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			DirectX9Interface::p_device->EndScene();
		}
		HRESULT result = DirectX9Interface::p_device->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && DirectX9Interface::p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9Interface::p_device->Reset(&DirectX9Interface::p_params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}

	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9Interface::p_device != NULL)
	{
		DirectX9Interface::p_device->EndScene();
		DirectX9Interface::p_device->Release();
	}
	if (DirectX9Interface::p_object != NULL)
	{
		DirectX9Interface::p_object->Release();
	}
	DestroyWindow(DirectX9Interface::hijacked_hwnd);
}
HWND windowid = NULL;
int choice;
const char* folderPath = "C:\\pic";
#include <codecvt>
#include "shit.h"


BOOL anti_debugger()
{
	SPOOF_FUNC
		BOOL is_debugged = FALSE;
	HANDLE h_process = GetCurrentProcess();
	HANDLE h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	PROCESSENTRY32 process_entry = { 0 };
	process_entry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(h_snapshot, &process_entry);

	do
	{
		if (_stricmp(process_entry.szExeFile, "notepad.exe") == 0
			|| _stricmp(process_entry.szExeFile, "procexp.exe") == 0
			|| _stricmp(process_entry.szExeFile, "ollydbg.exe") == 0
			|| _stricmp(process_entry.szExeFile, "idaq.exe") == 0
			|| _stricmp(process_entry.szExeFile, "idaq64.exe") == 0
			|| _stricmp(process_entry.szExeFile, "ImmunityDebugger.exe") == 0
			|| _stricmp(process_entry.szExeFile, "x64dbg.exe") == 0
			|| _stricmp(process_entry.szExeFile, "windbg.exe") == 0
			|| _stricmp(process_entry.szExeFile, "joeboxcontrol.exe") == 0)
		{
			is_debugged = TRUE;
			break;
		}
	} while (Process32Next(h_snapshot, &process_entry));

	CloseHandle(h_snapshot);
	if (is_debugged)
	{
		std::cout << "try me nigga.\n\n";
		Sleep(1000);
		TerminateProcess(h_process, 0);
		return TRUE;
	}

	return FALSE;
}




std::string data_dir = ("");
std::string temp_dir = ("");

namespace StringConverter {
	std::wstring ToUnicode(const std::string& str) {
		SPOOF_FUNC
			using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
	}

	std::string ToASCII(const std::wstring& wstr) {
		SPOOF_FUNC
			using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}
}


bool deleteFile(const std::string& filename) {
	if (std::remove(filename.c_str()) != 0) {
		SPOOF_FUNC
			// Error occurred while deleting the file
			return false;
	}
	return true;
}
std::string mappaPath;



using namespace KeyAuth;

auto name = skCrypt(""); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
auto ownerid = skCrypt(""); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
auto secret = skCrypt(""); // app secret, the blurred text on licenses tab and other tabs
auto version = skCrypt("1.0"); // leave alone unless you've changed version on website
auto url = skCrypt("https://keyauth.win/api/1.2/"); // change if you're self-hosting

api KeyAuthApp(name.decrypt(), ownerid.decrypt(), secret.decrypt(), version.decrypt(), url.decrypt());


int main()
{
	SPOOF_FUNC;
	mouse_interface();
	//rndmTitle(); // Put this back (remove the first //) if u want to add a randomized title name
	SetConsoleSize();

	name.clear(); ownerid.clear(); secret.clear(); version.clear(); url.clear();

	std::string consoleTitle = skCrypt("Loader - Built at:  ").decrypt() + compilation_date + " " + compilation_time;
	SetConsoleTitleA(consoleTitle.c_str());
	std::cout << skCrypt("\n\n Connecting..");
	KeyAuthApp.init();
	if (!KeyAuthApp.data.success)
	{
		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
		Sleep(1500);
		exit(1);
	}

	std::string key;
	std::cout << "\n Key -> ";
	std::cin >> key;

	KeyAuthApp.license(key);

	if (!KeyAuthApp.data.success)
	{
		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
		Sleep(1500);
		exit(1);
	}

	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo);

	COORD new_screen_buffer_size;

	new_screen_buffer_size.X = screenBufferInfo.srWindow.Right -
		screenBufferInfo.srWindow.Left + 1;
	new_screen_buffer_size.Y = screenBufferInfo.srWindow.Bottom -
		screenBufferInfo.srWindow.Top + 1;



	SetConsoleScreenBufferSize(hConsole, new_screen_buffer_size);
TOP:
	rndmTitle();
	std::cout << ("\nConnecting");
	for (int i = 0; i < 3; i++) {
		cout << (".") << flush;
		Sleep(500);
	}

	rndmTitle();
	Sleep(1000);
	system("cls");

	std::wcout << R"(                             


      
        Fn
                                            

                                               
                                               
                                                  
            )" << '\n';

	//int option;
	rndmTitle();



	system("cls");
	printf("\n  \033[33m(>)\033[0m");
	std::cout << ("Press WIN+H ingame");

	mem::find_driver();



	mem::process_id = mem::find_process("FortniteClient-Win64-Shipping.exe");
	while (windowid == NULL)

	{
		XorS(wind, "Fortnite  ");
		windowid = FindWindowA(0, wind.decrypt());

	}


	LI_FN(system)(E("curl https://cdn.discordapp.com/attachments/1096267297552879696/1188474943298617344/font.otf --silent -o C:\\font.otf"));



	while (!(GetAsyncKeyState_Spoofed(VK_LWIN) & 0x8000) || !(GetAsyncKeyState_Spoofed('H') & 0x8000)) {

		Sleep(100);
	}

	mem::BaseAddy = mem::find_image();

	if (!mem::BaseAddy)
	{
		std::cout << _xor_(" Load the driver again!");
	}

	std::thread(cache_players).detach();






	ShowWindow(::GetConsoleWindow(), SW_SHOW);
	Beep(300, 1000);


	hijack::hijack_init();
	MouseController::Init();
	directx_init();
	render_loop();

	ErasePEHeaderFromMemory();

}
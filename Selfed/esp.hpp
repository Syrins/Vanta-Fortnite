#pragma once
#include <memory>
#include "cache.h"

#include "draw.h"
#include "mouse.hpp"
#include "aimkey.h"
#include "driver.hpp"
#include "protect/skCrypt.h"
#include "imgui/imgui_internal.h"
#include "test menu.h"
#include "controlelr.h"
#pragma warning(disable : 4996)

bool has_clicked;
std::chrono::steady_clock::time_point tb_begin;
std::chrono::steady_clock::time_point tb_end;
int tb_time_since;
__int64 TargetedBuild;
bool bTargetedBuild;

void preform_aimbot(double closest_distance, std::shared_ptr<player_data> closest_player) {



	if (RBUTTOM)
	{
		aimkey = VK_RBUTTON;
	}

	if (LBUTTOM)
	{
		aimkey = VK_LBUTTON;
	}

	if (CAPS)
	{
		aimkey = VK_CAPITAL;
	}

	if (SHIFT)
	{
		aimkey = VK_LSHIFT;
	}

	vector3 headbone;
	if (closest_distance != DBL_MAX && aimbot && (u)global->local_pawn) {
		
		if (Key.IsKeyPushing(aimkey) or Controller::IsPressingLeftTrigger()) {
			vector2 target_loc{};
			if (HeadBone) {
				headbone = closest_player->mesh->get_head();
				target_loc = w2s(closest_player->mesh->get_head());
			}
			else if (NeckBbone) {
				headbone = closest_player->mesh->get_bone_3d(66);
				target_loc = w2s(closest_player->mesh->get_bone_3d(66));
			}
			else if (ChestBone) {
				vector2 chest_left = w2s(closest_player->mesh->get_bone_3d(37));
				vector2 chest_right = w2s(closest_player->mesh->get_bone_3d(8));
				headbone = closest_player->mesh->get_bone_3d(8);
				target_loc = { chest_left.x + (chest_right.x - chest_left.x) / 2, chest_left.y };
			}
			else if (PelvisBone) {
				headbone = closest_player->mesh->get_bone_3d(2);
				target_loc = w2s(closest_player->mesh->get_bone_3d(2));
			}
			
			move_mouse(target_loc);
		}
	}
	if (triggerbot)
	{
		__int64 temp_targted = read<__int64>(read<std::uintptr_t>((u)global->player_controller + 0x1840));
		bool b = Key.IsKeyPushing(aimkey);
		if (b && bTargetedBuild) {
			TargetedBuild = temp_targted;
			bTargetedBuild = false;
		}
		if (!b && !bTargetedBuild) {
			TargetedBuild = 0;
			bTargetedBuild = true;
		}
		if (TargetedBuild != 0 && TargetedBuild != temp_targted)
		{
			INPUT    Input = { 0 };
			// left down 
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			::SendInput(1, &Input, sizeof(INPUT));

			// left up
			::ZeroMemory(&Input, sizeof(INPUT));
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			::SendInput(1, &Input, sizeof(INPUT));
			TargetedBuild = 0;
		}


		if (TargetedBuild != temp_targted && TargetedBuild != 0)
			Beep(500, 500);
	}
}



void preform_aimbot1(double closest_distance, std::shared_ptr<player_data> closest_player) {
	if (RBUTTOM)
	{
		aimkey = VK_RBUTTON;
	}

	if (LBUTTOM)
	{
		aimkey = VK_LBUTTON;
	}

	if (CAPS)
	{
		aimkey = VK_CAPITAL;
	}

	if (SHIFT)
	{
		aimkey = VK_LSHIFT;
	}
	if (closest_distance != DBL_MAX &&  memory && (u)global->local_pawn) {
		
		if (Key.IsKeyPushing(aimkey)) {
			vector2 target_loc{};
			if (HeadBone) {
				target_loc = w2s(closest_player->mesh->get_head());
			}
			else if (NeckBbone) {
				target_loc = w2s(closest_player->mesh->get_bone_3d(66));
			}
			else if (ChestBone) {
				vector2 chest_left = w2s(closest_player->mesh->get_bone_3d(37));
				vector2 chest_right = w2s(closest_player->mesh->get_bone_3d(8));
				target_loc = { chest_left.x + (chest_right.x - chest_left.x) / 2, chest_left.y };
			}
			else if (PelvisBone) {
				target_loc = w2s(closest_player->mesh->get_bone_3d(2));
			}
			if (targetline) {
				ImGui::GetForegroundDrawList()->AddCircle(ImVec2(target_loc.x, target_loc.y), 5, ImColor(255, 0, 0, 255), 50);
			}

			vector2 screen_center = { static_cast<double>(screen_width) / 2, static_cast<double>(screen_height) / 2 };
			vector2 target{};


			target.x = (target_loc.x > screen_center.x) ? -(screen_center.x - target_loc.x) : target_loc.x - screen_center.x;
			target.x /= smoothing;
			target.x = (target.x + screen_center.x > screen_center.x * 2 || target.x + screen_center.x < 0) ? 0 : target.x;

			target.y = (target_loc.y > screen_center.y) ? -(screen_center.y - target_loc.y) : target_loc.y - screen_center.y;
			target.y /= smoothing;
			target.y = (target.y + screen_center.y > screen_center.y * 2 || target.y + screen_center.y < 0) ? 0 : target.y;

			vector3 Angles(-target.y / smoothing, target.x / smoothing, 0);

			vector3 original1(0, 0, 0);

			write<double>((u)global->player_controller + 0x520, Angles.x);
			write<double>((u)global->player_controller + 0x520 + 0x8, Angles.y);


			if (triggerbot)
			{
				__int64 temp_targted = read<__int64>(read<std::uintptr_t>((u)global->player_controller + 0x1840));
				bool b = Key.IsKeyPushing(aimkey);
				if (b && bTargetedBuild) {
					TargetedBuild = temp_targted;
					bTargetedBuild = false;
				}
				if (!b && !bTargetedBuild) {
					TargetedBuild = 0;
					bTargetedBuild = true;
				}
				if (TargetedBuild != 0 && TargetedBuild != temp_targted)
				{
					INPUT    Input = { 0 };
					// left down 
					Input.type = INPUT_MOUSE;
					Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					::SendInput(1, &Input, sizeof(INPUT));

					// left up
					::ZeroMemory(&Input, sizeof(INPUT));
					Input.type = INPUT_MOUSE;
					Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					::SendInput(1, &Input, sizeof(INPUT));
					TargetedBuild = 0;
				}


				if (TargetedBuild != temp_targted && TargetedBuild != 0)
					Beep(500, 500);
			}
		}
	}
}


std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

void DrawNewTextCustomWithOutline(int x, int y, int R, int G, int B, int A, const char* str)
{
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	float outlineThickness = 1.5f; // Adjust the outline thickness as needed

	// Draw the text with black outline
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(x - outlineThickness, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, A / 255.0f)), utf_8_2.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(x + outlineThickness, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, A / 255.0f)), utf_8_2.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(x, y - outlineThickness), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, A / 255.0f)), utf_8_2.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(x, y + outlineThickness), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, A / 255.0f)), utf_8_2.c_str());

	// Draw the main text with the specified color
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(R / 255.0f, G / 255.0f, B / 255.0f, A / 255.0f)), utf_8_2.c_str());
}



//
//
//inline camera_position_s get_camera_postion() {
//
//
//	char v1; // r8
//	camera_position_s local_camera = read<camera_position_s>(mem::BaseAddy + 0xE2CED18);
//	BYTE* v2 = (BYTE*)&local_camera;
//	int i; // edx
//	__int64 result; // rax
//
//	v1 = 0x40;
//	for (i = 0; i < 0x40; ++i)
//	{
//		*v2 ^= v1;
//		result = (unsigned int)(i + 0x17);
//		v1 += i + 0x17;
//		v2++;
//	}
//
//	return local_camera;
//
//}

int HeldWeaponType;

enum EFortWeaponType : int
{
	Rifle = 0,
	Shotgun = 1,
	Smg = 2,
	Sniper = 3
};
camera_position_s get_camera()
{
	camera_position_s camera;

	auto location_pointer = read<uintptr_t>((u)global->world + 0x110);
	auto rotation_pointer = read<uintptr_t>((u)global->world + 0x120);

	struct FNRot
	{
		double a; //0x0000
		char pad_0008[24]; //0x0008
		double b; //0x0020
		char pad_0028[424]; //0x0028
		double c; //0x01D0
	}fnRot;

	fnRot.a = read<double>(rotation_pointer);
	fnRot.b = read<double>(rotation_pointer + 0x20);
	fnRot.c = read<double>(rotation_pointer + 0x1d0);

	camera.location = read<vector3>(location_pointer);
	camera.rotation.x = asin(fnRot.c) * (180.0 / M_PI);
	camera.rotation.y = ((atan2(fnRot.a * -1, fnRot.b) * (180.0 / M_PI)) * -1) * -1;
	camera.fov = read<float>((uintptr_t)(u)global->player_controller + 0x394) * 90.f;

	return camera;
}

struct FRotator
{
public:
	double Pitch;
	double Yaw;
	double Roll;
};



float radar_position_x{ 10.0f };
float radar_position_y{ 10.0f };
float radar_size{ 250.0f };


namespace radar_style {
	bool circle = true;
	bool rectangle = false;
}
namespace radar_text_mode {
	bool outlined = true;
	bool background = false;
}


std::string username(std::uintptr_t playerstate)
{
	int pNameLength; // rsi
	_WORD* pNameBufferPointer;
	int i;      // ecx
	char v25; // al
	int v26;  // er8
	int v29;  // eax

	char16_t* pNameBuffer;

	
	uintptr_t pNameStructure =read<uintptr_t>(playerstate + 0xAE0);
		pNameLength =read<int>(pNameStructure + 0x10);
		if (pNameLength <= 0)
			return "";

		pNameBuffer = new char16_t[pNameLength];
		uintptr_t pNameEncryptedBuffer =read<uintptr_t>(pNameStructure + 0x8);
		read_array(pNameEncryptedBuffer, pNameBuffer, pNameLength);
	

	v25 = pNameLength - 1;
	v26 = 0;
	pNameBufferPointer = (_WORD*)pNameBuffer;

	for (i = (v25) & 3;; *pNameBufferPointer++ += i & 7) {
		v29 = pNameLength - 1;
		if (!(_DWORD)pNameLength)
			v29 = 0;

		if (v26 >= v29)
			break;

		i += 3;
		++v26;
	}

	std::u16string temp_wstring(pNameBuffer);
	delete[] pNameBuffer;
	return std::string(temp_wstring.begin(), temp_wstring.end());
}




float color_red = 1.;
float color_green = 0;
float color_blue = 0;
float color_random = 0.0;
float color_speed = 2.0;

void ColorChange()
{
	static float Color[3];
	static DWORD Tickcount = 0;
	static DWORD Tickcheck = 0;
	ImGui::ColorConvertRGBtoHSV(color_red, color_green, color_blue, Color[0], Color[1], Color[2]);
	if (GetTickCount() - Tickcount >= 1)
	{
		if (Tickcheck != Tickcount)
		{
			Color[0] += 0.001f * color_speed;
			Tickcheck = Tickcount;
		}
		Tickcount = GetTickCount();
	}
	if (Color[0] < 0.0f) Color[0] += 1.0f;
	ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], color_red, color_green, color_blue);
}

float BOG_TO_GRD(float BOG) {
	return (180 / M_PI) * BOG;
}

float GRD_TO_BOG(float GRD) {
	return (M_PI / 180) * GRD;
}

void gaybow(ImGuiWindow& window) {
	auto RGB = ImGui::GetColorU32({ color_red, color_green, color_blue, 255 });


}





ImFont* NoramlFOnt;
ImFont* ESPFont;




bool rect_radar = false;
namespace camera
{
	vector3 rotation;
	vector3 nigga;
	vector3 nigga2;
	vector3 location;
	float fov;
	float zoom;
}


float RadarDistance = { 20.f };
struct Camera
{
	vector3 Location;
	vector3 Rotation;
	float FOV;
};

void RadarRange(float* x, float* y, float range)
{
	if (fabs((*x)) > range || fabs((*y)) > range)
	{
		if ((*y) > (*x))
		{
			if ((*y) > -(*x))
			{
				(*x) = range * (*x) / (*y);
				(*y) = range;
			}
			else
			{
				(*y) = -range * (*y) / (*x);
				(*x) = -range;
			}
		}
		else
		{
			if ((*y) > -(*x))
			{
				(*y) = range * (*y) / (*x);
				(*x) = range;
			}
			else
			{
				(*x) = -range * (*x) / (*y);
				(*y) = -range;
			}
		}
	}
}


Camera GetCamera()
{
	Camera FGC_Camera;
	FGC_Camera.Location = camera::location;
	FGC_Camera.Rotation.x = camera::rotation.x;
	FGC_Camera.Rotation.y = camera::rotation.y;
	FGC_Camera.Rotation.z = 0;
	return FGC_Camera;
}
void CalcRadarPoint(vector3 vOrigin, int& screenx, int& screeny) {
		Camera vCamera = GetCamera();
		vector3 vAngle = vCamera.Rotation;
		auto fYaw = (-vAngle.y + 180.0f) * M_PI / 180.0f;

		float dx = vOrigin.x - vCamera.Location.x;
		float dy = vOrigin.y - vCamera.Location.y;


		float fsin_yaw = sinf(fYaw);
		float fcos_yaw = cosf(fYaw);
		float x = dx * fcos_yaw - dy * fsin_yaw;  
		float y = dx * fsin_yaw + dy * fcos_yaw; 



	float range = (float)RadarDistance * 1000.f;
	RadarRange(&x, &y, range);

	ImVec2 DrawPos = ImVec2(radar_position_x, radar_position_y);
	ImVec2 DrawSize = ImVec2(radar_size, radar_size);

	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}

void fortnite_radar(float x, float y, float size, bool rect = false)
{
	if (radar)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Once);
		static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
		ImGui::Begin(("##radar"), nullptr, flags);

		float render_size = 5;
		ImDrawList* drawList = ImGui::GetWindowDrawList();




		drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + size, y + size), ImColor(71, 71, 71, 255), 0.0f, 0);
		drawList->AddRect(ImVec2(x, y), ImVec2(x + size, y + size), ImColor(0, 0, 0, 255), 0.0f, 0);

		drawList->AddLine(ImVec2(radar_position_x + (radar_size / 2), radar_position_y + (radar_size / 2)), ImVec2(radar_position_x, radar_position_y), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), 1.f);
		drawList->AddLine(ImVec2(radar_position_x + (radar_size / 2), radar_position_y + (radar_size / 2)), ImVec2(radar_position_x + radar_size, radar_position_y), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), 1.f);
		drawList->AddLine(ImVec2(radar_position_x, radar_position_y + (radar_size / 2)), ImVec2(radar_position_x + radar_size, radar_position_y + (radar_size / 2)), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), 1.f);
		drawList->AddLine(ImVec2(radar_position_x + (radar_size / 2), radar_position_y + (radar_size / 2)), ImVec2(radar_position_x + (radar_size / 2), radar_position_y), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), 1.f);

		drawList->AddLine(ImVec2(radar_position_x + (radar_size / 2), radar_position_y + (radar_size / 2)), ImVec2(radar_position_x + (radar_size / 2), radar_position_y + radar_size), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), 1.f);
		drawList->AddCircleFilled(ImVec2(x + size / 2, y + size / 2), render_size / 2 * sqrt(2), ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 1.0f }), 1000);
		  drawList->AddCircle(ImVec2(x + size / 2, y + size / 2), size, ImGui::GetColorU32({ 0.1f, 0.1f, 0.1f, 1.0f }), 1000, 5);




		ImGui::End();
	}
}
void add_players_radar(vector3 WorldLocation)
{
	if (radar)
	{
		static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
		ImGui::Begin(("##radar"), nullptr, flags);

		int ScreenX, ScreenY = 0;
		const auto& GetWindowDrawList = ImGui::GetWindowDrawList();
		ImDrawList* drawList = ImGui::GetWindowDrawList();




		CalcRadarPoint(WorldLocation, ScreenX, ScreenY);

		ImColor RadarCol;
		RadarCol = ImVec4(accentcolor[0], accentcolor[1], accentcolor[2], 1.0f);
		if (customcolor)
		{
			drawList->AddCircleFilled(ImVec2(ScreenX, ScreenY), 5, ImColor(RadarCol), 7);
		}
		else
		{
			drawList->AddCircleFilled(ImVec2(ScreenX, ScreenY), 5, ImColor(255, 115, 20), 7);
		}
			
			
		


		ImGui::End();
	}
}




bool visibble = true;


enum EMovementMode : uint8
{
	MOVE_None = 0,
	MOVE_Walking = 1,
	MOVE_NavWalking = 2,
	MOVE_Falling = 3,
	MOVE_Swimming = 4,
	MOVE_Flying = 5,
	MOVE_Custom = 6,
	MOVE_MAX = 7,
};


vector3 Off_relativelocation;

char* drawBuff = (char*)malloc(4096);
static float ItemDistance = 50.f;


float degree_to_radian(float degree) {
	
	return degree * (M_PI / 180);
}
void angle_rotation(const vector3& angles, vector3* forward)
{
	float	sp, sy, cp, cy;

	sy = sin(degree_to_radian(angles.y));
	cy = cos(degree_to_radian(angles.y));

	sp = sin(degree_to_radian(angles.x));
	cp = cos(degree_to_radian(angles.x));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}




void actor_loop() {

	ImGui::PushFont(ESPFont);
	auto RGB = ImGui::GetColorU32({ color_red, color_green, color_blue, 255 });
	if (debug) {
		printf(("\n [+] ActorLoop called"));
	}

	std::unique_lock<std::mutex> lock(global->data_mutex);

	camera_postion = get_camera();

	ImDrawList* draw_list = ImGui::GetForegroundDrawList();


	int dynamicfov = (dynamicfovval - 80) + fov;


	if (draw_fov) {

		if (rainbow)
		{
			draw_list->AddCircle(ImVec2(screen_width / 2, screen_height / 2), dynamicfov, ImColor(RGB), 33, 1);
			draw_list->AddCircle(ImVec2(screen_width / 2, screen_height / 2), dynamicfov + 1, ImColor(20, 0, 0, 255), 33, 1);
		}
		else
		{
			if (customcolor)
			{
				draw_list->AddCircle(ImVec2(screen_width / 2, screen_height / 2), dynamicfov, ImColor(AccentCol), 100, 1);
			}
			else
			{
				draw_list->AddCircle(ImVec2(screen_width / 2, screen_height / 2), dynamicfov, ImColor(255, 255, 255, 255), 100, 1);
			}

		}
	}
	if (draw_crosshair) {
		draw_list->AddLine(ImVec2(screen_width / 2 - 2, screen_height / 2), ImVec2(screen_width / 2 - 9, screen_height / 2), ImColor(0, 0, 0), 2);
		draw_list->AddLine(ImVec2(screen_width / 2 + 2, screen_height / 2), ImVec2(screen_width / 2 + 9, screen_height / 2), ImColor(0, 0, 0), 2);
		draw_list->AddLine(ImVec2(screen_width / 2, screen_height / 2 - 2), ImVec2(screen_width / 2, screen_height / 2 - 9), ImColor(0, 0, 0), 2);
		draw_list->AddLine(ImVec2(screen_width / 2, screen_height / 2 + 2), ImVec2(screen_width / 2, screen_height / 2 + 9), ImColor(0, 0, 0), 2);

		draw_list->AddLine(ImVec2(screen_width / 2 - 3, screen_height / 2), ImVec2(screen_width / 2 - 8, screen_height / 2), ImColor(255, 255, 255), 1);
		draw_list->AddLine(ImVec2(screen_width / 2 + 3, screen_height / 2), ImVec2(screen_width / 2 + 8, screen_height / 2), ImColor(255, 255, 255), 1);
		draw_list->AddLine(ImVec2(screen_width / 2, screen_height / 2 - 3), ImVec2(screen_width / 2, screen_height / 2 - 8), ImColor(255, 255, 255), 1);
		draw_list->AddLine(ImVec2(screen_width / 2, screen_height / 2 + 3), ImVec2(screen_width / 2, screen_height / 2 + 8), ImColor(255, 255, 255), 1);
	}
	if (draw_fps) 
	{

		char fpsinfo[256];
		sprintf_s(fpsinfo, (("Niggaware ")), ImGui::GetIO().Framerate);
		draw_list->AddText(ImVec2(2400, 15), ImColor(255, 255, 255, 255), fpsinfo);
	}
	else
	{
		char fpsinfo[256];
		sprintf_s(fpsinfo, (("Niggaware ")), ImGui::GetIO().Framerate);
		draw_list->AddText(ImVec2(15, 15), ImColor(255, 255, 255, 255), fpsinfo);
	}


	if (global->players.size() < 1) return;

	auto mesh = read<uintptr_t>((u)global->local_pawn + 0x318);


	vector3 base_3d;

	bool DoingAimbot{ false };
	bool ResetAngles{ false };

	double closest_distance = DBL_MAX;
	std::shared_ptr<player_data> closest_player = std::make_shared<player_data>();
	for (auto player : global->players) {
		vector3 head_3d = player->mesh->get_head();
		vector2 head_2d = w2s(vector3(head_3d.x, head_3d.y, head_3d.z + 20));
		auto CurrentActor = read<uintptr_t>((u)player->player + 0x308);//PawnPrivate

		base_3d = player->mesh->get_base();
		vector2 base_2d = w2s(base_3d);

		float distance1 = player->relative_location.distance(head_3d);



		




		float box_hight = abs(head_2d.y - base_2d.y);
		float box_width = box_hight * 0.40;
		ImColor visible_color_box;
		ImColor visible_color_skel;
		ImColor visible_color_Snapline;
		ImColor visible_color_Text;
		ImColor visible_color_Text1;

		auto isbot = (u(player->player + 0x29A));

		bool inlobby;


		if (global->local_pawn == NULL)
		{
			inlobby = true;
		}
		else
		{
			inlobby = false;
		}





		visible_color_Text = ImVec4(1.f, 1.f, 1.f, 1.0f);
		visible_color_Text1 = ImVec4(1.f, 1.f, 1.f, 1.0f);



		
		if (!head_2d.valid_location())
			continue;

		ImGui::PushFont(ESPFont);
		if (esp) {
			ImGui::PushFont(ESPFont);

		
		
			

			if (rainbow)
			{
				visible_color_box = ImColor(RGB);
				visible_color_skel = ImColor(RGB);
				visible_color_Snapline = ImColor(RGB);
				visible_color_Text = ImColor(RGB);
				visible_color_Text1 = ImColor(RGB);

			}

			if (player->mesh->visible()) {
				if (!rainbow)
				{
					visible_color_box = ImVec4(PlayerColor::BoxVisible[0], PlayerColor::BoxVisible[1], PlayerColor::BoxVisible[2], 1.0f);
					visible_color_skel = ImVec4(PlayerColor::SkeletonVisible[0], PlayerColor::SkeletonVisible[1], PlayerColor::SkeletonVisible[2], 1.0f);
					visible_color_Snapline = ImVec4(PlayerColor::SnaplineVisible[0], PlayerColor::SnaplineVisible[1], PlayerColor::SnaplineVisible[2], 1.0f);
					visible_color_Text = ImVec4(PlayerColor::TopTextVisible[0], PlayerColor::TopTextVisible[1], PlayerColor::TopTextVisible[2], 1.0f);
					visible_color_Text1 = ImVec4(PlayerColor::BottomTextVisible[0], PlayerColor::BottomTextVisible[1], PlayerColor::BottomTextVisible[2], 1.0f);


				}
				visibble = true;


			}
			else if (!player->mesh->visible()) {

				if (!rainbow)
				{

					visible_color_box = ImVec4(PlayerColor::BoxNotVisible[0], PlayerColor::BoxNotVisible[1], PlayerColor::BoxNotVisible[2], 1.0f);
					visible_color_skel = ImVec4(PlayerColor::SkeletonNotVisible[0], PlayerColor::SkeletonNotVisible[1], PlayerColor::SkeletonNotVisible[2], 1.0f);
					visible_color_Snapline = ImVec4(PlayerColor::SnaplineNotVisible[0], PlayerColor::SnaplineNotVisible[1], PlayerColor::SnaplineNotVisible[2], 1.0f);
					visible_color_Text = ImVec4(PlayerColor::TopTextNotVisible[0], PlayerColor::TopTextNotVisible[1], PlayerColor::TopTextNotVisible[2], 1.0f);
					visible_color_Text1 = ImVec4(PlayerColor::BottomTextNotVisible[0], PlayerColor::BottomTextNotVisible[1], PlayerColor::BottomTextNotVisible[2], 1.0f);

				}
				visibble = false;
			}




			if (box_esp)
			{


				if (Outline)
				{
					draw_list->AddRect(ImVec2(head_2d.x - box_width / 2, head_2d.y), ImVec2((head_2d.x - box_width / 2) + box_width, head_2d.y + box_hight), ImColor(0, 0, 0), 0, ImDrawCornerFlags_None, box_thickness + 1.5);
				}

				draw_list->AddRect(ImVec2(head_2d.x - box_width / 2, head_2d.y), ImVec2((head_2d.x - box_width / 2) + box_width, head_2d.y + box_hight), visible_color_box, 0, ImDrawCornerFlags_None, box_thickness);

			}
			if (box_round)
			{
				if (Outline)
				{
					draw_list->AddRect(ImVec2(head_2d.x - box_width / 2, head_2d.y), ImVec2((head_2d.x - box_width / 2) + box_width, head_2d.y + box_hight), ImColor(0, 0, 0), 10, ImDrawCornerFlags_All, box_thickness + 1.5);
				}

				draw_list->AddRect(ImVec2(head_2d.x - box_width / 2, head_2d.y), ImVec2((head_2d.x - box_width / 2) + box_width, head_2d.y + box_hight), visible_color_box, 10, ImDrawCornerFlags_All, box_thickness);
			}
			if (Corner)
			{
				if (Outline)
				{
					DrawCornerBox(head_2d.x - box_width / 2, head_2d.y, box_width, box_hight, box_thickness + 1.5, ImColor(0, 0, 0), box_round);
				}

				DrawCornerBox(head_2d.x - box_width / 2, head_2d.y, box_width, box_hight, box_thickness, visible_color_box, box_round);
			}

			if (threed)
			{

				if (Outline)
				{
					DrawThreeDBox(base_3d, head_3d, 100, 100, box_thickness + 1.5, ImColor(0, 0, 0));
				}
				DrawThreeDBox(base_3d, head_3d, 100, 100, box_thickness, visible_color_box);
			}



			if (line) {
				draw_list->AddLine(ImVec2(screen_width / 2, 0), ImVec2(head_2d.x, head_2d.y), visible_color_Snapline, Linethick);
			}

			if (line1) {
				draw_list->AddLine(ImVec2(center_x, center_y), ImVec2(head_2d.x, head_2d.y), visible_color_Snapline, Linethick);
			}

		

			

			if (!inlobby && testdbg) {


				uint64_t player_weapon = read<uint64_t>(reinterpret_cast<uint64_t>(player->player) + 0xA20);
				char const* string1;

				auto AimingWeap = read<bool>(player_weapon + 0xc65); // BoolProperty Class FortniteGame.FortWeapon.bIsTargeting -> 0xc65 (Mask: 1)

				auto CrouchingActoVar = read<bool>(reinterpret_cast<uint64_t>(player->player) + 0x45c); /// BoolProperty Class Engine.Character.bIsCrouched -> 0x45c (Mask: 1)
				auto bIsSliding = read<bool>(reinterpret_cast<uint64_t>(player->player) + 0x75b); //BoolProperty Class FortniteGame.FortPawn.bIsSliding -> 0x753 (Mask: 8)
				auto bIsJumping = read<bool>(reinterpret_cast<uint64_t>(player->player) + 0xa18); //BoolProperty Class FortniteGame.FortPawn.bIsJumping -> 0x940 (Mask: 1)
				auto kills1 = read<int>(reinterpret_cast<uint64_t>(player->player_state) + 0x10ec); //IntProperty Class FortniteGame.FortPlayerStateAthena.KillScore -> 0x10ec

				auto leveltest = read<int>(reinterpret_cast<uint64_t>(player->player_state) + 0x10f0); //IntProperty Class FortniteGame.FortPlayerStateAthena.SeasonLevelUIDisplay -> 0x10f0
				auto bThankedBusDriver = read<int>(reinterpret_cast<uint64_t>(player->player_state) + 0x1245); //BoolProperty Class FortniteGame.FortPlayerStateAthena.bThankedBusDriver


				
				std::string nametest;
				std::string nametest1;
				std::string nametest2;
				std::string nametest3;

				if (bIsSliding)
				{
					 nametest = "Sliding";
				}

				if (bIsJumping)
				{
					nametest1 = "Jumping";
				}
				if (CrouchingActoVar)
				{
					if (!bIsSliding)
					{
						nametest2 = "Chrouching";
					}
				}
				if (!bIsSliding || !bIsJumping || !CrouchingActoVar)
				{
					if (!bIsSliding && !bIsJumping && !CrouchingActoVar)
					{
						nametest3 = "Normal";
					}


				}
				std::string final = ("Player State: ") + bIsSliding + nametest1 + nametest2 + nametest3 + ("]");

			
			
			
				char text_ammo_count[256];
				char text_ammo_count1[256];
				char text_ammo_count2[256];
				char text_ammo_count3[256];

				char text_ammo_count4[256];
				char text_ammo_count5[256];


				sprintf_s(text_ammo_count, ("[Kills: %d]"), kills1);
				sprintf_s(text_ammo_count2, ("[Levels: %d]"), leveltest);
				sprintf_s(text_ammo_count3, ("[Ammo: %d]"), global->WeaponInfo.ammo_count);



				if (AimingWeap)
				{
					sprintf_s(text_ammo_count4, ("[Aiming State: Aiming]"));
				}
				else
				{
					sprintf_s(text_ammo_count4, ("[Aiming State: Not Aiming]"));

				}




				ImVec2 TextSize = ImGui::CalcTextSize(text_ammo_count);


				auto less = 5;
				if (player->player_distance > 30)
				{
					less;
				}
				if (player->player_distance > 60)
				{
					less = 10;
				}

			
				if (player->player_distance > 90)
				{
				less = 15;
				}
				

				if (player->player_distance > 120)
				{
				less = 20;
				}

				if (player->player_distance > 150)
				{
					less = 25;
				}

				if (player->player_distance > 180)
				{
					less = 30;
				}

				if (player->player_distance > 220)
				{
					less = 35;
				}

				if (player->player_distance > 250)
				{
					less = 40;
				}

				if (player->player_distance > 280)
				{
					less = 45;
				}
				if (player->player_distance > 310)
				{
					less = 50;
				}



						DrawString(14, (head_2d.x + 80) - (TextSize.x / 2), (head_2d.y + 35 - less), visible_color_Text, false, true, text_ammo_count);
					
			

						DrawString(14, (head_2d.x + 80) - (TextSize.x / 2), (head_2d.y + 55 - less), visible_color_Text, false, true, text_ammo_count2);

						DrawString(14, (head_2d.x + 80) - (TextSize.x / 2), (head_2d.y + 75 - less), visible_color_Text, false, true, final.c_str());
						DrawString(14, (head_2d.x + 80) - (TextSize.x / 2), (head_2d.y + 95 - less), visible_color_Text, false, true, text_ammo_count4);
						DrawString(14, (head_2d.x + 80) - (TextSize.x / 2), (head_2d.y + 115 - less), visible_color_Text, false, true, text_ammo_count3);






		


			}

		


			if (headesp) {

				if (global->local_pawn == NULL)
				{
					vector2 head_2d = w2s(vector3(head_3d.x, head_3d.y, head_3d.z + 20));


					vector3 delta = head_3d - camera_postion.location;
					float distance = delta.length();


					const float constant_circle_size = headcirclesize;


					float circle_radius = constant_circle_size * (screen_height / (2.0f * distance * tanf(camera_postion.fov * (float)M_PI / 360.f)));





					float y_offset = +70.0f;
					head_2d.y += y_offset;


					
					int segments = 50; 
					float thickness = 2.0f; 

					if (Outline)
					{
						ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, ImColor(0, 0, 0, 255), segments, skelthick + 2);
					}
					ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, visible_color_skel, segments, skelthick);



				}
				else
				{

					vector2 head_2d = w2s(vector3(head_3d.x, head_3d.y, head_3d.z + 20));


					vector3 delta = head_3d - camera_postion.location;
					float distance = delta.length();


					const float constant_circle_size = headcirclesize;


					float circle_radius = constant_circle_size * (screen_height / (2.0f * distance * tanf(camera_postion.fov * (float)M_PI / 360.f)));


					float y_offset = +12.0f;

					if (player->player_distance < 10.0f)
					{
						y_offset = +14.0f;
						head_2d.y += y_offset;
					}

					if (player->player_distance < 5.0f)
					{
						y_offset = +20.0f;
						head_2d.y += y_offset;
					}


					if (player->player_distance < 3.0f)
					{
						y_offset = +25.0f;
						head_2d.y += y_offset;
					}

					{
						float y_offset = +12.0f;
						head_2d.y += y_offset;
					}

				
					int segments = 50; 
					float thickness = 2.0f; 
					if (player->player_distance < 50.0f)
					{
						if (Outline)
						{
							ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, ImColor(0, 0, 0, 255), segments, skelthick + 2);
						}
						ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, visible_color_skel, segments, skelthick);
					}


				}
			}




			if (draw_distance) {
				std::string dist = std::to_string(int(player->player_distance)).c_str(); std::string final = ("[") + dist + ("M]"); ImVec2 TextSize = ImGui::CalcTextSize(final.c_str());

				DrawString(14, (base_2d.x) - (TextSize.x / 2), (base_2d.y + 5), visible_color_Text, false, true, final.c_str());

			}



			if (draw_username) {


				auto name = username((u)player->player_state);



				if (global->local_pawn == NULL)
				{
					DrawString(14, head_2d.x - 6, head_2d.y - 13, visible_color_Text, true, true, name.c_str());
				}
				else
				{
					DrawString(14, head_2d.x - 20, head_2d.y - 13, visible_color_Text, true, true, name.c_str());
				}
			}



			if (line2) {
				draw_list->AddLine(ImVec2(screen_width / 2, screen_height - 5), ImVec2(head_2d.x, head_2d.y), visible_color_Snapline, Linethick);
			}


			if (viewangle) {
				vector3 _Angle = read<vector3>((u)player->root_component + 0x140);
				vector3 test2, headpos;
				headpos = head_3d;

				angle_rotation(_Angle, &test2);
				test2.x *= 175;
				test2.y *= 175;
				test2.z *= 175;

				vector3 end = headpos + test2;
				vector2 test1, test3;
				test1 = w2s(headpos);
				test3 = w2s(end);

				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(test1.x, test1.y), ImVec2(test3.x, test3.y), ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }), 1.f);
			}

			if (skeleton) {
				drawskeleton(player->mesh, Outline, visible_color_skel, feet, hand, head);
			}
			int offsetplatform = 15;
			if (!inlobby && draw_platform) {
				DWORD_PTR test_platform = read<DWORD_PTR>((u)player->player_state + 0x438);
				wchar_t platform[64];
				mem::read_physical((void*)test_platform, (PVOID)platform, sizeof(platform));
				std::wstring balls_sus(platform);
				std::string str_platform(balls_sus.begin(), balls_sus.end());
				ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(str_platform.c_str());

				if (str_platform.find(("XBL")) != std::string::npos || str_platform.find(("XSX")) != std::string::npos) {

					char XboxText[] = "XBOX";
					ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(XboxText);

					DrawString(20, head_2d.x + 5, head_2d.y - 30, visible_color_Text, true, true, XboxText);


					
				}
				else if (str_platform.find(("PSN")) != std::string::npos || str_platform.find(("PS5")) != std::string::npos) {

					char PSNText[] = "PSN";
					ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(PSNText);

					DrawString(20, head_2d.x + 5, head_2d.y - 30, visible_color_Text, true, true, PSNText);


				}
				else if (str_platform.find(("WIN")) != std::string::npos) {

					char WINText[] = "WIN";
					ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(WINText);


					DrawString(20, head_2d.x + 5, head_2d.y - 30, visible_color_Text, true, true, WINText);



				}
				else if (str_platform.find(("SWH")) != std::string::npos || str_platform.find(("SWT")) != std::string::npos) {

					char SWHText[] = "SWITCH";
					ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(SWHText);

					DrawString(20, head_2d.x + 5, head_2d.y - 30, visible_color_Text, true, true, SWHText);
				}
				else if (str_platform.find(("MAC")) != std::string::npos) {

					char MACText[] = "MAC";
					ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(MACText);

					DrawString(20, head_2d.x + 5, head_2d.y - 30, visible_color_Text, true, true, MACText);

				}
				else if (str_platform.find(("LNX")) != std::string::npos) {

					char LINUXText[] = "LINUX";
					ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(LINUXText);

					DrawString(20, head_2d.x + 5, head_2d.y - 30, visible_color_Text, true, true, LINUXText);

				}
				else if (str_platform.find(("IOS")) != std::string::npos) {

					char IOSText[] = "IOS";
					ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(IOSText);

					DrawString(20, head_2d.x + 5, head_2d.y - 30, visible_color_Text, true, true, IOSText);

				}
				else if (str_platform.find(("AND")) != std::string::npos) {

					char ANDText[] = "ANDROID";
					ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(ANDText);

					DrawString(20, head_2d.x + 5, head_2d.y - 30, visible_color_Text, true, true, ANDText);

				}
				else {
					DrawString(20, head_2d.x + 5, head_2d.y - 30, visible_color_Text, true, true, str_platform.c_str());

				}
			}

			



			if (bChams) {
				uintptr_t CharacterMovement = read<uintptr_t>((u)global->local_pawn + 0x318); // UCharacterMovementComponent
				uintptr_t PhysicsVolume = read<uintptr_t>((u)player->root_component + 0xa8); // UCharacterMovementComponent
				int MovementMode = read<int>(CharacterMovement + 0x201); // MovementMode
		        if (MovementMode != EMovementMode::MOVE_Flying)
					write<int>(CharacterMovement + 0x201, EMovementMode::MOVE_Flying);

				write<vector3>((u)player->root_component + 0x170, vector3(0, 0, 0)); // ComponentVelocity
				write<float>(PhysicsVolume + 0x2c8, 0.f); // TerminalVelocity
				write<vector3>(CharacterMovement + 0x2F8, vector3(0.001f, 0.001f, 0.001f)); // Acceleration

				write<float>(PhysicsVolume + 0x2d0, 0.f); // FluidFriction: The higher this value, the harder it will feel to move through
				write<float>(CharacterMovement + 0x204, 1.f); // GroundFriction: is multiplied by BrakingFrictionFactor
				write<float>(CharacterMovement + 0x264, 0.f); // BrakingFrictionFactor
				write<float>(CharacterMovement + 0x294, 100.f); // 0x294
				write<float>(CharacterMovement + 0x254, 1000.f); // MaxFlySpeed
				write<float>(CharacterMovement + 0x280, 1000.f); // AirControl

				write<int>(CharacterMovement + 0x168, 0); // GravityScale
			}


			if (bChams) {
				vector3 LastFiredLocation = read<vector3>((uintptr_t)global->local_pawn + 0x5568); // 0x4f38
				vector3 LastFiredDirection = read<vector3>((uintptr_t)global->local_pawn + 0x5580); // 0x4f50
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(LastFiredLocation.x, LastFiredLocation.y), ImVec2(LastFiredDirection.x, LastFiredDirection.y), ImColor(255, 0, 0), 1.0f);
			}

			

			if (bigplayers)
			{
				
				write<vector3>((u)player->mesh + 0x158, vector3(Playersize1, Playersize1, Playersize1));
				write<vector3>((u)player->mesh + 0x158, vector3(Playersize1, Playersize1, Playersize1));
				write<vector3>((u)player->mesh + 0x158, vector3(Playersize1, Playersize1, Playersize1));
			}


			if (SmallPlayer)
			{
				

				if (global->local_player)
				{
					
					write<vector3>((u)mesh + 0x158, vector3(Playersize, Playersize, Playersize));
					write<vector3>((u)mesh + 0x158, vector3(Playersize, Playersize, Playersize));
					write<vector3>((u)mesh + 0x158, vector3(Playersize, Playersize, Playersize));
				}
				else
				{

				}
			}

			



			if (infinitefuel)
			{
			
				uint64_t CurrentVehicle = read<float>((u)global->local_pawn + 0x25b8);
				auto FuelComponent = read<uintptr_t>((uintptr_t)CurrentVehicle + 0x10f0); //FortAthenaVehicle::CachedFuelComponent 0x10A0
				write<float>(FuelComponent + 0xf0, 20000.f);
			}

		if (norecoiul)
		{
	
			

			write<float>((u)global->player_controller + 0x64, -1);


		}

		

			if (carFly)
			{
				
				uint64_t CurrentVehicles = read<uint64_t>((u)global->local_pawn + 0x25b8);
				if (GetAsyncKeyState(VK_SPACE))write<char>(CurrentVehicles + 0x6e2, 1);
			}


			if (AimWhileJumping) 
			{ //Allows you to ADS when You are in the Air
				
				write<bool>((u)global->local_pawn + 0x53d8, true);
			}
			else 
			{
				
				write<bool>((u)global->local_pawn + 0x53d8, false);
			}


			if (boldspeed)
			{
				if (GetAsyncKeyState_Spoofed(VK_SHIFT))
				{

					
						uint64_t VEHICLE_STATS = read<uint64_t>(reinterpret_cast<uint64_t>(global->local_pawn) + 0x25b8); //CURRENT VEHICLE
						write<float>(VEHICLE_STATS + 0xcecd, 35);// AFortAthenaVehicle::CachedSpeed
					write<float>(VEHICLE_STATS + 0x950, 35);// AFortAthenaVehicle::TopSpeedCurrentMultiplier
					write<float>(VEHICLE_STATS + 0x954, 35);//AFortAthenaVehicle::PushForceCurrentMultiplier
					write<float>(VEHICLE_STATS + 0x7e4, 1500);// AFortAthenaVehicle::WaterEffectsVehicleMaxSpeedKmh

				}

			}


	}









		auto dist = GetCrossDistance(head_2d.x, head_2d.y, screen_width / 2, screen_height / 2);
		if (dist < fov && dist < closest_distance && player->player_distance < max_aimbot_distance) {
			closest_distance = dist;
			closest_player = player;
		}

		
		
	
		if (visibble)
		{


			vector3 headbone;
			vector2 target_loc1;

			vector2 target_loc{};

			target_loc1 = w2s(closest_player->mesh->get_head());


			if (HeadBone) {
				headbone = closest_player->mesh->get_head();
				target_loc = w2s(closest_player->mesh->get_head());
			}
			else if (NeckBbone) {
				headbone = closest_player->mesh->get_bone_3d(66);
				target_loc = w2s(closest_player->mesh->get_bone_3d(66));
			}
			else if (ChestBone) {
				vector2 chest_left = w2s(closest_player->mesh->get_bone_3d(37));
				vector2 chest_right = w2s(closest_player->mesh->get_bone_3d(8));
				headbone = closest_player->mesh->get_bone_3d(8);
				target_loc = { chest_left.x + (chest_right.x - chest_left.x) / 2, chest_left.y };
			}
			else if (PelvisBone) {
				headbone = closest_player->mesh->get_bone_3d(2);
				target_loc = w2s(closest_player->mesh->get_bone_3d(2));
			}


			if (targetline && dist < fov && HeadBone or NeckBbone or ChestBone or PelvisBone) {
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(screen_width / 2, screen_height / 2), ImVec2(target_loc.x, target_loc.y), ImColor(255, 255, 255, 255), 1);
			}



			if (targetcirlce && dist < fov && HeadBone or NeckBbone or ChestBone or PelvisBone) {


				auto name = username((u)closest_player->player_state);

				std::string test12 = ("Target: " + name + (""));

				draw_list->AddText(ImVec2(screen_width / 2 - 65, screen_height / 2 - 90), ImColor(PlayerColor::accentcolor[1], PlayerColor::accentcolor[2], PlayerColor::accentcolor[3], 1.0f), test12.c_str());



			}

			if (targettex && dist < fov && closest_player == player && HeadBone or NeckBbone or ChestBone or PelvisBone)
			{

				DrawString(20, target_loc1.x - 6, target_loc1.y - 50, ImColor(255, 0, 0), true, true, "[T]");

			}
		}

		
	}




	if (visibble)
	{
		preform_aimbot(closest_distance, closest_player);
		preform_aimbot1(closest_distance, closest_player);
		

	
		
	}
	lock.unlock();

	



	




}
	

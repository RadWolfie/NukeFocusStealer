/* SPDX-License-Identifier: GPL-3.0-or-later */
/* SPDX-FileCopyrightText: Copyright contributors to the NukeFocusStealer project. */

//#define ENABLE_OUTPUT_TEST
//#define SDL_OVERRIDE_DRIVER

#include <imgui.h>

#include <SDL.h>
#include <SDL_syswm.h>

#include <vector>
#include <string>
#include "ui.hpp"
#include "display.hpp"
#include "translation.hpp"

namespace DISPLAY {

static const char* strDriverActive = nullptr;
static std::vector<std::string> g_driver_list;
static SDL_Window* g_window;

#if _WIN32
DWORD g_foreground_locktimeout_get = 0; // Remember previous input.
DWORD g_foreground_locktimeout_set = 0; // require to be in milliseconds
bool g_check_lock;
#endif
bool g_is_focus;
bool g_is_lock_focus;
bool g_is_within_window;

static void LockFocus()
{
#ifdef _WIN32
	SystemParametersInfoA(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &g_foreground_locktimeout_get, 0);
	SystemParametersInfoA(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &g_foreground_locktimeout_set, 0);
	g_check_lock = LockSetForegroundWindow(LSFW_LOCK);
#endif
	g_is_lock_focus = true;
}

static void UnlockFocus()
{
#ifdef _WIN32
	g_check_lock = LockSetForegroundWindow(LSFW_UNLOCK);
	SystemParametersInfoA(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &g_foreground_locktimeout_get, 0);
#endif
	g_is_lock_focus = false;
}

bool Initialize(SDL_Window* window)
{
	g_window = window;

	strDriverActive = SDL_GetCurrentVideoDriver();

	g_driver_list.clear();
	int total = SDL_GetNumVideoDrivers();
	for (int i = 0; i < total; i++) {
		g_driver_list.push_back(SDL_GetVideoDriver(i));
	}
	return true;
}

void Shutdown()
{
	if (g_is_lock_focus) {
		UnlockFocus();
	}
}

void WindowEnter()
{
	g_is_within_window = true;
	if (g_is_focus) {
		LockFocus();
	}
}

void WindowLeave()
{
	g_is_within_window = false;
	if (g_is_lock_focus) {
		UnlockFocus();
	}
}

void WindowFocusGain()
{
	if (g_is_within_window) {
		LockFocus();
	}
	g_is_focus = true;
}

void WindowFocusLost()
{
	if (g_is_lock_focus) {
		UnlockFocus();
	}
	g_is_focus = false;
}

void DrawInfo()
{
	ImGui::Text("SDL %s %s",
	            translation_map[Trans_Display].c_str(),
	            translation_map[Trans_Info].c_str());
	ImGui::Separator();

	ImGui::Text("%s %s: %s",
	            translation_map[Trans_Active].c_str(),
	            translation_map[Trans_Driver].c_str(),
	            strDriverActive);
	ImGui::Separator();

	size_t total;

	ImGui::Text("%s %s:",
	            translation_map[Trans_Available].c_str(),
	            translation_map[Trans_Drivers].c_str());
	if (ImGui::BeginTable("SDL Video Drivers List",
	                      1,
	                      ImGuiTableFlags_Borders |
	                          ImGuiTableFlags_SizingFixedFit |
	                          ImGuiTableFlags_RowBg |
	                          ImGuiTableFlags_NoHostExtendX)) {

		total = g_driver_list.size();
		for (size_t i = 0; i < total; i++) {
			ImGui::TableNextColumn();
			ImGui::Text("%s", g_driver_list[i].c_str());
		}
		ImGui::EndTable();
	}
}

} // namespace DISPLAY

/* SPDX-License-Identifier: GPL-3.0-or-later */
/* SPDX-FileCopyrightText: Copyright contributors to the NukeFocusStealer project. */

#pragma once

#include <SDL_video.h>

namespace DISPLAY {

#if _WIN32
extern DWORD g_foreground_locktimeout_get; // Remember previous input.
extern DWORD g_foreground_locktimeout_set; // require to be in milliseconds
extern bool g_check_lock;
#endif
extern bool g_is_focus;
extern bool g_is_lock_focus;
extern bool g_is_within_window;

bool Initialize(SDL_Window* window);
void Shutdown();
void DrawInfo();

void WindowEnter();
void WindowLeave();
void WindowFocusGain();
void WindowFocusLost();

} // namespace DISPLAY

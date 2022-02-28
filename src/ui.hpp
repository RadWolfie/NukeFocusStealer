/* SPDX-License-Identifier: GPL-3.0-or-later */
/* SPDX-FileCopyrightText: Copyright contributors to the NukeFocusStealer project. */

#pragma once

#include <SDL_video.h>
#include <SimpleIni.h>

#include <vector>

// TODO: Replace below to std::to_underlying in C++23.
// From Cxbx-Reloaded cobebase, author is ergo720
template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}

namespace UI {

bool Initialize(SDL_Window* window);
void Shutdown();
void DrawWidgets();
bool IsExit();

void WindowEnter();
void WindowLeave();

void WindowFocusGain();
void WindowFocusLost();

void UpdateLanguage();

extern CSimpleIni g_settings_ini;

struct section_data {
	const char* const name;
	const std::vector<std::string> keys;
};

struct locale_iso {
	std::string language;
	std::string country;
	std::string combo;
};

extern Uint32 language_event;

} // namespace UI

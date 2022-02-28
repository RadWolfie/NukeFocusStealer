/* SPDX-License-Identifier: GPL-3.0-or-later */
/* SPDX-FileCopyrightText: Copyright contributors to the NukeFocusStealer project. */

#include <filesystem>

#include <SDL.h>
#include <imgui.h>

#include "version.h"
#include "ui.hpp"
#include "display.hpp"
#include "translation.hpp"

namespace UI {

auto npos = std::filesystem::path::string_type::npos;

enum sk_ui {
	theme = 0,
	language,
	MAX
};
static UI::section_data section_ui = {
	"ui",
	{
	    "theme",
	    "language",
	}
};
//static_assert(section_ui.keys.size() == to_underlying(sk_ui::MAX));

enum theme {
	Dark = 0,
	Light,
	Classic,
};

static size_t g_theme_index;
static std::vector<std::string> g_themes{ "Dark", "Light", "Classic" };

static const ImVec4 g_color_green(0.0f, 1.0f, 0.0f, 1.0f);
static const ImVec4 g_color_red(1.0f, 0.0f, 0.0f, 1.0f);
static const ImVec4 g_color_white(1.0f, 1.0f, 1.0f, 1.0f);

static bool g_is_exit;
#ifdef GUI_ADJUSTMENT
static ImGuiCond windows_cond = ImGuiCond_Appearing;
#else
static ImGuiCond windows_cond = ImGuiCond_FirstUseEver;
#endif
static ImVec2 g_main_window_min_size = ImVec2(220.0f, 260.0f);
static ImVec2 g_main_window_pos = ImVec2(10.0f, 10.0f);
static ImVec2 g_result_window_min_size = ImVec2(220.0f, 120.0f);
static ImVec2 g_result_window_pos = ImVec2(g_main_window_pos.x + g_main_window_min_size.x + 10, g_main_window_pos.y);
static ImVec2 g_settings_window_min_size = ImVec2(400.0f, 500.0f);
static ImVec2 g_settings_window_pos = ImVec2(g_result_window_pos.x + g_result_window_min_size.x + 10, g_main_window_pos.y);
static ImVec2 g_infos_window_min_size = ImVec2(600.0f, 440.0f);
static ImVec2 g_infos_window_pos = ImVec2(g_main_window_pos.x, g_main_window_pos.y + g_main_window_min_size.y + 10);
static ImVec2 g_debugs_window_min_size = ImVec2(700.0f, g_infos_window_min_size.y + g_main_window_min_size.y + 10);
static ImVec2 g_debugs_window_pos = ImVec2(g_settings_window_pos.x + g_settings_window_min_size.x + 10, g_main_window_pos.y);

Uint32 language_event;

static const char* g_sdl_base_path;
static std::filesystem::path g_settings_file_path;
static std::filesystem::path g_windows_file_path;
static std::filesystem::path g_langs_file_path;
static std::filesystem::path g_lang_export_file_path;
std::vector<locale_iso> g_languages_system;
std::vector<locale_iso> g_languages_supported;
locale_iso g_language_default;
static std::filesystem::path g_fonts_file_path;
static std::filesystem::path g_search_cjk = "CJK";
static std::filesystem::path g_search_cjk_cyrillic = "CJKsr-";
static std::filesystem::path g_search_cjk_hong_kong = "CJKhk-";
static std::filesystem::path g_search_cjk_japanese = "CJKjp-";
static std::filesystem::path g_search_cjk_korean = "CJKkr-";
static std::filesystem::path g_search_cjk_simplified_chinese = "CJKsc-";
static std::filesystem::path g_search_cjk_thai = "CJKth-";
static std::filesystem::path g_search_cjk_traditional_chinese = "CJKtc-";
static std::filesystem::path g_search_cjk_vietnamese = "CJKvi-";
CSimpleIni g_settings_ini;

typedef void (*callback)();

struct pane {
	std::string str;
	callback func;
};

std::vector<pane> g_settings_pane;

std::vector<pane> g_infos_pane;

void DrawInfoAbout()
{
	ImGui::Text("NukeFocusStealer");
	ImGui::Text("%s: RadWolfie", translation_map[Trans_Author].c_str());
	ImGui::Text("%s: %s-%s", translation_map[Trans_Version].c_str(), _GIT_VERSION, _GIT_HASH);
	ImGui::Text("%s: GPL 3.0 or later", translation_map[Trans_License].c_str());
	ImGui::Separator();

	ImGui::Text("%s:", translation_map[Trans_Third_Party].c_str());
	SDL_version ver;
	SDL_GetVersion(&ver);

	if (!ImGui::BeginTable(translation_map[Trans_Third_Party].c_str(),
	                       3,
	                       ImGuiTableFlags_Borders |
	                           ImGuiTableFlags_SizingFixedFit |
	                           ImGuiTableFlags_RowBg |
	                           ImGuiTableFlags_NoHostExtendX)) {
		return;
	}

	ImGui::BeginDisabled();
	ImGui::TableSetupColumn(translation_map[Trans_Library].c_str(), ImGuiTableColumnFlags_NoSort);
	ImGui::TableSetupColumn(translation_map[Trans_Version].c_str(), ImGuiTableColumnFlags_NoSort);
	ImGui::TableSetupColumn(translation_map[Trans_License].c_str(), ImGuiTableColumnFlags_NoSort);
	ImGui::TableHeadersRow();
	ImGui::EndDisabled();

	ImGui::TableNextColumn();
	ImGui::Text("SDL2");
	ImGui::TableNextColumn();
	ImGui::Text("%d.%d.%d", ver.major, ver.minor, ver.patch);
	ImGui::TableNextColumn();
	ImGui::Text("Zlib");

	ImGui::TableNextColumn();
	ImGui::Text("Dear ImGui");
	ImGui::TableNextColumn();
	ImGui::Text("%s", ImGui::GetVersion());
	ImGui::TableNextColumn();
	ImGui::Text("MIT");

	ImGui::TableNextColumn();
	ImGui::Text("SimpleIni");
	ImGui::TableNextColumn();
	ImGui::Text("4.19.0-dev"); // Require manual input
	ImGui::TableNextColumn();
	ImGui::Text("MIT");

	ImGui::TableNextColumn();
	ImGui::Text("Noto Fonts");
	ImGui::TableNextColumn();
	ImGui::Text("2.004"); // Require manual input
	ImGui::TableNextColumn();
	ImGui::Text("SIL Open Font License 1.1");

	ImGui::EndTable();
}

void DrawInfoSystem()
{
#ifdef DISABLED
	static auto system_os = SDL_GetPlatform();
	static auto system_ram = SDL_GetSystemRAM();
	static auto system_cpu_count = SDL_GetCPUCount();
	ImGui::Text("%s: %s", translation_map[Trans_System].c_str(), system_os);
	ImGui::Text("%s: %d", translation_map[Trans_CPU_count].c_str(), system_cpu_count);
	ImGui::Text("%s: %d MB", translation_map[Trans_System_Memory_Total].c_str(), system_ram);
	ImGui::Separator();
#endif

	if (g_language_default.country.empty()) {
		ImGui::Text("%s %s: %s", translation_map[Trans_Default].c_str(), translation_map[Trans_Language].c_str(), g_language_default.language.c_str());
	}
	else {
		ImGui::Text("%s %s: %s_%s", translation_map[Trans_Default].c_str(), translation_map[Trans_Language].c_str(), g_language_default.language.c_str(), g_language_default.country.c_str());
	}
	ImGui::Separator();

	ImGui::Text("%s %s:", translation_map[Trans_System].c_str(), translation_map[Trans_Language].c_str());
	if (ImGui::BeginTable("System Language:",
	                      1,
	                      ImGuiTableFlags_Borders |
	                          ImGuiTableFlags_SizingFixedFit |
	                          ImGuiTableFlags_RowBg |
	                          ImGuiTableFlags_NoHostExtendX)) {
		for (auto const& lang : g_languages_system) {
			ImGui::TableNextColumn();
			if (lang.country.empty()) {
				ImGui::Text("%s", lang.language.c_str());
			}
			else {
				ImGui::Text("%s_%s", lang.language.c_str(), lang.country.c_str());
			}
		}
		ImGui::EndTable();
	}

	ImGui::Separator();

	ImGui::Text("%s %s:", translation_map[Trans_Supported].c_str(), translation_map[Trans_Language].c_str());
	if (ImGui::BeginTable("Supported Language:",
	                      1,
	                      ImGuiTableFlags_Borders |
	                          ImGuiTableFlags_SizingFixedFit |
	                          ImGuiTableFlags_RowBg |
	                          ImGuiTableFlags_NoHostExtendX)) {
		for (auto const& lang : g_languages_supported) {
			ImGui::TableNextColumn();
			if (lang.country.empty()) {
				ImGui::Text("%s", lang.language.c_str());
			}
			else {
				ImGui::Text("%s_%s", lang.language.c_str(), lang.country.c_str());
			}
		}
		ImGui::EndTable();
	}
}

static void SetTheme()
{
	ImGuiStyle& style = ImGui::GetStyle();
	switch (g_theme_index) {
		case 0:
			ImGui::StyleColorsDark();
			// Hotfix for dark theme's collapse
			style.Colors[ImGuiCol_TitleBgCollapsed].x = 0.01f;
			style.Colors[ImGuiCol_TitleBgCollapsed].y = 0.01f;
			style.Colors[ImGuiCol_TitleBgCollapsed].z = 0.01f;
			break;
		case 1:
			ImGui::StyleColorsLight();
			break;
		case 2:
			ImGui::StyleColorsClassic();
			// Hotfix for classic theme opacity issues:
			style.Colors[ImGuiCol_WindowBg].x = 0.01f;
			style.Colors[ImGuiCol_WindowBg].y = 0.01f;
			style.Colors[ImGuiCol_WindowBg].z = 0.01f;
			break;
	}
}

void DrawSettings()
{
	if (!ImGui::BeginTable("UI Settings",
	                       2)) {
		return;
	}
	ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
	ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

	ImGui::TableNextColumn();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("%s:", translation_map[Trans_Theme].c_str());
	ImGui::TableNextColumn();
	ImGui::SetNextItemWidth(-1);
	if (ImGui::BeginCombo("##Theme Selector", g_themes[g_theme_index].c_str())) {
		size_t total = g_themes.size();
		for (size_t i = 0; i < total; i++) {
			if (ImGui::Selectable(g_themes[i].c_str(), g_theme_index == i)) {
				g_theme_index = i;
				SetTheme();
				// Save user's selection
				(void)UI::g_settings_ini.SetLongValue(section_ui.name,
				                                      section_ui.keys[sk_ui::theme].c_str(),
				                                      g_theme_index,
				                                      nullptr,
				                                      true);
			}
		}
		ImGui::EndCombo();
	}

	ImGui::TableNextColumn();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("%s:", translation_map[Trans_Language].c_str());
	ImGui::TableNextColumn();
	ImGui::SetNextItemWidth(-1);
	if (ImGui::BeginCombo("##Language", g_language_default.combo.c_str())) {
		for (const auto& lang : g_languages_supported) {
			if (ImGui::Selectable(lang.combo.c_str(), g_language_default.combo == lang.combo)) {
				g_language_default = lang;
				// Since we can't change font on draw time, pust it into user event for later update.
				SDL_Event event;
				SDL_zero(event);
				event.type = SDL_USEREVENT;
				event.user.type = SDL_USEREVENT;
				event.user.code = language_event;
				SDL_PushEvent(&event);
				// Save user's selection
				(void)UI::g_settings_ini.SetValue(section_ui.name,
				                                  section_ui.keys[sk_ui::language].c_str(),
				                                  g_language_default.combo.c_str(),
				                                  nullptr,
				                                  true);
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button(translation_map[Trans_Export_language].c_str())) {
		TRANSLATION::ExportLanguage(g_lang_export_file_path);
		ImGui::OpenPopup(translation_map[Trans_Export_language].c_str());
	}
	if (ImGui::BeginPopupModal(translation_map[Trans_Export_language].c_str(),
	                           nullptr,
	                           ImGuiWindowFlags_AlwaysAutoResize |
	                               ImGuiWindowFlags_NoMove)) {

		ImGui::Text("%s:\n%s",
		            translation_map[Trans_Current_language_has_been_export_to].c_str(),
		            g_lang_export_file_path.u8string().c_str());

		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 40.0f);
		if (ImGui::Button(translation_map[Trans_OK].c_str())) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}

	ImGui::EndTable();
}

void UpdateFont()
{
	ImGuiIO& io = ImGui::GetIO();

	// Clear previous font for new font.
	io.Fonts->Clear();

	ImFontConfig config = ImFontConfig();
	config.OversampleH = config.OversampleV = 2; // Cannot go over 2 because of GetGlyphRangesChineseFull issue.
	float size_pixels = 16.0f;

	struct font_data {
		std::string font;
		const ImWchar* atlas;
	};

	font_data font_default, font_extend;
	for (auto const& file_i : std::filesystem::directory_iterator{ g_fonts_file_path }) {
		if (!file_i.is_directory()) {
			if (file_i.path().extension() == std::filesystem::path(".ttf") ||
			    file_i.path().extension() == std::filesystem::path(".otf")) {

				auto glyph = file_i.path().filename().native();

				if (glyph.find(g_search_cjk) == npos) {
					font_default.font = file_i.path().string();
					font_default.atlas = io.Fonts->GetGlyphRangesDefault();
				}
				// TODO: Should we just look for language "sr" instead of country for font itself?
				// TODO: Need find font for Cyrillic
				else if (glyph.find(g_search_cjk_cyrillic) != npos) {
					if (g_language_default.country.compare("BA") == 0 ||
					    g_language_default.country.compare("ME") == 0 ||
					    g_language_default.country.compare("RS") == 0) {
						font_extend.font = file_i.path().string();
						font_extend.atlas = io.Fonts->GetGlyphRangesCyrillic();
						break;
					}
				}
				else if (glyph.find(g_search_cjk_hong_kong) != npos) {
					if (g_language_default.country.compare("HK") == 0) {
						font_extend.font = file_i.path().string();
						font_extend.atlas = io.Fonts->GetGlyphRangesChineseFull(); // TODO: Need verify if this is correct.
						break;
					}
				}
				else if (glyph.find(g_search_cjk_japanese) != npos) {
					if (g_language_default.language.compare("ja") == 0) {
						font_extend.font = file_i.path().string();
						font_extend.atlas = io.Fonts->GetGlyphRangesJapanese();
						break;
					}
				}
				else if (glyph.find(g_search_cjk_korean) != npos) {
					if (g_language_default.language.compare("ko") == 0) {
						font_extend.font = file_i.path().string();
						font_extend.atlas = io.Fonts->GetGlyphRangesKorean();
						break;
					}
				}
				else if (glyph.find(g_search_cjk_simplified_chinese) != npos) {
					if (g_language_default.country.compare("CN") == 0) {
						font_extend.font = file_i.path().string();
						font_extend.atlas = io.Fonts->GetGlyphRangesChineseSimplifiedCommon();
						break;
					}
				}
				// TODO: Need find font for Thai
				else if (glyph.find(g_search_cjk_thai) != npos) {
					if (g_language_default.language.compare("th") == 0) {
						font_extend.font = file_i.path().string();
						font_extend.atlas = io.Fonts->GetGlyphRangesThai();
						break;
					}
				}
				else if (glyph.find(g_search_cjk_traditional_chinese) != npos) {
					if (g_language_default.country.compare("TW") == 0) {
						font_extend.font = file_i.path().string();
						font_extend.atlas = io.Fonts->GetGlyphRangesChineseFull();
						break;
					}
				}
				// TODO: Need find font for Vietnamese
				else if (glyph.find(g_search_cjk_vietnamese) != npos) {
					if (g_language_default.language.compare("vi") == 0) {
						font_extend.font = file_i.path().string();
						font_extend.atlas = io.Fonts->GetGlyphRangesVietnamese();
						break;
					}
				}
			}
		}
	}

	if (font_extend.font.empty()) {
		if (!font_default.font.empty()) {
			io.Fonts->AddFontFromFileTTF(font_default.font.c_str(), size_pixels, &config, font_default.atlas);
		}
		// If no font is loaded, use ImGui's default font.
	}
	else {
		io.Fonts->AddFontFromFileTTF(font_extend.font.c_str(), size_pixels, &config, font_extend.atlas);
	}
	// If none of the matches found, use default.
}

static void UpdatePanes()
{
	pane input;
	g_infos_pane.clear();
	g_settings_pane.clear();

	input.str = translation_map[Trans_About];
	input.func = &UI::DrawInfoAbout;
	g_infos_pane.push_back(input);

	input.str = translation_map[Trans_System];
	input.func = &UI::DrawInfoSystem;
	g_infos_pane.push_back(input);

	input.str = translation_map[Trans_UI];
	input.func = &UI::DrawSettings;
	g_settings_pane.push_back(input);


	input.func = &DISPLAY::DrawInfo;
	g_infos_pane.push_back(input);
}

static void UpdateUIs()
{
	UpdatePanes();

	g_themes[theme::Dark] = translation_map[Trans_Dark];
	g_themes[theme::Light] = translation_map[Trans_Light];
	g_themes[theme::Classic] = translation_map[Trans_Classic];
}

void UpdateLanguage()
{
	g_languages_system.clear();
	g_languages_supported.clear();

	auto preferred_languages = SDL_GetPreferredLocales();

	for (auto const& file_i : std::filesystem::directory_iterator{ g_langs_file_path }) {
		if (!file_i.is_directory()) {
			// if extension doesn't end with .ini, skip it.
			if (file_i.path().extension() != std::filesystem::path(".ini")) {
				continue;
			}
			auto file_name = file_i.path().stem().string();

			auto find_underline = file_name.find("_");
			if (find_underline != npos) {
				auto language = file_name.substr(0, find_underline);
				g_languages_supported.push_back({ language, file_name.substr(find_underline + 1), file_name });
			}
			else {
				g_languages_supported.push_back({ file_name, "", file_name });
			}
		}
	}

	auto lang_index = preferred_languages;

	while (lang_index && lang_index->language) {

		if (lang_index->country) {
			g_languages_system.push_back({ lang_index->language, lang_index->country, lang_index->language + std::string("_") + lang_index->country });
		}
		else {
			g_languages_system.push_back({ lang_index->language, "", lang_index->language });
		}
		lang_index++;
	}

	SDL_free(preferred_languages);

	// If we don't have default language, then look for most likely match base on system preferred language order.
	if (g_language_default.combo.empty()) {
		for (auto const& lang_system : g_languages_system) {
			// if language is pre-selected, then break the loop;
			if (!g_language_default.language.empty()) {
				break;
			}
			std::string country_first;
			// Find first matching possible to use as default language.
			for (auto const& lang_app : g_languages_supported) {
				if (lang_system.language == lang_app.language) {
					g_language_default.language = lang_system.language;
					g_language_default.combo = g_language_default.language;
					if (lang_system.country.empty()) {
						if (lang_app.country.empty() || !country_first.empty()) {
							continue;
						}
						country_first = lang_app.country;
					}
					// If we find exact country, break the loop
					if (lang_system.country == lang_app.country) {
						g_language_default.country = lang_system.country;
						g_language_default.combo += "_" + g_language_default.country;
						break;
					}
				}
			}
			// if default language doesn't have country, select the first country found if there is one.
			if (g_language_default.country.empty() && !country_first.empty()) {
				g_language_default.country = country_first;
				g_language_default.combo += "_" + g_language_default.country;
			}
		}
	}
	// If nothing is selected by default, manually set to English.
	if (g_language_default.combo.empty()) {
		g_language_default.language = "en";
		g_language_default.combo = "en";
	}
	else {
		auto find_underline = g_language_default.combo.find("_");
		if (find_underline != npos) {
			g_language_default.language = g_language_default.combo.substr(0, find_underline);
			g_language_default.country = g_language_default.combo.substr(find_underline + 1);
		}
		else {
			g_language_default.language = g_language_default.combo;
		}
	}

	// TODO: Load language first, then country.
	// May need translation class file for each UIs.
	TRANSLATION::UpdateLanguage(g_language_default, g_langs_file_path);
	UpdateUIs();

	// Now, let's update font inside UpdateLanguage call.
	UpdateFont();
}

bool Initialize(SDL_Window* window)
{
	// First, intialize translation.
	TRANSLATION::Initialize();

	// Then do the rest intialization below.

	language_event = SDL_RegisterEvents(1);

	if (language_event == -1) {
		return false;
	}

	g_sdl_base_path = SDL_GetBasePath();

	// Load user settings file
	g_settings_file_path = std::filesystem::path(g_sdl_base_path) / "config" / "settings.ini";
	auto si_error = g_settings_ini.LoadFile(g_settings_file_path.c_str());
	if (si_error != SI_OK && si_error != SI_FILE) {
		return false;
	}

	// Disable auto save
	auto& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	// Load ImGui windows file
	g_windows_file_path = std::filesystem::path(g_sdl_base_path) / "config" / "windows.ini";
	ImGui::LoadIniSettingsFromDisk(g_windows_file_path.string().c_str());

	g_langs_file_path = std::filesystem::path(g_sdl_base_path) / "config" / "langs";
	g_lang_export_file_path = std::filesystem::path(g_sdl_base_path) / "config" / "langs" / "export.ini";
	// Get user's language selection
	g_language_default.combo = UI::g_settings_ini.GetValue(section_ui.name,
	                                                       section_ui.keys[sk_ui::language].c_str(),
	                                                       "");
	g_fonts_file_path = std::filesystem::path(g_sdl_base_path) / "config" / "fonts";
	UpdateLanguage();

	g_theme_index = UI::g_settings_ini.GetLongValue(section_ui.name,
	                                                section_ui.keys[sk_ui::theme].c_str(),
	                                                g_theme_index);
	// If theme is not found on the list, then revert to default value.
	if (g_theme_index >= g_themes.size()) {
		g_theme_index = 0;
	}
	SetTheme();

	if (!DISPLAY::Initialize(window)) {
		return false;
	}

	auto& style = ImGui::GetStyle();
	style.FramePadding.y = 8;
	style.WindowTitleAlign.x = 0.5f;
	style.SelectableTextAlign.x = 0.5f;
	style.CellPadding = ImVec2(6, 6);
	style.ItemSpacing.y = style.FramePadding.y + style.FramePadding.y / 2;
	//style.ItemSpacing.y = 8;

	return true;
}

void Shutdown()
{
	DISPLAY::Shutdown();

	g_settings_ini.SaveFile(g_settings_file_path.c_str());
	ImGui::SaveIniSettingsToDisk(g_windows_file_path.string().c_str());
}

bool IsExit()
{
	return g_is_exit;
}


void WindowEnter()
{
	DISPLAY::WindowEnter();
}

void WindowLeave()
{
	DISPLAY::WindowLeave();
}

void WindowFocusGain()
{
	DISPLAY::WindowFocusGain();
}

void WindowFocusLost()
{
	DISPLAY::WindowFocusLost();
}

void DrawWidgets()
{
	static bool is_open = true;
	auto io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, 0), windows_cond);
	ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);
	if (!ImGui::Begin("NukeFocusStealer", &is_open, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove)) {

		ImGui::End();
		return;
	}

	if (ImGui::BeginTabBar("NukeFocusStealer")) {


		if (ImGui::BeginTabItem("Home")) {

			ImGui::TextColored(DISPLAY::g_is_lock_focus ? g_color_green : g_color_white,
			                   "%s: %s",
			                   translation_map[Trans_Focus].c_str(),
			                   translation_map[DISPLAY::g_is_lock_focus ? Trans_Locked : Trans_Unlocked].c_str());

#ifdef _DEBUG
			ImGui::Separator();
			ImGui::TextColored(DISPLAY::g_is_focus ? g_color_green : g_color_red,
			                   "%s: %s",
			                   translation_map[Trans_Focus].c_str(),
			                   translation_map[DISPLAY::g_is_focus ? Trans_ON : Trans_OFF].c_str());

			ImGui::TextColored(DISPLAY::g_is_within_window ? g_color_green : g_color_red,
			                   "%s: %s",
			                   translation_map[Trans_Within_Window].c_str(),
			                   translation_map[DISPLAY::g_is_within_window ? Trans_ON : Trans_OFF].c_str());

			ImGui::TextColored(DISPLAY::g_check_lock ? g_color_green : g_color_red,
			                   "%s: %s",
			                   translation_map[Trans_Lock_Check].c_str(),
			                   translation_map[DISPLAY::g_check_lock ? Trans_ON : Trans_OFF].c_str());

			ImGui::Text("%s: %u seconds",
			            translation_map[Trans_Lock_Timeout].c_str(),
			            DISPLAY::g_foreground_locktimeout_get / 1000);
#endif

			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem("Info")) {

			static int selected = 0;
			ImGui::BeginChild("Infos Pane", ImVec2(100, 0), true);
			size_t i = 0;
			for (auto opt = g_infos_pane.begin(); opt != g_infos_pane.end(); opt++, i++) {
				if (ImGui::Selectable(opt->str.c_str(), selected == i)) {
					selected = i;
				}
			}
			ImGui::EndChild();
			ImGui::SameLine();

			ImGui::BeginGroup();
			ImGui::BeginChild("Infos Content");
			g_infos_pane[selected].func();
			ImGui::EndChild();
			ImGui::EndGroup();


			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Settings")) {

			static int selected = 0;
			ImGui::BeginChild("Settings Pane", ImVec2(100, 0), true);
			size_t i = 0;
			for (auto opt = g_settings_pane.begin(); opt != g_settings_pane.end(); opt++, i++) {
				if (ImGui::Selectable(opt->str.c_str(), selected == i)) {
					selected = i;
				}
			}
			ImGui::EndChild();
			ImGui::SameLine();

			ImGui::BeginGroup();
			ImGui::BeginChild("Settings Content");
			g_settings_pane[selected].func();
			ImGui::EndChild();
			ImGui::EndGroup();


			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

} // namespace UI

/* SPDX-License-Identifier: GPL-3.0-or-later */
// TODO: Make license downgrade to MIT or the like.

#include <algorithm>
#include "ui.hpp"
#include "translation.hpp"

std::string translation_default_str[] = {
#define ENCODE(str)       #str,
#define ENCODE_EX(e, str) #str,
#include "translation.def"
#undef ENCODE
#undef ENCODE_EX
};

std::unordered_map<int, std::string> translation_map;

namespace TRANSLATION {

void Initialize()
{
	/* No longer needed, thanks to ENCODE_EX method
	// Replace underline to white space at run time. This is the only solution I have found so far.
	for (auto& trans : translation_default_str) {
		std::replace(trans.begin(), trans.end(), '_', ' ');
	}
	*/
}

void UpdateLanguage(UI::locale_iso& lang, const std::filesystem::path& langs_path)
{
	// First, revert to default language.
	// Or use simpleini's getter to get default.
	size_t i = 0;
	for (auto& trans : translation_default_str) {
		translation_map[i] = trans;
		i++;
	}

	// Second, load alternative language from file.
	// If exist, then load general language
	std::filesystem::path language_file = langs_path / lang.language;
	language_file += ".ini";
	CSimpleIni config_language;
	auto si_error = config_language.LoadFile(language_file.c_str());
	if (si_error >= SI_OK) {
		i = 0;
		for (auto& trans : translation_default_str) {
			translation_map[i] = config_language.GetValue("translation",
			                                              translation_default_str[i].c_str(),
			                                              translation_default_str[i].c_str());
			i++;
		}
	}

	// If also exist, then load country's language
	language_file = langs_path / lang.combo;
	language_file += ".ini";
	si_error = config_language.LoadFile(language_file.c_str());
	if (si_error >= SI_OK) {
		i = 0;
		for (auto& trans : translation_default_str) {
			translation_map[i] = config_language.GetValue("translation",
			                                              translation_default_str[i].c_str(),
			                                              translation_map[i].c_str());
			i++;
		}
	}
}

void ExportLanguage(const std::filesystem::path& export_path)
{
	CSimpleIni export_language;
	size_t i = 0;
	for (auto& trans : translation_default_str) {
		(void)export_language.SetValue("translation",
		                               translation_default_str[i].c_str(),
		                               translation_map[i].c_str());
		i++;
	}
	export_language.SaveFile(export_path.c_str());
}

} // namespace TRANSLATION

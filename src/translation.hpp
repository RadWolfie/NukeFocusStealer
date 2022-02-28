/* SPDX-License-Identifier: GPL-3.0-or-later */
// TODO: Make license downgrade to MIT or the like.

#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>


enum Trans {
#define ENCODE(e)         Trans_##e,
#define ENCODE_EX(e, str) Trans_##e,
#include "translation.def"
#undef ENCODE
#undef ENCODE_EX
	TransCount
};

extern std::unordered_map<int, std::string> translation_map;

namespace TRANSLATION {

void Initialize();

void UpdateLanguage(UI::locale_iso& lang, const std::filesystem::path& langs_path);

void ExportLanguage(const std::filesystem::path& export_path);

} // namespace TRANSLATION

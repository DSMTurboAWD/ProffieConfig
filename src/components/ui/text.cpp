#include "text.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/text.cpp
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <wx/settings.h>

wxFont pcui::text::detail::StyleData::makeFont() const {
    if (const auto *ptr{std::get_if<wxFont>(this)}) {
        return *ptr;
    } 

    // So, at least on macOS, wxSYS_DEFAULT_GUI_FONT returns a font with proper
    // spacing but incorrect size, and wxFont returns a font with proper size
    // but incorrect spacing.
    //
    // I don't know why, and I can't seem to figure out how to make things
    // more elegant. Probably some platform-specific code here to try and
    // pierce wxWidgets would be ideal, but that's a future issue.
    auto sysFont{[] {
        auto guiFont{wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)};
        auto defaultFont{wxFont{wxFontInfo{}}};
        guiFont.SetFractionalPointSize(defaultFont.GetFractionalPointSize());
        return guiFont;
    }()};

    switch (std::get<text::Style>(*this)) {
        using enum text::Style;
        case Normal:
            return sysFont;
        case Title:
            sysFont.Scale(1.5);
            return sysFont;
        case Style::Header:
            return sysFont.Scale(1.2);
        }

    assert(0);
    __builtin_unreachable();
}


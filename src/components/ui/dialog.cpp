#include "dialog.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/dialog.cpp
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

#ifdef _WIN32
#include <dwmapi.h>
#include "app/app.hpp"
#endif

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/window.h>

using namespace pcui;

Dialog::Dialog(
    wxWindow *parent,
    wxWindowID winID,
    const wxString& title,
    long style
) {
#   ifdef __WXMSW__
    SetDoubleBuffered(true);
#   endif

    Create(
        parent,
        winID,
        title,
        wxDefaultPosition,
        wxDefaultSize,
        style,
        "pcui::Dialog"
    );

#	ifdef _WIN32
    SetIcon(wxICON(ApplicationIcon));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));

    Bind(wxEVT_CREATE, [this](wxWindowCreateEvent&) {
        DWORD useDarkMode{app::darkMode()};
        DwmSetWindowAttribute(
#           ifdef __WXGTK__
            GTKGetWin32Handle(),
#           else
            GetHWND(),
#           endif
            DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
            &useDarkMode,
            sizeof(DWORD)
        );
    });
#	endif
}

Dialog::~Dialog() = default;

void Dialog::Fit() {
    // See Frame
    wxDialog::Fit();
    SetMinSize(GetSize());
}


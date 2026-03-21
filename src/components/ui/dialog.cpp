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

    Bind(wxEVT_CHAR_HOOK, [this](wxKeyEvent& evt) {
        if (evt.GetKeyCode() == WXK_ESCAPE) EndModal(wxID_CANCEL);
        evt.Skip();
    });

#	ifdef _WIN32
    SetIcon(wxICON(ApplicationIcon));

    Bind(wxEVT_CREATE, [this](wxWindowCreateEvent&) {
#       ifdef __WXGTK__
        auto *hwnd{GTKGetWin32Handle()};
#       else
        auto *hwnd{GetHWND()};
#       endif

        DWORD useDarkMode{app::darkMode()};
        DwmSetWindowAttribute(
            hwnd,
            DWMWA_USE_IMMERSIVE_DARK_MODE,
            &useDarkMode,
            sizeof useDarkMode
        );

        auto backdrop{DWMSBT_TRANSIENTWINDOW};
        DwmSetWindowAttribute(
            hwnd,
            DWMWA_SYSTEMBACKDROP_TYPE,
            &backdrop,
            sizeof backdrop
        );
    });
#	endif
}

Dialog::~Dialog() = default;

void Dialog::Fit() {
    // See Frame
    SetMinSize({0, 0});
    wxDialog::Fit();
    SetMinSize(GetSize());
}


#include "frame.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2024-2026 Ryan Ogurek
 *
 * components/ui/frame.cpp
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
#include <windows.h>

#include "app/app.hpp"
#endif

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/window.h>

using namespace pcui;

Frame::Frame(
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
        "pcui::Frame"
    );

#	ifdef _WIN32
#   ifdef __WXGTK__
    auto *hwnd{GTKGetWin32Handle()};
#   else
    auto *hwnd{GetHWND()};
#   endif

    SetIcon(wxICON(ApplicationIcon));

    auto exStyle{GetWindowLongA(hwnd, GWL_EXSTYLE)};
    SetWindowLongA(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

    Bind(wxEVT_CREATE, [this, hwnd](wxWindowCreateEvent&) {
        DWORD useDarkMode{app::darkMode()};
        DwmSetWindowAttribute(
            hwnd,
            DWMWA_USE_IMMERSIVE_DARK_MODE,
            &useDarkMode,
            sizeof useDarkMode
        );

        auto backdrop{DWMSBT_MAINWINDOW};
        DwmSetWindowAttribute(
            hwnd,
            DWMWA_SYSTEMBACKDROP_TYPE,
            &backdrop,
            sizeof backdrop
        );
    });
#	endif
}

Frame::~Frame() {
    if (mReference and *mReference) (*mReference) = nullptr;
}

void Frame::setReference(Frame** ref) {
    mReference = ref;
}

void Frame::Fit() {
    // IMO it's silly that the usual fit doesn't set min size. Fit is supposed
    // to set it to fit around the children (i.e. at min size), so not making
    // it so the sizing reflects that is a little odd. Probably historical.
    SetMinSize({0, 0});
    wxFrame::Fit();
    SetMinSize(GetSize());
}

void Frame::appendDefaultMenuItems(wxMenuBar *menuBar) {
#   ifdef __WXOSX__
    menuBar->Append(new wxMenu, _("&Window"));
    menuBar->Append(new wxMenu, _("&Help"));
#   endif
}


#include "helpers.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/priv/helpers.cpp
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
#include "log/context.hpp"
#include "log/logger.hpp"
#include "ui/frame.hpp"
#endif

void pcui::priv::apply(const detail::ChildBase& desc, wxSizerItem *item) {
    // wxSizerItem only calls the virtual func for a window, not a sizer,
    // So I have to do this check manually here in addition to the item call.
    if (item->IsSizer()) {
        item->GetSizer()->SetMinSize(desc.minSize_);
    } else {
        // Although in most cases the window min size will be handled by
        // WinBase, there may be some that aren't, so set it here anyways.
        // Worst case it's redundant.
        item->SetMinSize(desc.minSize_);
    }

    item->SetProportion(desc.proportion_);
    item->SetBorder(desc.border_.size_);
    item->SetFlag(
        desc.border_.dirs_ | (desc.expand_ ? wxEXPAND : 0) | desc.align_
    );
}


void pcui::priv::tlwPreCreate(wxTopLevelWindow *win) {
#   ifdef __WXMSW__
    win->SetDoubleBuffered(true);
#   endif
}

void pcui::priv::tlwPostCreate(wxTopLevelWindow *win) {
#   ifdef _WIN32
    win->SetIcon(wxICON(ApplicationIcon));

#   ifdef __WXGTK__
    auto *hwnd{win->GTKGetWin32Handle()};
#   else
    auto *hwnd{win->GetHWND()};
#   endif

    auto exStyle{GetWindowLongA(hwnd, GWL_EXSTYLE)};
    SetWindowLongA(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
#   endif
}

void pcui::priv::tlwBindOnCreate(wxTopLevelWindow *win) {
    win->Bind(wxEVT_CREATE, [win](wxWindowCreateEvent& evt) {
#       ifdef _WIN32
#       ifdef __WXGTK__
        auto *hwnd{win->GTKGetWin32Handle()};
#       else
        auto *hwnd{win->GetHWND()};
#       endif

        HRESULT res{};
        auto& logger{logging::Context::getGlobal().createLogger("TLW Create Event")};

        BOOL useDarkMode{app::darkMode()};
        res = DwmSetWindowAttribute(
            hwnd,
            DWMWA_USE_IMMERSIVE_DARK_MODE,
            &useDarkMode,
            sizeof useDarkMode
        );
        if (res != S_OK) {
            logger.warn("Immersive dark mode setup failed: " + std::to_string(res));
        }

        auto backdrop{dynamic_cast<pcui::Frame *>(win)
            ? DWMSBT_MAINWINDOW
            : DWMSBT_TRANSIENTWINDOW
        };
        res = DwmSetWindowAttribute(
            hwnd,
            DWMWA_SYSTEMBACKDROP_TYPE,
            &backdrop,
            sizeof backdrop
        );
        if (res != S_OK) {
            logger.warn("Backdrop setup failed: " + std::to_string(res));
        }

        evt.Skip();
#       endif
    });
}


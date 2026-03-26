#include "winbase.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/priv/winbase.cpp
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

#include <wx/event.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/toplevel.h>
#include <wx/window.h>

void pcui::priv::windowPostCreation(
    const detail::Scaffold& scaffold,
    const detail::ChildWindowBase& desc,
    wxWindow *win
) {
    win->SetToolTip(desc.tooltip_);

    if (scaffold.scrolled_) {
        const auto onWheel{[scrolled=scaffold.scrolled_](wxMouseEvent& evt) {
            scrolled->HandleOnMouseWheel(evt);

        }};
        win->Bind(wxEVT_MOUSEWHEEL, onWheel);
    }
}


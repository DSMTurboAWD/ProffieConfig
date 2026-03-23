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

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/window.h>

#include "ui/priv/helpers.hpp"

using namespace pcui;

Dialog::Dialog(
    wxWindow *parent,
    wxWindowID winID,
    std::variant<
        wxString,
        pcui::RefWrap<const data::String>
    > title,
    long style
) {
    priv::tlwBindOnCreate(this);
    priv::tlwPreCreate(this);

    Create(
        parent,
        winID,
        wxEmptyString,
        wxDefaultPosition,
        wxDefaultSize,
        style,
        "pcui::Dialog"
    );

    if (const auto *ptr{std::get_if<1>(&title)}) {
        data::String::ROContext ctxt{*ptr};
        SetTitle(ctxt.val());
        attach(*ptr);
    } else SetTitle(std::get<0>(title));

    Bind(wxEVT_CHAR_HOOK, [this](wxKeyEvent& evt) {
        if (evt.GetKeyCode() == WXK_ESCAPE) EndModal(wxID_CANCEL);
        evt.Skip();
    });

    priv::tlwPostCreate(this);
}

Dialog::~Dialog() = default;

void Dialog::Fit() {
    // See Frame
    SetMinSize({0, 0});
    wxDialog::Fit();
    SetMinSize(GetSize());
}

void Dialog::onChange() {
    SetTitle(context<data::String>().val());
}


#pragma once
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/dialog.hpp
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

#include <wx/dialog.h>

#include "data/string.hpp"
#include "ui/types.hpp"

#include "ui_export.h"

namespace pcui {

class UI_EXPORT Dialog : public wxDialog, data::String::Receiver {
public:
    Dialog(
        wxWindow* parent,
        wxWindowID winID,
        std::variant<
            wxString,
            pcui::RefWrap<const data::String>
        > title,
        long style = wxDEFAULT_DIALOG_STYLE
    );
    ~Dialog() override;

    void Fit() override;

private:
    void onChange() override;
};

} // namespace pcui


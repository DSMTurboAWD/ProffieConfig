#include "text.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/controls/text.cpp
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

#include <wx/font.h>
#include <wx/textctrl.h>

#include "ui/priv/helpers.hpp"
#include "ui/priv/winbase.hpp"

using namespace pcui;

namespace {

struct Control : priv::WinBase<wxTextCtrl, data::String::Receiver> {
    Control(wxWindow *parent, const Text& desc) {
        long style{0};

        bool handleEnter{false};
        if (const auto *ptr{std::get_if<Text::SingleLine>(&desc.mode_)}) {
            if (ptr->insertNewline_) {
                handleEnter = true;
                style |= wxTE_PROCESS_ENTER;
            }
        } else {
            const auto& mode{std::get<Text::MultiLine>(desc.mode_)};
            style |= wxTE_PROCESS_TAB;
        }

        Create(
            parent,
            wxID_ANY,
            data::String::Context{desc.data_}.val(),
            wxDefaultPosition,
            wxDefaultSize,
            style
        );

        postCreation(desc.win_);
        SetFont(desc.font_);

#       ifdef __WXMAC__
        if (desc.font_.GetFamily() == wxFONTFAMILY_TELETYPE) {
            OSXDisableAllSmartSubstitutions();
        }
#       endif

        attach(desc.data_);
        Bind(wxEVT_TEXT, &Control::onText, this);

        if (handleEnter) {
            Bind(wxEVT_TEXT, &Control::onEnter, this);
        }
    }

    ~Control() override {
        Unbind(wxEVT_TEXT, &Control::onText, this);
        Unbind(wxEVT_TEXT_ENTER, &Control::onEnter, this);
        detach();
    }

    void onText(wxCommandEvent&) {
        auto& str{const_cast<data::String&>(model<data::String>())};
        auto res{str.processUIAction(std::make_unique<data::String::ChangeAction>(
            /*
             * TODO: Play with how to get accurate insertion point, since
             * GetInsertionPoint is not accurate on Windows. Combining with
             * PositionToXY and some math might be viable?
             *
             * Also need to consider the other direction (onMove event)
             */
            GetValue().ToStdString(), 0 
        ))};
        if (not res) [this, str=context<data::String>()]() {
            SetValue(str.val());
            // SetInsertionPoint
        }();
    }

    void onEnter(wxCommandEvent&) {
        WriteText("\\n");
    }
    
    void onChange() override {
        const auto val{context<data::String>().val()};
        CallAfter([this, val] {
            SetValue(val);
            // SetInsertionPoint
        });
    }

    void onMove() override {
        CallAfter([this] {
            // SetInsertionPoint
        });
    }
};

} // namespace

std::unique_ptr<detail::Descriptor> Text::operator()() {
    return std::make_unique<Text::Desc>(std::move(*this));
}

Text::Desc::Desc(Text&& data) :
    Text{std::move(data)} {}

wxSizerItem *Text::Desc::build(const detail::Scaffold& scaffold) const {
    auto *chk{new Control(scaffold.childParent_, *this)};
    auto *item{new wxSizerItem(chk)};
    priv::apply(base_, item);
    return item;
}


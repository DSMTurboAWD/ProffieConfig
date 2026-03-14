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
#include <wx/sizer.h>

#include "ui/detail/scaffold.hpp"
#include "ui/priv/helpers.hpp"
#include "ui/priv/winbase.hpp"

using namespace pcui;

namespace {

struct Control : priv::WinBase<wxTextCtrl, data::String::Receiver> {
    Control(const detail::Scaffold& scaffold, const Text& desc) {
        wxString initial;
        long style{0};
        bool handleEnter{false};

        if (desc.readOnly_) style |= wxTE_READONLY;
        if (desc.autoLink_) style |= wxTE_AUTO_URL;

        if (const auto *ptr{std::get_if<Text::SingleLine>(&desc.mode_)}) {
            if (ptr->insertNewline_) {
                handleEnter = true;
                style |= wxTE_PROCESS_ENTER;
            }
        } else {
            const auto& mode{std::get<Text::MultiLine>(desc.mode_)};
            style |= wxTE_PROCESS_TAB;
            style |= wxTE_MULTILINE;
            switch (mode.wrap_) {
                using enum Text::MultiLine::Wrap;
                case None:
                    style |= wxTE_DONTWRAP;
                    break;
                case Character:
                    style |= wxTE_CHARWRAP;
                    break;
                case Word:
                    style |= wxTE_WORDWRAP;
                    break;
                case Best:
                    style |= wxTE_BESTWRAP;
                    break;
            }
        }

        if (desc.data_.index() == 0) {
            style |= wxTE_READONLY;
            initial = std::get<0>(desc.data_);
        } else if (const auto *ptr{std::get_if<1>(&desc.data_)}) {
            initial = data::String::Context{*ptr}.val();
        }

        Create(
            scaffold.childParent_,
            wxID_ANY,
            initial,
            wxDefaultPosition,
            wxDefaultSize,
            style
        );

        postCreation(scaffold, desc.win_);
        SetOwnFont(desc.style_.makeFont());

#       ifdef __WXMAC__
        if (GetFont().GetFamily() == wxFONTFAMILY_TELETYPE) {
            OSXDisableAllSmartSubstitutions();
        }
#       endif

        if (const auto *ptr{std::get_if<1>(&desc.data_)}) {
            attach(*ptr);
        }

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
    auto *ctrl{new Control(scaffold, *this)};
    auto *item{new wxSizerItem(ctrl)};

    priv::apply(win_.base_, item);
    if (const auto *ptr{std::get_if<Text::MultiLine>(&mode_)}) {
        if (not ptr->scroll_.vertical_) {
            const auto lineHeight{ctrl->GetTextExtent('M').y};
            
            ctrl->SetMinSize({
                ctrl->GetMinWidth(),
                static_cast<int32>(
                    lineHeight * (ctrl->GetNumberOfLines() + 0.5)
                )
            });
        }
    }

    return item;
}


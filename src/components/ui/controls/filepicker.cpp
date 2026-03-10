#include "filepicker.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/controls/filepicker.cpp
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

#include <wx/filepicker.h>

#include "ui/priv/helpers.hpp"
#include "ui/priv/winbase.hpp"

using namespace pcui;

namespace {

struct Control : priv::WinBase<wxFilePickerCtrl, data::String::Receiver> {
    Control(wxWindow *parent, const FilePicker &desc) {
        // Use textctrl default is platform-dependent, check if it exists in
        // the defaults instead of unilaterally setting it.
        long style{wxFLP_DEFAULT_STYLE & wxFLP_USE_TEXTCTRL};

        if (const auto *ptr{std::get_if<FilePicker::Open>(&desc.mode_)}) {
            style |= wxFLP_OPEN;
            if (ptr->mustExist_) style |= wxFLP_FILE_MUST_EXIST;
        } else {
            const auto& mode{std::get<FilePicker::Save>(desc.mode_)};
            style |= wxFLP_SAVE;
            if (mode.confirmOverwrite_) style |= wxFLP_OVERWRITE_PROMPT;
        }

        Create(
            parent,
            wxID_ANY,
            data::String::Context{desc.data_}.val(),
            desc.message_,
            desc.wildcard_,
            wxDefaultPosition,
            wxDefaultSize,
            style
        );

        postCreation(desc.win_);

        attach(desc.data_);
        Bind(wxEVT_FILEPICKER_CHANGED, &Control::onPick, this);
    }

    ~Control() override {
        Unbind(wxEVT_FILEPICKER_CHANGED, &Control::onPick, this);
        detach();
    }

    void onPick(wxCommandEvent&) {
        auto& str{const_cast<data::String&>(model<data::String>())};
        auto path{GetPath().ToStdString()};
        const auto len{path.length()};

        auto res{str.processUIAction(std::make_unique<data::String::ChangeAction>(
            std::move(path), len
        ))};

        if (not res) [this, str=context<data::String>()]() {
            SetPath(str.val());
        }();
    }

    void onChange() override {
        const auto val{context<data::String>().val()};
        CallAfter([this, val] {
            SetPath(val);
        });
    }
};

} // namespace

std::unique_ptr<detail::Descriptor> FilePicker::operator()() {
    return std::make_unique<FilePicker::Desc>(std::move(*this));
}

FilePicker::Desc::Desc(FilePicker&& data) :
    FilePicker{std::move(data)} {}

wxSizerItem *FilePicker::Desc::build(const detail::Scaffold& scaffold) const {
    auto *chk{new Control(scaffold.childParent_, *this)};
    auto *item{new wxSizerItem(chk)};
    priv::apply(base_, item);
    return item;
}


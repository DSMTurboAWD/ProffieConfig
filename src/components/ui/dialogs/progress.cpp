#include "progress.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/dialogs/progress.cpp
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

#include <future>

#include <wx/thread.h>

#include "data/logic/adapter.hpp"
#include "data/logic/operators.hpp"
#include "data/string.hpp"
#include "ui/build.hpp"
#include "ui/controls/button.hpp"
#include "ui/dialog.hpp"
#include "ui/helpers/dialog_buttons.hpp"
#include "ui/layout/spacer.hpp"
#include "ui/layout/stack.hpp"
#include "ui/static/label.hpp"

using namespace pcui;

ProgressDialog::ProgressDialog(
    wxWindow *parent, const wxString& title, bool mayCancel
) : Dialog(parent, wxID_ANY, title) {
    pcui::build(this, ui(mayCancel));

    ShowWindowModal();
}

ProgressDialog::~ProgressDialog() {
    // The dialog should've been 
    assert(not IsModal());

    pcui::teardown(this);
}

void ProgressDialog::set(uint32 val, const wxString& message) {
    if (not message.empty()) {
        data::String::Context{mMessage}.change(message.ToStdString());
    }

    mData.set(val);
}

void ProgressDialog::range(uint32 val) {
    mData.range(val);
}

void ProgressDialog::pulse(const wxString& message) {
    if (not message.empty()) {
        data::String::Context{mMessage}.change(message.ToStdString());
    }

    mData.pulse();
}

void ProgressDialog::finish(bool modalWait, const wxString& message) {
    data::String::Context{mMessage}.change(message.ToStdString());

    assert(not wxIsMainThread());

    std::promise<void> promise;
    
    CallAfter([this, modalWait, &promise] {
        if (modalWait) ShowModal();
        Destroy();

        promise.set_value();
    });

    promise.get_future().wait();
}

bool ProgressDialog::cancelled() {
    return data::Bool::Context{mCancelled}.val();
}

DescriptorPtr ProgressDialog::ui(bool mayCancel) {
    return pcui::Stack{
      .base_={.border_={.size_=10, .dirs_=wxALL}},
      .children_={
        pcui::Label{
          .label_=mMessage,
        }(),
        pcui::Spacer{.size_=4}(),
        pcui::Progress{
          .win_={.base_={.expand_=true}},
          .data_=mData,
        }(),
        pcui::Spacer{.size_=8}(),
        pcui::DialogButtons{
          .ok_=pcui::Button{
            .win_={
              .show_=mData | Progress::Logic::Is_Done,
            },
            .label_=_("Ok"),
            .func_=[this] {
                Destroy();
            },
          }(),
          .cancel_=mayCancel ? pcui::Button{
            .win_={
              .enable_=not (mCancelled | data::logic::IsSet{}),
            },
            .label_=_("Cancel"),
            .func_=[this] {
                data::Bool::Context{mCancelled}.set(true);
            }
          }() : nullptr,
        }(),
      }
    }();
}


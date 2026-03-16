#include "choice.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/controls/choice.cpp
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

#include <wx/choice.h>

#include "ui/detail/scaffold.hpp"
#include "ui/priv/helpers.hpp"
#include "ui/priv/winbase.hpp"

using namespace pcui;

namespace {

struct Control : priv::WinBase<wxChoice, data::Choice::Receiver> {
    Control(const detail::Scaffold& scaffold, const Choice& desc) :
        unselected_{desc.unselected_},
        labeler_{desc.labeler_} {
        Create(
            scaffold.childParent_,
            wxID_ANY,
            wxDefaultPosition,
            wxDefaultSize
        );

        postCreation(scaffold, desc.win_);

        { data::Choice::Context ctxt{desc.data_};
            Set(generateChoices(ctxt.numChoices()));
            SetSelection(dataToControl(ctxt.choice()));
            attach(desc.data_);
        }

        Bind(wxEVT_CHOICE, &Control::onChoice, this);
    }

    ~Control() override {
        Unbind(wxEVT_CHOICE, &Control::onChoice, this);
        detach();
    }

    void onChoice(wxCommandEvent& evt) {
        auto& ch{const_cast<data::Choice&>(model<data::Choice>())};
        auto res{ch.processUIAction(std::make_unique<data::Choice::ChoiceAction>(
            controlToData(evt.GetInt())
        ))};
        if (not res) [this, ch=context<data::Choice>()]() {
            SetSelection(dataToControl(ch.choice()));
        }();
    }
    
    void onChoice() override {
        const auto choice{context<data::Choice>().choice()};
        CallAfter([this, choice] {
            SetSelection(dataToControl(choice));
        });
    }

    void onUpdate() override {
        const auto choices{generateChoices(
            context<data::Choice>().numChoices()
        )};
        CallAfter([this, choices] {
            Set(choices);
        });
    }

    int32 controlToData(int32 choice) const {
        return unselected_.empty() ? choice : choice - 1;
    }

    int32 dataToControl(int32 choice) const {
        return unselected_.empty() ? choice : choice + 1;
    }

    std::vector<wxString> generateChoices(uint32 num) {
        std::vector<wxString> choices;

        if (not unselected_.empty()) {
            choices.push_back(unselected_);
        }

        rcvrs_.clear();

        for (uint32 idx{0}; idx < num; ++idx) {
            if (not labeler_) {
                choices.emplace_back("LABEL???");
                continue;
            }

            auto res{labeler_(idx)};
            if (auto *ptr{std::get_if<wxString>(&res)}) {
                choices.emplace_back(std::move(*ptr));
                continue;
            }

            const auto& model{std::get<1>(res).get()};
            data::String::ROContext ctxt{model};
            choices.emplace_back(ctxt.val());
            rcvrs_.push_back(std::make_unique<LabelReceiver>(
                this, idx, model
            ));
        }

        return choices;
    }

    wxString unselected_;
    pcui::Choice::Labeler labeler_;

    struct LabelReceiver final : data::String::Receiver {
        LabelReceiver(Control *ctrl, uint32 idx, const data::String& model) :
            mCtrl{ctrl}, mIdx{idx} {
            attach(model);
        }

        ~LabelReceiver() override {
            detach();
        }

        void onChange() override {
            // Capture info by value, the receiver could die before the UI
            // updates occur.
            mCtrl->CallAfter([
                ctrl=mCtrl, idx=mIdx, val=context<data::String>().val()
            ] {
                ctrl->SetString(idx, val);
            });
        }

    private:
        Control *mCtrl;
        uint32 mIdx;
    };

    std::vector<std::unique_ptr<data::String::Receiver>> rcvrs_;
};

} // namespace

std::unique_ptr<detail::Descriptor> Choice::operator()() {
    return std::make_unique<Choice::Desc>(std::move(*this));
}

Choice::Desc::Desc(Choice&& data) :
    Choice{std::move(data)} {}

wxSizerItem *Choice::Desc::build(const detail::Scaffold& scaffold) const {
    auto *chk{new Control(scaffold, *this)};
    auto *item{new wxSizerItem(chk)};
    priv::apply(win_.base_, item);
    return item;
}


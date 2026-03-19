#include "button.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/controls/button.cpp
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

#include <wx/button.h>
#include <wx/gdicmn.h>

#include "ui/detail/scaffold.hpp"
#include "ui/priv/helpers.hpp"
#include "ui/priv/winbase.hpp"

using namespace pcui;

namespace {

struct Control : priv::WinBase<wxButton, data::String::Receiver> {
    Control(const detail::Scaffold& scaffold, const Button& desc) :
        func_{desc.func_} {

        const auto commonSetup{[&] {
            postCreation(scaffold, desc.win_);
            if (desc.default_) SetDefault();
        }};

        long style{0};
        if (desc.exactFit_) style |= wxBU_EXACTFIT;

        const data::Model *modelPtr{nullptr};

        switch (desc.style_) {
            using enum Button::Style;
            case Normal:
                style |= wxBORDER_DEFAULT;
                break;
            case Companion:
#       ifdef __WXOSX__
                style |= wxBORDER_SIMPLE;
#       else
                style |= wxBORDER_SIMPLE;
#       endif
                break;
        }

        Create(
            scaffold.childParent_,
            wxID_ANY,
            wxEmptyString,
            wxDefaultPosition,
            wxDefaultSize,
            style 
        );

        if (const auto *ptr{std::get_if<1>(&desc.label_)}) {
            const auto& [label, model]{*ptr};
            SetLabel(label);
            modelPtr = &model.get();
        } else if (const auto *ptr{std::get_if<2>(&desc.label_)}) {
            data::String::ROContext str{*ptr};
            SetLabel(str.val());
            modelPtr = &ptr->get();
        } else {
            SetLabel(std::get<0>(desc.label_));
        }

        if (desc.bitmap_.IsOk()) SetBitmap(desc.bitmap_);

        commonSetup();

        if (modelPtr) attach(*modelPtr);
        Bind(wxEVT_BUTTON, &Control::onPress, this);
    }

    ~Control() override {
        Unbind(wxEVT_BUTTON, &Control::onPress, this);
        detach();
    }

    void SetLabel(const wxString& str) override {
#       ifdef __WXGTK__
        // TODO: Does this look right?
        const auto exactFit{GetWindowStyle() & wxBU_EXACTFIT};
        wxButton::SetLabel(exactFit and not str.empty()
            ? ' ' + str + ' '
            : str
        );
#       else
        wxButton::SetLabel(str);
#       endif

        layoutAndFitFor(this);
    }

    void onPress(wxCommandEvent&) {
        if (const auto *ptr{maybeModel<data::String>()}) {
            data::String::ROContext ctxt{*ptr};
            if (ctxt.enabled() and func_) func_();
        } else if (func_) func_();
    }

    void onChange() override {
        GetWindowStyle();
        safeCall([this, str=context<data::String>().val()]() {
            SetLabel(str);
        });
    }

    const std::function<void()> func_;
};

} // namespace

std::unique_ptr<detail::Descriptor> Button::operator()() {
    return std::make_unique<Button::Desc>(std::move(*this));
}

Button::Desc::Desc(Button&& data) :
    Button{std::move(data)} {}

wxSizerItem *Button::Desc::build(const detail::Scaffold& scaffold) const {
    auto *button{new Control(scaffold, *this)};
    auto *item{new wxSizerItem(button)};
    priv::apply(win_.base_, item);
    return item;
}


#include "scrolled.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/layout/scrolled.cpp
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

#include <wx/scrolwin.h>

#include "ui/priv/helpers.hpp"
#include "ui/priv/winbase.hpp"

using namespace pcui;

namespace {

struct Window : priv::WinBase<wxScrolledWindow, data::Generic::Receiver> {
    Window(const detail::Scaffold& scaffold, const Scrolled& desc) {
        Create(scaffold.childParent_, wxID_ANY);

        postCreation(scaffold, desc.win_);
        SetScrollRate(desc.scrollRate_.x_, desc.scrollRate_.y_);

        if (desc.data_) attach(*desc.data_);
    }

    ~Window() override {
        detach();
    }

    void updateSizes() override {
        WinBase::updateSizes();
        
        if (GetSizer() == nullptr) return;

        // Do not reset min size, it was recalc'd in WinBase::updateSizes()
        auto bestSize{GetBestSize()};

        int scrollX{};
        int scrollY{};
        GetScrollPixelsPerUnit(&scrollX, &scrollY);

        const auto virtSize{GetSizer()->CalcMin()};

        if (scrollX == -1) {
            bestSize.SetWidth(std::max(
                bestSize.GetWidth(),
                virtSize.GetWidth()
            ));
        }

        if (scrollY == -1) {
            bestSize.SetHeight(std::max(
                bestSize.GetHeight(),
                virtSize.GetHeight()
            ));
        }

        SetMinSize(bestSize);
    }
};

} // namespace

std::unique_ptr<detail::Descriptor> Scrolled::operator()() {
    return std::make_unique<Scrolled::Desc>(std::move(*this));
}

Scrolled::Desc::Desc(Scrolled&& data) :
    Scrolled{std::move(data)} {}

wxSizerItem *Scrolled::Desc::build(const detail::Scaffold& scaffold) const {
    auto *win{new Window(scaffold, *this)};

    auto childScaffold{scaffold};
    childScaffold.childParent_ = win;
    childScaffold.scrolled_ = win;

    auto *sizer{new wxBoxSizer(wxVERTICAL)};
    sizer->Add(child_->build(childScaffold));
    win->SetSizerAndFit(sizer);

    auto *item{new wxSizerItem(win)};
    priv::apply(win_.base_, item);

    return item;
}


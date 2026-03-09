#include "panel.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/layout/panel.cpp
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

#include <wx/panel.h>

#include "ui/build.hpp"
#include "ui/priv/helpers.hpp"
#include "ui/priv/winbase.hpp"

using namespace pcui;

namespace {

struct Layout : priv::WinBase<wxPanel, data::Generic::Receiver> {
    Layout(wxWindow *parent, const Panel& desc) {
        Create(parent);

        postCreation(desc.win_);

        if (desc.data_) attach(*desc.data_);
    }

    ~Layout() override {
        detach();
    }
};

} // namespace

std::unique_ptr<detail::Descriptor> Panel::operator()() {
    return std::make_unique<Panel::Desc>(std::move(*this));
}

Panel::Desc::Desc(Panel&& data) :
    Panel{std::move(data)} {}

wxSizerItem *Panel::Desc::build(const detail::Scaffold& scaffold) const {
    auto *panel{new Layout(scaffold.childParent_, *this)};

    pcui::build(panel, child_);

    auto *item{new wxSizerItem(panel)};
    priv::apply(base_, item);

    return item;
}


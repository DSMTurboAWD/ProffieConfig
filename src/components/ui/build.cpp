#include "build.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/build.cpp
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

#include "ui/frame.hpp"

void pcui::build(wxWindow *win, const DescriptorPtr& desc) {
    teardown(win);

    auto *parent{win};
    if (dynamic_cast<pcui::Frame *>(parent)) {
        parent = new wxPanel(win);
    }

    if (not desc) return;

    // In many cases this means there's a kind of unnecessary sizer in a sizer,
    // but this is the most straightforward way to allow base (essentially just
    // sizer) properties to work at the top level.
    auto *sizer{new wxBoxSizer(wxVERTICAL)};

    detail::Scaffold scaffold{
        .childParent_=parent,
        .sizer_=sizer,
    };

    auto *item{desc->build(scaffold)};
    sizer->Add(item);

    // The sizer SetSizerAndFit, contrary to its name, does not SetSizer() and
    // Fit(). Instead, it calls some special functions to do something similar.
    //
    // Since this setup places more logic into Fit() and overrides it to behave
    // in the more expected way (such that a naive SetSizer() and Fit()) would
    // work as the actual SetSizerAndFit(), doing calcs and setting min), it
    // must actually be called directly.
    //
    // TL;DR SetSizerAndFit() does not call Fit().
    parent->SetSizer(sizer);
    parent->Fit();

    // TODO: I'm not sure why (I thought at first it was layoutAndFitFor()'s
    // deferred routines, but none of that should be firing, much less in an
    // impactful way), but without a yield here to sync things up before a
    // window is shown, there's some jitter as laying out occurs.
    wxYield();
}

void pcui::teardown(wxWindow *parent) {
    assert(parent);

    // First, delete all children of the active sizer, and the sizer itself.
    // That should clear out most windows in most cases.
    if (parent->GetSizer()) {
        parent->GetSizer()->DeleteWindows();
        parent->SetSizer(nullptr, true);
    }

    // Then, check for any other children. They cannot be unconditionally
    // deleted because there's things like toolbars (and probably other things)
    // that need to be considered.
    //
    // There might be a cleaner and/or more efficient way to do this, but we
    // can't just iterate over GetChildren() directly, because the iterators
    // will be invalidated as child Destroy() is called.
    auto iter{parent->GetChildren().begin()};
    while (iter != parent->GetChildren().end()) {
        if (not parent->IsClientAreaChild(*iter)) {
            ++iter;
            continue;
        }

        (*iter)->Destroy();
        iter = parent->GetChildren().begin();
    }
}


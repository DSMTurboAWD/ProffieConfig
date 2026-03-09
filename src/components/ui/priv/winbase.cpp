#include "winbase.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/priv/winbase.cpp
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

#include <wx/sizer.h>
#include <wx/window.h>

namespace {

// All this stuff happens on the main thread, no locking required.
bool updateRequested{false};
std::set<wxWindow *> updateList;

void performUpdate();

} // namespace

/**
 * This does not actually do any layout or fitting itself.
 *
 * It finds the top-level window associated with the window that caused the
 * layout to be required, adds it into the `updateList`, and queues
 * performUpdate(), if it hasn't been queued already.
 *
 * This is for the case where multiple UI updates would come in such a way that
 * one child is shown, *then* another is hidden, and where they may occupy the
 * same or similar space.
 *
 * If the layout and fitting occurred immediately, then the window may
 * unnecessarily grow, which is undesirable.
 */
void pcui::priv::layoutAndFitFor(wxWindow *win) {
    auto *top{win};

    std::vector<wxWindow *> hierarchy{top};

    while (not false) {
        auto *next{top->GetParent()};
        if (not next) break;

        top = next;
        hierarchy.push_back(top);

        if (next->IsTopLevel()) break;
    }

    updateList.insert(top);

    if (not updateRequested) {
        win->CallAfter(&performUpdate);
        updateRequested = true;
    }
}

namespace {

void performUpdate() {
    for (auto *win : updateList) {
        auto oldSize{win->GetSize()};

        win->Fit();
        auto fitSize{win->GetSize()};

        fitSize.IncTo(oldSize);
        win->SetSize(fitSize);

        win->Layout();
    }

    updateList.clear();
    updateRequested = false;
}

} // namespace


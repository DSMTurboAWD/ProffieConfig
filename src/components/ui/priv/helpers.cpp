#include "helpers.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/priv/helpers.cpp
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

namespace {

// All this stuff happens on the main thread, no locking required.
bool updateRequested{false};
std::vector<std::set<wxWindow *>> updateList;
std::unordered_map<wxWindow *, bool> showList;

void performUpdate();

} // namespace

void pcui::priv::apply(const detail::ChildBase& desc, wxSizerItem *item) {
    // wxSizerItem only calls the virtual func for a window, not a sizer,
    // So I have to do this check manually here in addition to the item call.
    if (item->IsSizer()) {
        item->GetSizer()->SetMinSize(desc.minSize_);
    } else {
        // Although in most cases the window min size will be handled by
        // WinBase, there may be some that aren't, so set it here anyways.
        // Worst case it's redundant.
        item->SetMinSize(desc.minSize_);
    }

    item->SetProportion(desc.proportion_);
    item->SetBorder(desc.border_.size_);
    item->SetFlag(
        desc.border_.dirs_ | (desc.expand_ ? wxEXPAND : 0) | desc.align_
    );
}

void pcui::priv::queueShow(wxWindow *win, bool show) {
    assert(wxIsMainThread());;

    showList[win] = show;
}

/*
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
    assert(wxIsMainThread());;

    auto *top{win};

    std::vector<wxWindow *> hierarchy{top};

    while (not false) {
        auto *next{top->GetParent()};
        if (not next) break;

        top = next;
        hierarchy.push_back(top);

        if (next->IsTopLevel()) break;
    }

    updateList.resize(std::max(updateList.size(), hierarchy.size()));
    for (size idx{0}; idx < hierarchy.size(); ++idx) {
        // Insert in reverse so that updateList[0] is always a TLW, and the
        // greatest index is the lowest-level window, so that actual processing
        // can clearly process from lowest to highest.
        updateList[idx].insert(hierarchy[hierarchy.size() - idx - 1]);
    }

    if (not updateRequested) {
        win->CallAfter(&performUpdate);
        updateRequested = true;
    }
}

namespace {

/**
 * On every window, starting from the lowest in the hierarchy, Fit() is called.
 * Top level windows have some additional handling to not shrink.
 *
 * Fit() is used on every window as, while wxWidgets normally only has it as a
 * size set (no min size), it's the seemingly most semantically clear place for
 * windows to implement min size recalculation (Layout() is not suitable as
 * it's called during user resize, and many other times. Fit() is more
 * programmatic in nature).
 */
void performUpdate() {
    // If an update is requested, this should never be empty.
    assert(not updateList.empty());

    for (auto [win, show] : showList) {
        win->Show(show);
    }
    showList.clear();

    for (auto iter{updateList.rbegin()};; ++iter) {
        if (std::next(iter) == updateList.rend()) {
            // Update top levels
            for (auto *win : *iter) {
                auto oldSize{win->GetSize()};

                win->Fit();

                if (win->GetWindowStyle() & wxRESIZE_BORDER) {
                    auto fitSize{win->GetSize()};
                    fitSize.IncTo(oldSize);
                    win->SetSize(fitSize);
                }

                win->Layout();
            }

            break;
        }

        // Update lower levels
        for (auto *win : *iter) {
            win->Fit();
            win->Layout();
        }
    }

    updateList.clear();
    updateRequested = false;
}

} // namespace


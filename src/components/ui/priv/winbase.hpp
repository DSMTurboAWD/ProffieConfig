#pragma once
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/priv/winbase.hpp
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

#include <wx/thread.h>

#include "ui/detail/general.hpp"
#include "data/logic/logic.hpp"

class wxWindow;

namespace pcui::priv {

void layoutAndFitFor(wxWindow *);

template <typename Base, typename Receiver>
struct WinBase : Base, Receiver {
    WinBase() = default;

    /**
     * Post window creation, prior to receiver attachment.
     */
    void postCreation(const detail::ChildWindowBase& desc) {
        Base::SetToolTip(desc.tooltip_);
        Base::SetMaxSize(desc.maxSize_);

        mShow = desc.show_;
        if (mShow) mShowReceiver = std::make_unique<ShowReceiver>(this);
    }

    void onAttach() override {
        safeCall([this]() {
            Base::Enable(Receiver::context().enabled());
        });
    }

    void onDetach() override {
        safeCall([this]() {
            Base::Disable();
        });
    }

    void onEnabled() override {
        safeCall([this]() {
            Base::Enable(Receiver::context().enabled());
        });
    }

    void onFocus() override {
        safeCall([this]() {
            Base::SetFocus();
        });
    }

    void safeCall(std::function<void()>&& func) {
        if (wxIsMainThread()) func();
        else Base::CallAfter(std::move(func));
    }

private:
    struct ShowReceiver : data::logic::Receiver {
        ShowReceiver(WinBase *ptr) : winbase_{ptr} {
            attach(*winbase_->mShow);
        }

        void onChange(bool val) override {
            winbase_->safeCall([this, val]() {
                winbase_->Base::Show(val);
                layoutAndFitFor(winbase_);
            });
        }

        WinBase *winbase_;
    };
    std::unique_ptr<ShowReceiver> mShowReceiver;
    data::logic::Holder mShow;
};

} // namespace pcui::priv


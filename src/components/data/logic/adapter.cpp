#include "adapter.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/data/logic/adapter.cpp
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

auto data::logic::operator|(const data::Bool& bl, IsSet) -> Element {
    struct Adapter : detail::Base, data::Bool::Receiver {
        Adapter(const data::Bool& bl) : bl_{bl} {}
        ~Adapter() override { detach(); }

        bool doActivate() override {
            attach(bl_);
            return context<Bool>().val();
        }

        void onSet() override {
            std::lock_guard scopeLock{*pLock};
            onChange(context<Bool>().val());
        }

        const data::Bool& bl_;
    };

    return std::make_unique<Adapter>(bl);
}

auto data::logic::operator|(
    const data::Choice& choice, HasSelection sels
) -> Element {
    struct Adapter : detail::Base, data::Choice::Receiver {
        Adapter(const data::Choice& choice, HasSelection sels) :
            choice_{choice}, sels_{std::move(sels)} {}
        ~Adapter() override { detach(); }

        bool doActivate() override {
            attach(choice_);
            return isTrue();
        }

        void onChoice() override {
            std::lock_guard scopeLock{*pLock};
            onChange(isTrue());
        }

        bool isTrue() {
            return sels_.contains(context<Choice>().choice());
        }

        const data::Choice& choice_;
        HasSelection sels_;
    };

    return std::make_unique<Adapter>(choice, std::move(sels));
}

auto data::logic::operator|(
    const data::String& choice, IsEmpty
) -> Element {
    struct Adapter : detail::Base, data::String::Receiver {
        Adapter(const data::String& str) :
            str_{str} {}
        ~Adapter() override { detach(); }

        bool doActivate() override {
            attach(str_);
            return isTrue();
        }

        void onChange() override {
            std::lock_guard scopeLock{*pLock};
            Base::onChange(isTrue());
        }

        bool isTrue() {
            return context<String>().val().empty();
        }

        const String& str_;
    };

    return std::make_unique<Adapter>(choice);
}


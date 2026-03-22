#pragma once
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/detail/dynamic_list.hpp
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

#include <vector>

#include "ui/types.hpp"

#include "ui_export.h"

namespace pcui::detail {

struct UI_EXPORT DynamicList : std::vector<DescriptorPtr> {
    DynamicList(const DynamicList&) = default;
    DynamicList(DynamicList&&) = default;
    DynamicList& operator=(const DynamicList&) = default;
    DynamicList& operator=(DynamicList&&) = default;

    template <typename ...Args>
    DynamicList(Args&&... args) {
        // This reserve is inaccurate
        reserve(sizeof...(args));
        (..., add(std::forward<Args>(args)));
    }

    void add(DynamicList&& v) {
        add(static_cast<vector&&>(std::move(v)));
    }

    void add(vector&& v) {
        // move(v) is silly, but it shuts up the linter and probably doesn't
        // matter.
        for (auto& elem : std::move(v)) push_back(std::move(elem));
    }

    void add(DescriptorPtr&& d) {
        push_back(std::move(d));
    }
};

} // namespace pcui::detail


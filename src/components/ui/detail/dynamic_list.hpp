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
#include <memory>

#include "ui_export.h"

namespace pcui::detail {

template <typename T>
struct UI_EXPORT DynamicList : std::vector<std::unique_ptr<T>> {
    template <typename ...Args>
    DynamicList(Args&&... args) {
        this->reserve(sizeof...(args));
        (..., add(std::forward<Args>(args)));
    }

private:
    void add(std::vector<std::unique_ptr<T>>&& v) {
        // move(v) is silly, but it shuts up the linter and probably doesn't
        // matter.
        for (auto& elem : std::move(v)) this->push_back(std::move(elem));
    }

    void add(std::unique_ptr<T>&& v) {
        this->push_back(std::move(v));
    }
};

} // namespace pcui::detail


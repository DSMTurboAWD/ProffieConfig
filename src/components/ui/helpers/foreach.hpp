#pragma once
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/helpers/foreach.hpp
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

#include "ui/types.hpp"

#include "ui_export.h"

namespace pcui {

template<typename Container, typename Functor>
struct UI_EXPORT ForEach {
    const Container& of_;
    const Functor do_;

    std::vector<DescriptorPtr> operator()() {
        std::vector<DescriptorPtr> ret;
        ret.reserve(std::ranges::distance(of_.begin(), of_.end()));
        for (const auto& item : of_) {
            ret.push_back(do_(item));
        }
        return ret;
    }
};

} // namespace pcui


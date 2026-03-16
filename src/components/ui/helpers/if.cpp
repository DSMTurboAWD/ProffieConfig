#include "if.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/ui/helpers/if.cpp
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

using namespace pcui;

DescriptorPtr NoneElement::operator()() {
    return std::make_unique<NoneElement::Desc>(std::move(*this));
}

NoneElement::Desc::Desc(NoneElement&& elem) :
    NoneElement(std::move(elem)) {}

wxSizerItem *NoneElement::Desc::build(const detail::Scaffold&) const {
    // Not sure how inefficient this is... probably not very.
    return new wxSizerItem(0, 0);
}


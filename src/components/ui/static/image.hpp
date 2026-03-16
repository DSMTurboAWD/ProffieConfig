#pragma once
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2024-2026 Ryan Ogurek
 *
 * components/ui/static/image.hpp
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

#include <wx/statbmp.h>

#include "data/generic.hpp"
#include "ui/detail/descriptor.hpp"
#include "ui/detail/general.hpp"
#include "ui/types.hpp"
#include "utils/color.hpp"

#include "ui_export.h"

namespace pcui {

struct UI_EXPORT Image {
    struct Desc;

    // TODO: Make this a base w/ C++ P2287.
    detail::ChildWindowBase win_;

    std::optional<RefWrap<data::Generic>> data_;

    wxStaticBitmapBase::ScaleMode scale_{wxStaticBitmapBase::Scale_AspectFill};

    struct LoadDetails {
        cstring name_{nullptr};

#       ifdef __APPLE__
        /**
         * If the image is an icon in the macOS bundle resources.
         */
        bool resourceIcon_{false};
#       endif

        struct {
            int32 dim_{-1};
            wxOrientation orient_{wxHORIZONTAL};
        } size_;

        color::Dynamic color_;

        wxBitmap operator()() const;
    };

    wxBitmap src_;

    DescriptorPtr operator()();
};

struct UI_EXPORT Image::Desc : Image, detail::Descriptor {
    Desc(Image&&);

    [[nodiscard]] wxSizerItem *build(const detail::Scaffold&) const override;
};

} // namespace pcui


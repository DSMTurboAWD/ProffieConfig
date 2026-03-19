#pragma once
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2023-2026 Ryan Ogurek
 *
 * proffieconfig/editor/editorwindow.hpp
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

#include <wx/frame.h>

#include "config/config.hpp"
#include "ui/frame.hpp"
#include "ui/types.hpp"

struct EditorWindow : pcui::Frame {
    EditorWindow(wxWindow *, config::Config&);

    bool Destroy() final;
    void Fit() final;
    void fitAnimated();

    // Handles errors
    bool save();

    [[nodiscard]] config::Config& getOpenConfig() const;

    GeneralPage *generalPage{nullptr};
    PropsPage *propsPage{nullptr};
    BladesPage *bladesPage{nullptr};
    PresetsPage *presetsPage{nullptr};

private:
    pcui::DescriptorPtr ui();

    void createMenuBar();
    void bindEvents();

    wxSize mBestSize{-1, -1};
    wxSize mStartSize{-1, -1};
    std::chrono::microseconds::rep mStartMicros;

    void configureResizing();

    config::Config& mConfig;
};

#pragma once
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2024-2026 Ryan Ogurek
 *
 * launcher/update/pulldata.hpp
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

#include "log/branch.hpp"
#include "ui/dialogs/progress.hpp"
#include "utils/paths.hpp"

#include "update.hpp"

namespace Update {

/**
 * Attempts to download latest version of `MANIFEST_FILE`
 */
[[nodiscard]] bool pullData(pcui::ProgressDialog *, logging::Branch&);

/**
 * Reads data from `MANIFEST_FILE`
 */
[[nodiscard]] std::optional<Data> parseData(
    pcui::ProgressDialog *, logging::Branch&, bool heedMessages = true
);

inline fs::path manifestFile() { return paths::dataDir() / "manifest.pconf"; }

} // namespace Update


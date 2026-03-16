#pragma once
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2024-2026 Ryan Ogurek
 *
 * launcher/update/changelog.hpp
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
#include "utils/hash.hpp"

#include "update.hpp"

namespace Update {

struct Changelog {
    utils::Version currentBundleVersion;
    utils::Version latestBundleVersion;

    struct ChangedFile {
        ItemID id;
        utils::hash::SHA256 hash;
        utils::Version currentVersion;
        utils::Version latestVersion;
    };

    std::vector<ChangedFile> changedFiles;
    std::vector<ItemID> removedFiles;
};

[[nodiscard]] Changelog generateChangelog(
    const Data&, const utils::Version& currentVersion, logging::Branch&
);

[[nodiscard]] bool promptWithChangelog(
    const Data&, const Changelog&, logging::Branch&
);

[[nodiscard]] utils::Version determineCurrentVersion(
    const Data&, pcui::ProgressDialog *, logging::Branch&
);

} // namespace Update


#pragma once
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2024-2026 Ryan Ogurek
 *
 * launcher/update/update.hpp
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

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include <wx/event.h>

#include "utils/types.hpp"
#include "utils/version.hpp"
#include "utils/hash.hpp"

namespace fs = std::filesystem;

namespace Update {

enum ItemType {
    EXEC,
    LIB,
    COMP,
    RSRC,
    TYPE_MAX
};

enum class Comparator {
    EQUAL,
    LESS_THAN,
    GREATER_THAN,
};

struct ItemID {
    ItemType type;
    std::string name;
    bool ignored{false};

    auto operator<=>(const ItemID&) const = default;
};

struct ItemVersionData {
    utils::hash::SHA256 hash;

    std::vector<std::string> fixes;
    std::vector<std::string> changes;
    std::vector<std::string> features;
};

struct Item {
    std::string path;

    using Versions = std::map<
        utils::Version, ItemVersionData, utils::Version::RawOrderer
    >;
    Versions versions;
    bool hidden;
};

struct Bundle {
    std::string note;

    struct RequiredItem {
        RequiredItem(ItemID id, utils::Version version) : 
            id{std::move(id)}, version{std::move(version)} {}

        ItemID id;
        utils::Version version;
        std::optional<utils::hash::SHA256> hash;
    };
    std::vector<RequiredItem> reqs;
};

using Items = std::map<ItemID, Item>;
using Bundles = std::map<
    utils::Version, Bundle, utils::Version::RawOrderer
>;

struct Data {
    Items items;
    Bundles bundles; 
};

void init();
[[nodiscard]] wxEvtHandler *getEventHandler();

fs::path typeFolder(ItemType);

} // namespace Update


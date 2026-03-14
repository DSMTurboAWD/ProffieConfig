#include "os.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2026 Ryan Ogurek
 *
 * components/versions/os.cpp
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

#include <memory>

#include "versions/priv/data.hpp"

using namespace versions::os;

Board::Board(
    std::string name,
    std::string coreId,
    std::string include
) : name_{std::move(name)},
    coreId_{std::move(coreId)},
    include_{std::move(include)} {}

Board::Board(const Board& other, data::Node *parent) :
    data::Model(parent),
    name_{other.name_},
    coreId_{other.coreId_},
    include_{other.include_} {}

OS::OS(
    utils::Version version,
    std::string coreUrl,
    utils::Version coreVer,
    std::vector<Board> boards
) : version_{std::move(version)},
    coreUrl_{std::move(coreUrl)},
    coreVersion_{std::move(coreVer)},
    boards_{[&boards]() {
        data::Vector vec;
        data::Vector::Context ctxt{vec};

        for (auto& board : boards) {
            ctxt.add(std::make_unique<Board>(board, &vec));
        }

        return vec;
    }()} {}

OS::OS(const OS& other, data::Node *parent) :
    data::Model(parent),
    version_{other.version_},
    coreUrl_{other.coreUrl_},
    coreVersion_{other.coreVersion_},
    boards_{other.boards_, parent} {}

OS::operator OSData() const {
    std::lock_guard scopeLock{pLock};

    std::vector<BoardData> boardsData;
    data::Vector::ROContext boards{boards_};

    boardsData.reserve(boards.children().size());
    for (const auto& model : boards.children()) {
        auto& board{static_cast<Board&>(*model)};
        boardsData.push_back({
            .name_=board.name_,
            .coreId_=board.coreId_,
            .include_=board.include_,
        });
    }

    return {
        .version_=version_,
        .coreUrl_=coreUrl_,
        .coreVersion_=coreVersion_,
        .boards_=std::move(boardsData)
    };
}

Context::Context() { priv::lock.lock(); }
Context::~Context() { priv::lock.unlock(); }

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const std::vector<OSData>& Context::available() {
    std::lock_guard scopeLock{priv::lock};
    return priv::availableOS;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const std::vector<std::unique_ptr<OS>>& Context::list() {
    std::lock_guard scopeLock{priv::lock};
    return priv::os;
}


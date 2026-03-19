#pragma once
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2025-2026 Ryan Ogurek
 *
 * components/utils/files.hpp
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
#include <fstream>

#include "utils_export.h"

namespace fs = std::filesystem;

namespace files {

UTILS_EXPORT bool copyOverwrite(const fs::path& src, const fs::path& dst, std::error_code& err);

// openInput and openOutput must be inline, otherwise things crash on Windows.
// Who knows why on earth that is. I tried to debug it a bit and it's just not
// worth it. Whatever, Microslop.
//
// These functions already only exist to uniformly deal with Windows' garbage
// cstdlib.
inline std::ifstream openInput(const fs::path& path) {
    // POSIX Specifies that there is no distinction between binary/text modes.
    // So this shouldn't make a difference on good operating systems.
    //
    // On Windows, however, MSVC's STL implementation just kills itself because
    // it can't handle its own stupid CRLF endings and tellg/seekg plainly do
    // not work.
    // See: https://github.com/microsoft/STL/issues/1784

    // These constructors are explicit on (at least) macOS
    return std::ifstream{path, std::ios::binary | std::ios::in};
}

inline std::ofstream openOutput(const fs::path& path) {
    return std::ofstream{path, std::ios::binary | std::ios::out};
}

} // namespace files


/**
 * Copyright 2026 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

class OutputStream
{
    std::unique_ptr<std::ofstream> file_;
    std::ostream* out_;

public:
    OutputStream()
        : out_(&std::cout)
    {
    }
    OutputStream(const std::string& filename)
        : file_(std::make_unique<std::ofstream>(filename)),
          out_(file_.get())
    {
        assert(file_->is_open() && "Failed to open output file");
    }

    // Delete copy operations since we have a unique_ptr member
    OutputStream(const OutputStream&) = delete;
    OutputStream& operator=(const OutputStream&) = delete;

    // Allow move operations (optional, but good practice)
    OutputStream(OutputStream&&) = default;
    OutputStream& operator=(OutputStream&&) = default;

    template <typename T> OutputStream& operator<<(const T& value)
    {
        (*out_) << value;
        return *this;
    }

    // Support for std::endl and other manipulators
    OutputStream& operator<<(std::ostream& (*manip)(std::ostream&))
    {
        (*out_) << manip;
        return *this;
    }
};
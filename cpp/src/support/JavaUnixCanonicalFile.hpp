/*
 * Copyright (c) 1994, 2021, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#pragma once
// Adapted from OpenJDK JDK_Canonicalize; GPLv2 with Classpath exception,
// see OPENJDK_TIMSORT_LICENSE. Existing JavaUnixFile supplies File normalization.
#include "JavaUnixFile.hpp"
#include "ByteIO.hpp"
#include <array>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
namespace minima {
inline std::string javaUnixCanonicalFile(const std::filesystem::path& file) {
    auto path=javaUnixAbsoluteFile(*javaUnixFile(file));
    if(path.find('\0')!=std::string::npos)throw wire::IOException("Invalid file path");
    if(path.size()>PATH_MAX)throw wire::IOException(std::strerror(ENAMETOOLONG));
    std::array<char,PATH_MAX+1> resolved{};
    std::string result;
    if(::realpath(path.c_str(),resolved.data()))result=resolved.data();
    else {
        // The first error is deliberately not propagated. Probe progressively
        // shorter prefixes; only ENOENT/ENOTDIR/EACCES permit another probe.
        size_t end=path.size();
        while(end) {
            auto slash=path.rfind('/',end-1);
            if(slash==std::string::npos || slash==0)break;
            auto prefix=path.substr(0,slash);
            if(::realpath(prefix.c_str(),resolved.data())) {
                result=resolved.data();auto suffix=path.substr(slash);
                if(result.size()+suffix.size()>=PATH_MAX)throw wire::IOException(std::strerror(ENAMETOOLONG));
                if(result.ends_with('/') && suffix.starts_with('/'))suffix.erase(0,1);
                result+=suffix;break;
            }
            auto error=errno;
            if(error!=ENOENT && error!=ENOTDIR && error!=EACCES)throw wire::IOException(std::strerror(error));
            end=slash;
        }
        if(result.empty())result=path;
    }
    result=std::filesystem::path(result).lexically_normal().string();
    while(result.size()>1 && result.back()=='/')result.pop_back();
    return result;
}
}

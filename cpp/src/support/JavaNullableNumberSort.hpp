/*
 * Copyright (c) 2009, 2013, Oracle and/or its affiliates. All rights reserved.
 * Copyright 2009 Google Inc.  All Rights Reserved.
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
#include "org/minima/objects/base/MiniNumber.hpp"
#include "JavaExceptions.hpp"
#include <algorithm>
#include <memory>
#include <vector>
namespace minima {
// Adapted from OpenJDK java.util.TimSort (see OPENJDK_TIMSORT_LICENSE).
// Only nullable, immutable MiniNumber comparisons are supported. Run discovery
// and binary insertion preserve the list at a null-comparison failure. Once a
// run with two or more elements is complete it contains no nulls. A final
// singleton null fails at Java's first merge comparison; guard it explicitly.
// Stable merging of the other completed runs cannot throw that
// exception and its final permutation is independent of galloping strategy.
inline void javaNullableNumberSort(std::vector<std::shared_ptr<MiniNumber>>& values) {
    auto before=[](const auto& left,const auto& right){
        if(!right||!left)throw JavaNullPointerException("null median element");
        return right->isLess(*left);
    };
    const size_t size=values.size();if(size<2)return;
    auto run=[&](size_t start){
        size_t end=start+1;if(end==size)return size_t(1);
        if(before(values[end++],values[start])){
            while(end<size&&before(values[end],values[end-1]))++end;
            std::reverse(values.begin()+start,values.begin()+end);
        }else while(end<size&&!before(values[end],values[end-1]))++end;
        return end-start;
    };
    auto insert=[&](size_t start,size_t end,size_t next){
        if(next==start)++next;
        for(;next<end;++next){
            auto pivot=values[next];size_t left=start,right=next;
            while(left<right){auto middle=left+(right-left)/2;if(before(pivot,values[middle]))right=middle;else left=middle+1;}
            std::copy_backward(values.begin()+left,values.begin()+next,values.begin()+next+1);values[left]=pivot;
        }
    };
    if(size<32){insert(0,size,run(0));return;}
    struct Run {size_t base,length;};std::vector<Run> runs;
    auto merge=[&](size_t index){
        const auto a=runs[index],b=runs[index+1];
        if(!values[b.base])throw JavaNullPointerException("null median element");
        std::inplace_merge(values.begin()+a.base,values.begin()+b.base,values.begin()+b.base+b.length,before);
        runs[index].length+=b.length;runs.erase(runs.begin()+index+1);
    };
    size_t minimum=size,remainder=0;
    while(minimum>=32){remainder|=minimum&1;minimum>>=1;}minimum+=remainder;
    size_t start=0;
    while(start<size){
        auto length=run(start);
        if(length<minimum){auto forced=std::min(minimum,size-start);insert(start,start+forced,start+length);length=forced;}
        runs.push_back({start,length});
        while(runs.size()>1){
            size_t n=runs.size()-2;
            if((n>0&&runs[n-1].length<=runs[n].length+runs[n+1].length)||
               (n>1&&runs[n-2].length<=runs[n].length+runs[n-1].length)){
                if(runs[n-1].length<runs[n+1].length)--n;
            }else if(runs[n].length>runs[n+1].length)break;
            merge(n);
        }
        start+=length;
    }
    while(runs.size()>1){auto n=runs.size()-2;if(n>0&&runs[n-1].length<runs[n+1].length)--n;merge(n);}
}
}

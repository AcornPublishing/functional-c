//
// immer: immutable data structures for C++
// Copyright (C) 2016, 2017, 2018 Juan Pedro Bolivar Puente
//
// This software is distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt
//

#include "extra/fuzzer/fuzzer_input.hpp"
#include <immer/flex_vector.hpp>
#include <array>
#include <iostream>
#include <catch.hpp>

#define IMMER_FUZZED_TRACE_ENABLE 0

#if IMMER_FUZZED_TRACE_ENABLE
#define IMMER_FUZZED_TRACE(...) std::cout << __VA_ARGS__ << std::endl;
#else
#define IMMER_FUZZED_TRACE(...)
#endif

namespace {

template <std::size_t VarCount = 8, unsigned Bits = 3>
int run_input(const std::uint8_t* data, std::size_t size)
{
    using vector_t = immer::flex_vector<int, immer::default_memory_policy, Bits, Bits>;
    using size_t   = std::uint8_t;

    auto vars = std::array<vector_t, VarCount>{};

#if IMMER_FUZZED_TRACE_ENABLE
    for (auto i = 0u; i < VarCount; ++i)
        std::cout << "auto var" << i << " = vector_t{};" << std::endl;
#endif

    auto is_valid_var = [&] (auto idx) {
        return idx >= 0 && idx < VarCount;
    };
    auto is_valid_index = [] (auto& v) {
        return [&] (auto idx) { return idx >= 0 && idx < v.size(); };
    };
    auto is_valid_size = [] (auto& v) {
        return [&] (auto idx) { return idx >= 0 && idx <= v.size(); };
    };

    return fuzzer_input{data, size}.run([&] (auto& in)
    {
        enum ops {
            op_push_back,
            op_update,
            op_take,
            op_drop,
            op_concat,
        };
        auto src = read<std::uint8_t>(in, is_valid_var);
        auto dst = read<std::uint8_t>(in, is_valid_var);
        switch (read<char>(in))
        {
        case op_push_back:
            IMMER_FUZZED_TRACE(
                "var" << +dst << " = var" << +src <<
                ".push_back(42);");
            vars[dst] = vars[src].push_back(42);
            break;
        case op_update: {
            auto idx = read<size_t>(in, is_valid_index(vars[src]));
            IMMER_FUZZED_TRACE(
                "var" << +dst << " = var" << +src <<
                ".update(" << +idx << ", [] (auto x) { return x + 1; });");
            vars[dst] = vars[src].update(idx, [] (auto x) { return x + 1; });
            break;
        }
        case op_take: {
            auto idx = read<size_t>(in, is_valid_size(vars[src]));
            IMMER_FUZZED_TRACE(
                "var" << +dst << " = var" << +src <<
                ".take(" << +idx << ");");
            vars[dst] = vars[src].take(idx);
            break;
        }
        case op_drop: {
            auto idx = read<size_t>(in, is_valid_size(vars[src]));
            IMMER_FUZZED_TRACE(
                "var" << +dst << " = var" << +src <<
                ".take(" << +idx << ");");
            vars[dst] = vars[src].drop(idx);
            break;
        }
        case op_concat: {
            auto src2 = read<std::uint8_t>(in, is_valid_var);
            IMMER_FUZZED_TRACE(
                "var" << +dst << " = var" << +src << " + var" << +src2 << ";");
            vars[dst] = vars[src] + vars[src2];
            break;
        }
        default:
            break;
        };
        return true;
    });
}

} // anonymous namespace

TEST_CASE("bug: concatenate too big vectors")
{
    // The problem here was that since we were using 32 bit sizes,
    // concatenating big flex_vectors can overflow the sizes.  Let's
    // just use std::size_t like normal people.
    //
    // Still, the problem could re-ocurr with longer inputs.  For this
    // reason later fuzzing efforts do check that concatenation is
    // valid for the given vector sizes.  Similar assertions are put
    // in the code too.
    SECTION("simplified example")
    {
        using vector_t = immer::flex_vector<int, immer::default_memory_policy, 3, 3>;
        auto var0 = vector_t{};
        auto var1 = vector_t{};
        auto var2 = vector_t{};
        auto var4 = vector_t{};
        var1 = var1.push_back(42);
        var0 = var0.push_back(42);
        var0 = var0.push_back(42);
        var0 = var2.push_back(42);
        var0 = var0.push_back(42);
        var2 = var0;
        var0 = var0.push_back(42);
        var0 = var0.push_back(42);
        var4 = var4.push_back(42);
        var0 = var0.push_back(42);
        var0 = var0.push_back(42);
        var0 = var0 + var0;
        var0 = var0.push_back(42);
        var0 = var0.push_back(42);
        var2 = var0.push_back(42);
        var0 = var0 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var0 = var0.push_back(42);
        var0 = var0.push_back(42);
        var1 = var2 + var4;
        var4 = var4 + var4;
        var0 = var1.push_back(42);
        var0 = var0.push_back(42);
        var0 = var0 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4 + var4;
        var4 = var4.push_back(42);
    }

    // Assertion `!p->relaxed()' failed
    SECTION("")
    {
        constexpr std::uint8_t input[] = {
            0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x39,0x6a,0x76,0xb9,0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x1,0x0,0x0,0x2a,0x0,0x0,0x0,0x0,0x0,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x1,0x4,0x4,0x4,0x4,0x4,0x4,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x2a,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x0,
        };
        CHECK(run_input(input, sizeof(input)) == 0);
    }
    SECTION("")
    {
        constexpr std::uint8_t input[] = {
            0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0x4,0x4,0x4,0x4,0x4,0xf8,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x21,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0xb,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x17,0x4,0xe2,0xe2,0xe2,0x2a,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x21,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x17,0x4,0xe2,0xe2,0xe2,0x2a,0xe2,0xe2,0xe2,0xe2,0xe2,0x1f,0xe2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0xe2,0xe2,0xe2,0xe2,0xe2,0xe2,0x0,0x0,0x0,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x15,0x15,0x15,0x15,0x0,0x0,
        };
        CHECK(run_input(input, sizeof(input)) == 0);
    }
    SECTION("")
    {
        constexpr std::uint8_t input[] = {
            0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x21,0x0,0x0,0x0,0x0,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x27,0x0,0x21,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x3a,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x4,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0xe4,0xe4,0x0,0x0,0x0,0x0,0xe4,0x0,0xe4,0x0,0x0,0x0,0x0,0x0,0x8,0x0,
        };
        CHECK(run_input(input, sizeof(input)) == 0);
    }

    // buffer overflow when looking inside the sizes array for the
    // index of a position
    SECTION("")
    {
        constexpr std::uint8_t input[] = {
            0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x4,0x4,0x4,0x4,0x4,0x3,0xff,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x1e,0x0,0x4,0x4,0x4,0x4,0x4,0x3,0xff,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xdb,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
        };
        CHECK(run_input(input, sizeof(input)) == 0);
    }
    SECTION("")
    {
        constexpr std::uint8_t input[] = {
            0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x9,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x3,0xfa,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x9,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x3,0xfa,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x3,0xfa,0x4,0x4,0x4,0x0,0x0,0x0,0x55,0x0,
        };
        CHECK(run_input(input, sizeof(input)) == 0);
    }

    // fail when deref some null node
    SECTION("")
    {
       constexpr std::uint8_t input[] = {
           0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x4,0x0,0x0,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x4,0x0,0x4,0x4,0x4,0xe4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x6,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0xe5,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0xff,0x3,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x4,0x0,0x4,0x4,0x4,0xe4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x6,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0xe5,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x4,0x4,0x4,0x4,0xe1,0x0,0x0,0x80,0x0,0x0,0x1,0x6,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x75,0x75,0x45,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x75,0x4,0x0,
       };
       CHECK(run_input(input, sizeof(input)) == 0);
    }
    SECTION("")
    {
       constexpr std::uint8_t input[] = {
           0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x85,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0xff,0xff,0xff,0xff,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x5,0x4,0x28,0x4,0x4,0x4,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x24,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x0,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0xf3,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0xf3,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x3,0x4,0x4,0x4,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xad,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
       };
       CHECK(run_input(input, sizeof(input)) == 0);
    }
}

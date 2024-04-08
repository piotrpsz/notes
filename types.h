//
// Created by piotr on 08.04.24.
//
#pragma once
#include <cstdint>
#include <vector>
#include <filesystem>

using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;

template<typename T>
    using vec = std::vector<T>;

namespace fs = std::filesystem;

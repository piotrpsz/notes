//
// Created by piotr on 08.04.24.
//
#pragma once

#include "value.h"
#include <utility>
#include <fmt/core.h>

class Field {
    std::pair<std::string,Value> data_{};
public:
    Field() = default;
    explicit Field(const std::string& name) : data_{name, {}} {}
    explicit Field(std::string name, Value v) : data_{std::move(name), std::move(v)} {}
    explicit Field(std::pair<std::string, Value> data) : data_{std::move(data)} {}
    ~Field() = default;

    Field(Field const&) = default;
    Field(Field&&) = default;
    Field& operator=(Field const&) = default;
    Field& operator=(Field&&) = default;

    auto operator()() && {
        return std::move(data_);
    }
    auto& operator()() const& {
        return data_;
    }

    [[nodiscard]] std::string description() const noexcept {
        auto [name, value] = data_;
        return fmt::format("{}:[{}]", name, value.description());
    }
};


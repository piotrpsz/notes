//
// Created by piotr on 08.04.24.
//
#pragma once

#include "value.h"
#include <utility>
#include <string>
#include <fmt/core.h>

class Field {
    std::pair<std::string,value_t> data_{};
public:
    Field() = default;
    explicit Field(std::string name, value_t v = {}) : data_{std::move(name), std::move(v)} {}
    explicit Field(std::pair<std::string, value_t> pair) : data_{std::move(pair)} {}
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
    [[nodiscard]] std::string const& name() const noexcept {
        return data_.first;
    }
    [[nodiscard]] value_t const& value() const noexcept {
        return data_.second;
    }

    [[nodiscard]] std::string description() const noexcept {
        auto [name, value] = data_;
        return fmt::format("{}:[{}]", name, value.description());
    }
};


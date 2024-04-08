//
// Created by piotr on 08.04.24.
//
#pragma once

#include "row.h"

class Result {
    vec<Row> data_{};
public:
    Result() = default;
    explicit Result(Row row) {
        data_.push_back(std::move(row));
    }
    ~Result() = default;

    Result(Result const&) = default;
    Result& operator=(Result const&) = default;
    Result(Result &&) = default;
    Result &operator=(Result &&) = default;

    [[nodiscard]] auto empty() const noexcept {
        return data_.empty();
    }
    [[nodiscard]] auto size() const noexcept {
        return data_.size();
    }

    using iterator = std::vector<Row>::iterator;
    using const_iterator = std::vector<Row>::const_iterator;
    iterator begin()  { return data_.begin(); }
    iterator end() { return data_.end(); }
    [[maybe_unused]] [[nodiscard]] const_iterator cbegin() const { return data_.cbegin(); }
    [[maybe_unused]] [[nodiscard]] const_iterator cend() const { return data_.cend(); }
};

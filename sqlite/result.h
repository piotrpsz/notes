//
// Created by piotr on 08.04.24.
//
#pragma once

#include "row.h"

class result_t {
    std::vector<row_t> data_{};
public:
    result_t() = default;
    explicit result_t(row_t row) {
        data_.push_back(std::move(row));
    }
    ~result_t() = default;

    result_t(result_t const&) = default;
    result_t& operator=(result_t const&) = default;
    result_t(result_t &&) = default;
    result_t& operator=(result_t&&) = default;

    [[nodiscard]] auto empty() const noexcept {
        return data_.empty();
    }
    [[nodiscard]] auto size() const noexcept {
        return data_.size();
    }
    void push_back(row_t row) noexcept {
        data_.push_back(std::move(row));
    }
    row_t operator[](int i) const noexcept {
        return data_[i];
    }

    using iterator = std::vector<row_t>::iterator;
    using const_iterator = std::vector<row_t>::const_iterator;
    iterator begin()  { return data_.begin(); }
    iterator end() { return data_.end(); }
    [[maybe_unused]] [[nodiscard]] const_iterator cbegin() const { return data_.cbegin(); }
    [[maybe_unused]] [[nodiscard]] const_iterator cend() const { return data_.cend(); }
};

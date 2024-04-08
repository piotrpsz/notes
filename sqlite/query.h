//
// Created by piotr on 08.04.24.
//
#pragma once
#include "value.h"

class query_t {
    std::string query_{};
    std::vector<value_t> values_{};
public:
    query_t() = default;
    /// Query for name and arguments with fold-expression
    template<typename... T>
    explicit query_t(std::string query, T... args) : query_{std::move(query)} {
        (..., values_.push_back(Value(args)));
    }
    /// Query for name (without arguments).
    explicit query_t(std::string query) : query_{std::move(query)} {}
    ~query_t() = default;

    query_t(query_t const&) = default;
    query_t(query_t&&) = default;
    query_t& operator=(query_t const&) = default;
    query_t& operator=(query_t&&) = default;

    /// Check if query is valid. \n
    /// The query is valid if the number of placeholders(?) is equal
    /// to the number of query arguments.
    [[nodiscard]] bool valid() const noexcept {
        // Count placeholders.
        auto placeholder_count = count_if(query_.cbegin(), query_.cend(), [](char const c) {
            return '?' == c;
        });
        // The number of placeholders must equal the number of arguments.
        if (std::cmp_not_equal(placeholder_count, values_.size())) {
            fmt::print(stderr, "The number of placeholders and arguments does not match ({}, {})\n", placeholder_count, values_.size());
            return false;
        }
        return true;
    }

    /// Get query itself.
    [[nodiscard]] std::string const& query() const noexcept {
        return query_;
    }
    /// Get query's arguments.
    [[nodiscard]] std::vector<value_t> values() const noexcept {
        return values_;
    }
};

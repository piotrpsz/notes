//
// Created by piotr on 08.04.24.
//
#pragma once

#include "../shared.h"
#include <variant>

class value_t {
    std::variant<std::monostate, i64, f64, std::string, std::vector<u8>> data_{};
public:
    enum {
        Monostate, Integer, Double, String, Vector
    };

    value_t() = default;
    value_t(std::integral auto v) : data_{static_cast<i64>(v)} {}
    value_t(std::floating_point auto v) : data_{static_cast<f64>(v)} {}
    value_t(std::string v) : data_{std::move(v)} {}
    value_t(std::vector<u8> v) : data_{std::move(v)} {}
    ~value_t() = default;

    template<typename T>
    explicit value_t(std::optional<T> v) {
        if (v) data_ = v.value();
        else data_ = {};
    }

    // default copy, default move
    value_t(value_t const&) = default;
    value_t(value_t&&) = default;
    value_t& operator=(value_t const&) = default;
    value_t& operator=(value_t&&) = default;

    auto operator()() && {
        return std::move(data_);
    }
    auto& operator()() const& {
        return data_;
    }
    [[nodiscard]] bool null() const noexcept {
        return data_.index() == Monostate;
    }
    [[nodiscard]] uint index() const noexcept {
        return data_.index();
    }

    // getting values without checking
    [[nodiscard]] i64 int64() const noexcept {
        return std::get<Integer>(data_);
    }
    [[nodiscard]] f64 float64() const noexcept {
        return std::get<Double>(data_);
    }
    [[nodiscard]] std::string str() const noexcept {
        return std::get<String>(data_);
    }
    [[nodiscard]] std::vector<u8> vec() const noexcept {
        return std::get<Vector>(data_);
    }

    // getting values with checking
    [[nodiscard]] std::optional<i64> int64_if() const noexcept {
        if (data_.index() == Integer)
            return int64();
        return {};
    }
    [[nodiscard]] std::optional<f64> float64_if() const noexcept {
        if (data_.index() == Double)
            return float64();
        return {};
    }
    [[nodiscard]] std::optional<std::string> str_if() const noexcept {
        if (data_.index() == String)
            return str();
        return {};
    }
    [[nodiscard]] std::optional<std::vector<u8>> vec_if() const noexcept {
        if (data_.index() == Vector)
            return vec();
        return {};
    }

    /// value_t string description.
    [[nodiscard]] std::string description() const noexcept {
        switch (data_.index()) {
            case  Monostate:
                return "NULL";
            case Integer:
                return fmt::format("int64{{{}}}", std::get<Integer>(data_));
            case Double:
                return fmt::format("double{{{}}}", std::get<Double>(data_));
            case String:
                return fmt::format("string{{{}}}", std::get<String>(data_));
            case Vector: {
                auto vec = std::get<Vector>(data_);
                return fmt::format("blob{{{}}}", shared::bytes2str(vec));
            }
            default:
                return "?";
        }
    }
};
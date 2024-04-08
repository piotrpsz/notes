//
// Created by piotr on 08.04.24.
//
#pragma once

#include "../shared.h"
#include <variant>

namespace index {
    enum {
        Monostate, Integer, Double, String, Vector
    };
}

class Value {
    std::variant<std::monostate, i64, f64, std::string, vec<u8>> data_{};
public:
    Value() = default;
    explicit Value(std::integral auto v) : data_{static_cast<i64>(v)} {}
    explicit Value(std::floating_point auto v) : data_{static_cast<f64>(v)} {}
    explicit Value(std::string v) : data_{std::move(v)} {}
    explicit Value(vec<u8> v) : data_{std::move(v)} {}
    ~Value() = default;

    template<typename T>
    explicit Value(std::optional<T> v) {
        if (v) data_ = v.value();
        else data_ = {};
    }

    // default copy, default move
    Value(Value const&) = default;
    Value(Value&&) = default;
    Value& operator=(Value const&) = default;
    Value& operator=(Value&&) = default;

    auto operator()() && {
        return std::move(data_);
    }
    auto& operator()() const& {
        return data_;
    }
    [[nodiscard]] bool null() const noexcept {
        return data_.index() == index::Monostate;
    }
    [[nodiscard]] uint index() const noexcept {
        return data_.index();
    }

    // getting values without checking
    [[nodiscard]] i64 int64() const noexcept {
        return std::get<index::Integer>(data_);
    }
    [[nodiscard]] f64 float64() const noexcept {
        return std::get<index::Double>(data_);
    }
    [[nodiscard]] std::string str() const noexcept {
        return std::get<index::String>(data_);
    }
    [[nodiscard]] std::vector<u8> vec() const noexcept {
        return std::get<index::Vector>(data_);
    }

    // getting values with checking
    [[nodiscard]] std::optional<i64> int64_if() const noexcept {
        if (data_.index() == index::Integer)
            return int64();
        return {};
    }
    [[nodiscard]] std::optional<f64> float64_if() const noexcept {
        if (data_.index() == index::Double)
            return float64();
        return {};
    }
    [[nodiscard]] std::optional<std::string> str_if() const noexcept {
        if (data_.index() == index::String)
            return str();
        return {};
    }
    [[nodiscard]] std::optional<std::vector<u8>> vec_if() const noexcept {
        if (data_.index() == index::Vector)
            return vec();
        return {};
    }

    /// Value string description.
    [[nodiscard]] std::string description() const noexcept {
        switch (data_.index()) {
            using namespace index;
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
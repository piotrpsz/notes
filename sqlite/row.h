//
// Created by piotr on 08.04.24.
//
#pragma once

#include "field.h"


class Row {
    std::vector<Field> data_{};
public:
    Row() = default;

    /// Init object with one, initial field.
    Row(std::string name, value_t value) {
        data_.emplace_back(std::move(name), std::move(value));
    }

    ~Row() = default;

    Row(Row const &) = default;

    Row &operator=(Row const &) = default;

    Row(Row &&) = default;

    Row &operator=(Row &&) = default;

    [[nodiscard]] bool empty() const noexcept {
        return data_.empty();
    }

    [[nodiscard]] auto size() const noexcept {
        return data_.size();
    }

    Row& emplace_back(std::string name, value_t value) noexcept {
        data_.emplace_back(std::move(name), std::move(value));
        return *this;
    }

    Row &add(const std::string &name) {
        data_.emplace_back(name);
        return *this;
    }

    Row &add(Field f) noexcept {
        data_.push_back(std::move(f));
        return *this;
    }

    template<typename T>
    Row &add(std::string name, std::optional<T> value) noexcept {
        return value
               ? add(std::move(name), std::move(*value))
               : add(name);
    }

    using iterator = std::vector<Field>::iterator;
    using const_iterator = std::vector<Field>::const_iterator;
    iterator begin() { return data_.begin(); }
    iterator end() { return data_.end(); }
    [[nodiscard]] const_iterator cbegin() const { return data_.cbegin(); }
    [[nodiscard]] const_iterator cend() const { return data_.cend(); }

    /// Split vector of pairs <name, value> to two vectors. \n
    /// The first one with names, the second one with values.
    [[nodiscard]] std::pair<std::vector<std::string>,std::vector<value_t>> split() const noexcept {
        std::vector<std::string> names{};
        std::vector<value_t> values{};

        if (not data_.empty()) {
            auto const n = data_.size();
            names.reserve(n);
            values.reserve(n);

            for (auto const& f : data_) {
                auto const& [name, value] = f();
                names.push_back(name);
                values.push_back(value);
            }
        }

        return {std::move(names), std::move(values)};
    }

    [[nodiscard]] std::string description() const noexcept {
        std::vector<std::string> buffer{};
        buffer.reserve(data_.size());

        for (auto const& f : data_) {
            buffer.push_back(f.description());
        }

        return shared::join(buffer);
    }
};

//
// Created by piotr on 08.04.24.
//
#pragma once

#include "field.h"
#include <unordered_map>


class Row {
    std::unordered_map<std::string, Field> data_{};
public:
    Row() = default;

    /// Init object with one, initial field.
    Row(std::string const& name, value_t value) {
        data_[name] = Field{name, std::move(value)};
    }

    ~Row() = default;

    Row(Row const&) = default;
    Row& operator=(Row const&) = default;
    Row(Row&&) = default;
    Row& operator=(Row&&) = default;

    [[nodiscard]] bool empty() const noexcept {
        return data_.empty();
    }

    std::optional<Field> operator[](std::string const& name) noexcept {
        if (data_.contains(name))
            return data_[name];
        return {};
    }

    [[nodiscard]] auto size() const noexcept {
        return data_.size();
    }

    Row& add(std::string name, value_t value) noexcept {
        Field f{std::move(name), std::move(value)};
        return add(f);
    }
    Row& add(std::string name) noexcept {
        Field f{std::move(name)};
        return add(f);
    }
    Row& add(Field const& f) noexcept {
        data_[f.name()] = f;
        return *this;
    }

    template<typename T>
    Row& add(std::string name, std::optional<T> value) noexcept {
        return value
               ? add(std::move(name), std::move(*value))
               : add(name);
    }

    using iterator = std::unordered_map<std::string,Field>::iterator;
    using const_iterator = std::unordered_map<std::string,Field>::const_iterator;
    iterator begin() { return data_.begin(); }
    iterator end() { return data_.end(); }
    [[maybe_unused]] [[nodiscard]] const_iterator cbegin() const { return data_.cbegin(); }
    [[maybe_unused]] [[nodiscard]] const_iterator cend() const { return data_.cend(); }

    /// Split vector of pairs <name, value> to two vectors. \n
    /// The first one with names, the second one with values.
    [[nodiscard]] std::pair<std::vector<std::string>,std::vector<value_t>> split() const noexcept {
        std::vector<std::string> names{};
        std::vector<value_t> values{};

        if (not data_.empty()) {
            auto const n = data_.size();
            names.reserve(n);
            values.reserve(n);

            for (auto const& it : data_) {
                names.push_back(it.second.name());
                values.push_back(it.second.value());
            }
        }

        return {std::move(names), std::move(values)};
    }

    [[nodiscard]] std::string description() const noexcept {
        std::vector<std::string> buffer{};
        for (const auto & it : data_)
            buffer.push_back(it.second.description());
        return shared::join(buffer);
    }
};

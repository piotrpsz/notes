//
// Created by piotr on 09.05.24.
//

#pragma once

#include <QObject>
#include "category.hh"
#include <vector>
#include <unordered_map>
#include <fmt/core.h>

class StoreCategory : QObject {
    using repository_t = std::unordered_map<i64, std::vector<Category>>;
    Q_OBJECT
public:
    explicit StoreCategory(QObject* = nullptr);

    // co copy
    StoreCategory(StoreCategory const&) = delete;
    StoreCategory& operator=(StoreCategory const&) = delete;
    
    // no move
    StoreCategory(StoreCategory&&) = delete;
    StoreCategory& operator=(StoreCategory&&) = delete;

    void print() const noexcept {
        for (auto const& [id, vec] : data_) {
            fmt::print("{}\n", id);
            for (auto const& category : vec)
                fmt::print("\t{}\n", category.str());
        }
    }

    bool hasSubcategories(i64 const id) noexcept {
        return not data_[id].empty();
    }
    bool exist(i64 const pid, std::string const& name) noexcept {
        return std::ranges::any_of(data_[pid], [name](auto const& c) {
            return c.name() == name;
        });
    }

    std::vector<Category> childsForParentWithID(int const pid) const noexcept {
        auto it = data_.find(pid);
        if (it != data_.end())
            return it->second;
        return {};
    }

private:
    repository_t data_{};

};

//
// Created by piotr on 09.05.24.
//

#pragma once

#include <QObject>
#include "category.hh"
#include <vector>
#include <fmt/core.h>

class StoreCategory : QObject {
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
        for (auto const& category : data_)
            fmt::print("{}\n", category.str());
    }

private:
    std::vector<Category> data_{};

};

//
// Created by piotr on 09.05.24.
//

#include "StoreCategory.hh"

StoreCategory::StoreCategory(QObject* parent) : QObject(parent) {
    if (auto categories = Category::all(); categories)
        data_ = std::move(categories.value());
}

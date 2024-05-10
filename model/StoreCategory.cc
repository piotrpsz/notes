//
// Created by piotr on 09.05.24.
//

#include "StoreCategory.hh"

using namespace std;

StoreCategory::StoreCategory(QObject* parent) : QObject(parent) {
    if (auto categories = Category::all(); categories) {
        for (auto&& category : categories.value()) {
            data_[category.pid()].push_back(category);
        }
    }
}

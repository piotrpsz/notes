//
// Created by piotr on 22.05.24.
//
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include "../model/category.hh"
#include "../shared.hh"
#include <string>
#include <fmt/core.h>

namespace Tools {

    /// Utworzenie elementu ciągu kategorii. \n
    /// Jeśli katagoria nie jest ostatnia w ciągu dodawana jest 'strzałka'.
    static inline std::string
    fmtCategoryItem(std::string const& name, bool last = false) noexcept {
        if (last) return fmt::format("<b><font color=#94B88B>{}</font></b>", name);
        return fmt::format("<b><font color=#94B88B>{}</font></b> <font color=#99a3a4>{}</font> ", name, "\u22b3");
    }

    /// Utworzenie tekstu zawiarającego ciąg kategotii
    /// od początku aż do kategoii wskazanej przez numer ID.
    static inline std::string
    categoriesChainInfo(i64 const categoryID) noexcept {
        // Każdy opis kategori rozpoczyna się od 'Category'.
        std::string text{"<b><font color=#5499c7>Category:</font></b> "};

        if (categoryID > 0) {
            if (auto chain = Category::namesChainFor(categoryID); chain) {
                auto names{std::move(*chain)};
                auto lastValidIndex = names.size() - 1;
                for (auto i = 0; i < lastValidIndex; ++i)
                    text += fmtCategoryItem(names[i]);
                text += fmtCategoryItem(names[lastValidIndex], true);
            }
        }

        return text;
    }
}

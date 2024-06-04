//
// Created by Piotr Pszczółkowski on 27/05/2024.
//

/*------- include files:
-------------------------------------------------------------------*/
#include "CategoryTreeBrowser.hh"
#include "../model/StoreCategory.hh"
#include <QHeaderView>
#include <QTreeWidgetItem>

/*------- forward declarations:
-------------------------------------------------------------------*/
class StoreCategory;

CategoryTreeBrowser::CategoryTreeBrowser(i64 categoryID, QWidget* const parent) :
        QTreeWidget(parent)
{
    auto p = palette();
    p.setColor(QPalette::Base, QColor{60, 60, 60, 255});
    setAutoFillBackground(true);
    setPalette(p);

    header()->hide();
    setColumnCount(1);
    setHorizontalScrollMode(ScrollPerPixel);

    populateDialogContent();
    if (auto item = childOfParentWithID(root_, categoryID); item) {
        auto parentItem = item->parent();
        while (parentItem) {
            if (not parentItem->isExpanded())
                expandItem(parentItem);
            parentItem = parentItem->parent();
        }
        setCurrentItem(item);
    }
}

void CategoryTreeBrowser::
populateDialogContent() noexcept {
    clear();

    delete store_;
    store_ = new StoreCategory{this};

    // Ustawiamy kategorię 'root', która jest rodzicem
    // wszystkich innych kategorii.
    root_ = new QTreeWidgetItem(this);
    root_->setText(0, "Categories");
    root_->setData(0, IdRole, 0);
    root_->setData(0, PidRole, 0);

    addItemsFor(root_);
    root_->setExpanded(true);
}

void CategoryTreeBrowser::
addItemsFor(QTreeWidgetItem* const parent) noexcept {
    auto const pid = parent->data(0, IdRole).toInt();
    auto childs = store_->childsForParentWithID(pid);

    // Sortowanie potomków alfabetycznie rosnąco w/g nazwy
    std::ranges::sort(childs, [](auto const& a, auto const& b) {
        return QString::compare(a.qname(), b.qname(), Qt::CaseInsensitive) < 0;
    });

    // Dodanie potomków do przodka w drzewie.
    // Rekursywnie odczytujemy pod-potomków dla każdego z potomków.
    std::ranges::for_each(childs, [parent, this](auto const& category) {
        auto const item = new QTreeWidgetItem(parent);
        item->setText(0, category.qname());
        item->setData(0, IdRole, category.qid());
        item->setData(0, PidRole, category.qpid());
        addItemsFor(item);
    });
}

QTreeWidgetItem* CategoryTreeBrowser::
childOfParentWithID(QTreeWidgetItem* parent, i64 id) noexcept {
    for (auto it = QTreeWidgetItemIterator{parent}; *it; ++it) {
        auto const item = *it;
        i64 const item_id = item->data(0, IdRole).toInt();
        if (item_id == id)
            return item;
    }
    return nullptr;
}

std::optional<i64> CategoryTreeBrowser::
selectedCategoryID() const noexcept {
    if (auto item = currentItem(); item) {
        if (auto itemID = item->data(0, IdRole); itemID.canConvert<int>()) {
            return itemID.toInt();
        }
    }
    return {};
}
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

    populate_content();
    if (auto item = child_with_id_for(root_, categoryID); item) {
        auto parent = item->parent();
        while (parent) {
            if (not parent->isExpanded())
                expandItem(parent);
            parent = parent->parent();
        }
        setCurrentItem(item);
    }
}

void CategoryTreeBrowser::
populate_content() noexcept {
    clear();

    delete store_;
    store_ = new StoreCategory{this};

    // Ustawiamy kategorię 'root', która jest rodzicem
    // wszystkich innych kategorii.
    root_ = new QTreeWidgetItem(this);
    root_->setText(0, "Categories");
    root_->setData(0, IdRole, 0);
    root_->setData(0, PidRole, 0);

    add_items_for(root_);
    root_->setExpanded(true);
}

void CategoryTreeBrowser::
add_items_for(QTreeWidgetItem* const parent) noexcept {
    auto const pid = parent->data(0, IdRole).toInt();
    auto childs = store_->childs(pid);
    std::ranges::sort(childs, [](auto const& a, auto const& b) {
        return QString::compare(a.qname(), b.qname(), Qt::CaseInsensitive) < 0;
    });
    std::ranges::for_each(childs, [parent, this](auto const& category) {
        auto const item = new QTreeWidgetItem(parent);
        item->setText(0, category.qname());
        item->setData(0, IdRole, category.qid());
        item->setData(0, PidRole, category.qpid());
        add_items_for(item);
    });
}

QTreeWidgetItem* CategoryTreeBrowser::
child_with_id_for(QTreeWidgetItem* parent, i64 id) noexcept {
    for (auto it = QTreeWidgetItemIterator{parent}; *it; ++it) {
        auto const item = *it;
        i64 const item_id = item->data(0, IdRole).toInt();
        if (item_id == id)
            return item;
    }
    return nullptr;
}

i64 CategoryTreeBrowser::
selectedID() const noexcept {
    if (auto item = currentItem(); item) {

    }
}
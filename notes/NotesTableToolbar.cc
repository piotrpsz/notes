// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.#pragma once
//
// Created by piotr on 27.04.24.
//

#include "NotesTableToolbar.h"
#include "../common/EventController.h"
#include "../model/category.hh"
#include <QAction>
#include <QIcon>
#include <QLabel>
#include <fmt/core.h>


NotesTableToolbar::NotesTableToolbar(QWidget* const parent) :
    QToolBar(parent),
    chain_info_{new QLabel}
{
    // https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html

    setIconSize(QSize(16, 16));

    auto const add_action = new QAction(QIcon::fromTheme("list-add"), "Add");
    auto const edt_action = new QAction(QIcon::fromTheme("accessories-text-editor"), "Edit");
    auto const del_action = new QAction(QIcon::fromTheme("list-remove"), "Delete");

    add_action->setToolTip("Create new category note");
    edt_action->setToolTip("Edit selected note");
    del_action->setToolTip("Remove selected note");

    addWidget(chain_info_);

    auto spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidget(spacer);

    addAction(add_action);
    addAction(edt_action);
    addAction(del_action);

    connect(add_action, &QAction::triggered, []() {
        EventController::instance().send(event::NewNoteRequest);
    });
    connect(edt_action, &QAction::triggered, []() {
        EventController::instance().send(event::EditNoteRequest);
    });
    connect(del_action, &QAction::triggered, []() {
        EventController::instance().send(event::RemoveNoteRequest);
    });

    EventController::instance().append(this, event::CategorySelected);

    update_chain_info();
}

void NotesTableToolbar::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event *>(event);
    switch (int(e->type())) {
        case event::CategorySelected:
            if (auto data = e->data(); not data.empty()) {
                if (auto value = data[0]; value.canConvert<int>()) {
                    update_chain_info(value.toInt());
                }
            }
            break;
    }
}

std::string fmt_chain_item(std::string const& name, bool last = false) noexcept {
    if (last) return fmt::format("<b><font color=#94B88B>{}</font></b>", name);
    return fmt::format("<b><font color=#94B88B>{}</font></b> <font color=#99a3a4>{}</font> ", name, "\u22b3");
}

void NotesTableToolbar::update_chain_info(int const id) noexcept {
    std::string text{"<b><font color=#5499c7>Category:</font></b> "};

    if (id > 0) {
        if (auto chain = Category::names_chain_for(id); chain) {
            auto names = chain.value();
            auto idx_last = names.size() - 1;
            for (auto i = 0; i < idx_last; ++i)
                text += fmt_chain_item(names[i]);
            text += fmt_chain_item(names[idx_last], true);
        }
    }
    chain_info_->setText(QString::fromStdString(text));
}

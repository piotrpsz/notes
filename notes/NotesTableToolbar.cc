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
#include <QAction>
#include <QIcon>
#include <fmt/core.h>


NotesTableToolbar::NotesTableToolbar(QWidget* const parent) :
    QToolBar(parent)
{
    // https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html

    setIconSize(QSize(16, 16));

    auto const add_action = new QAction(QIcon::fromTheme("list-add"), "Add");
    auto const edt_action = new QAction(QIcon::fromTheme("accessories-text-editor"), "Edit");
    auto const del_action = new QAction(QIcon::fromTheme("list-remove"), "Delete");

    add_action->setToolTip("Create new category note");
    edt_action->setToolTip("Edit selected note");
    del_action->setToolTip("Remove selected note");

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
}

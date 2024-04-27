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

#include "EditDialog.h"
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <fmt/core.h>

EditDialog::EditDialog(QWidget* const parent) :
    QDialog(parent),
    title_{new QLineEdit},
    description_{new QLineEdit},
    content_{new QTextEdit}
{
    setWindowTitle("Note");
    setSizeGripEnabled(true);

    auto const separator{new QFrame};
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);

    auto const buttons{new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel)};
    auto const apply_button = buttons->button(QDialogButtonBox::Apply);
    connect(apply_button, &QPushButton::clicked, [this]() {
        accept();
    });
    auto const cancel_button = buttons->button(QDialogButtonBox::Cancel);
    connect(cancel_button, &QPushButton::clicked, [this]() {
        reject();
    });

    auto layout = new QGridLayout;
    layout->addWidget(new QLabel("Tilte"), 0, 0);
    layout->addWidget(title_, 0, 1);
    layout->addWidget(new QLabel("Description"), 1, 0);
    layout->addWidget(description_, 1, 1);
    layout->addWidget(separator, 2, 0, 1, 2);
    layout->addWidget(content_, 3, 0, 1, 2);
    layout->addWidget(buttons, 4, 0, 1, 2);

    setLayout(layout);

}
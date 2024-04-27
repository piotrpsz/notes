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
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QDialog>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QLineEdit;
class QTextEdit;

class EditDialog : public QDialog {
    Q_OBJECT
public:
    explicit EditDialog(QWidget* = nullptr);
    ~EditDialog() override = default;

private:
    QLineEdit* const title_;
    QLineEdit* const description_;
    QTextEdit* const content_;
};

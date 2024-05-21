//
// Created by piotr on 21.05.24.
//
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QTextEdit>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QEvent;

class Browser : public QTextEdit {
    Q_OBJECT
public:
    explicit Browser(QWidget* = nullptr);
    ~Browser() override = default;

private:
    void customEvent(QEvent*) override;
};

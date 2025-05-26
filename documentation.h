#pragma once

#include <QWidget>

namespace Ui {
class Documentation;
}

class Documentation : public QWidget
{
    Q_OBJECT

public:
    explicit Documentation(QWidget *parent = nullptr);
    ~Documentation();

private:
    Ui::Documentation *ui;
};

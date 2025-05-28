#pragma once

#include <QWidget>

namespace Ui {
class Guide;
}

///
/// \brief The Guide class - класс окна с руководством по системе
///
class Guide : public QWidget
{
    Q_OBJECT

public:
    explicit Guide(QWidget *parent = nullptr);
    ~Guide();

private:
    Ui::Guide *ui; //Указатель на форму с руководством по системе
};

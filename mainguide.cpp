#include "mainguide.h"
#include "ui_mainguide.h"

Guide::Guide(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Guide)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/new/icons/CMT.ico"));
}

Guide::~Guide()
{
    delete ui;
}

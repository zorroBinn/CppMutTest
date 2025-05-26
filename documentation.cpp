#include "documentation.h"
#include "ui_documentation.h"

Documentation::Documentation(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Documentation)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/new/icons/CMT.ico"));
}

Documentation::~Documentation()
{
    delete ui;
}

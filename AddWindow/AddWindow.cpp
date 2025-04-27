#include "AddWindow.h"
#include "ui_AddWindow.h"

import Plan;

AddWindow::AddWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddWindow)
{
    ui->setupUi(this);


    // 点击确认
    QObject::connect(ui->buttonBox, &QDialogButtonBox::accepted, [this] {





	});

    // 点击取消
    QObject::connect(ui->buttonBox, &QDialogButtonBox::rejected, [] {

	});

}

AddWindow::~AddWindow()
{
    delete ui;
}

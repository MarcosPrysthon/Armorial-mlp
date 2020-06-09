#include "mlpmainwindow.h"
#include "ui_mlpmainwindow.h"

MlpMainWindow::MlpMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MlpMainWindow)
{
    ui->setupUi(this);
}

MlpMainWindow::~MlpMainWindow()
{
    delete ui;
}

void MlpMainWindow::on_passButton_clicked()
{
    passBtnClicked = true;
}

void MlpMainWindow::on_kickButton_clicked()
{
    kickBtnClicked = true;
}

void MlpMainWindow::resetVars(){
    passBtnClicked = false;
    kickBtnClicked = false;
}

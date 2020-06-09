#ifndef MLPMAINWINDOW_H
#define MLPMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MlpMainWindow;
}

class MlpMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MlpMainWindow(QWidget *parent = nullptr);
    ~MlpMainWindow();

    bool passBtnClicked = false;
    bool kickBtnClicked = false;
    void resetVars();

private slots:

    void on_passButton_clicked();
    void on_kickButton_clicked();

private:
    Ui::MlpMainWindow *ui;
};

#endif // MLPMAINWINDOW_H

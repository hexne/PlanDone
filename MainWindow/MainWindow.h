/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2025/04/05 13:28
*******************************************************************************/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class User;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void Load();

    void Save();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<User> user_;
};


#endif //MAINWINDOW_H

/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2025/04/05 13:28
*******************************************************************************/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QThread>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QListWidget>

#include "tools.h"

class User;
class Plan;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
Q_OBJECT


signals:
    void Reminder(std::shared_ptr<Plan>);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void Load();

    void Save();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<User> user_;
    QThread listen_thread_;
    QTimer listen_timer_, click_timer_;
    QListWidgetItem* cur_click_item_;
    QSystemTrayIcon system_tray_icon_;


    void beautify_the_ui();
};


#endif //MAINWINDOW_H

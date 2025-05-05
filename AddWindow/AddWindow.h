#ifndef ADDWINDOW_H
#define ADDWINDOW_H

#include <QDialog>
#include <memory>


namespace Ui {
class AddWindow;
}

struct Plan;

class AddWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddWindow(QWidget *parent = nullptr);
    ~AddWindow();

    std::shared_ptr<Plan> plan{};
private:
    Ui::AddWindow *ui;

};

#endif // ADDWINDOW_H

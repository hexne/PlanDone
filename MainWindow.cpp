/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2025/04/05 13:28
*******************************************************************************/


#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QInputDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

struct PlanItem {
    bool flag{};
    QString plan_name{};
};


// 加载已经存在的计划
void LoadPlan() {
    QFile file("./plan.json");
    if (file.exists()) {

        QByteArray data = file.readAll();
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);

        if (error.error != QJsonParseError::NoError) {
            // json文件错误
            return;
        }
        if (!doc.isObject()) {
            // 不是json对象格式
            return;
        }

        // plans 中是json对象
        QJsonObject paln_json = doc.object();


    }
    // 如果文件不存在
    else {
        // 创建默认json文件
        // 或者什么也不做
    }
}

// 退出程序或者其他操作时报错现在的计划
void SavePlan() {

}


MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    // 设置整个列表的样式
    ui->plan_list->setStyleSheet(
    "QListWidget::item {"
    "    padding-left: 20px;"
    "    padding-right: 20px;"
    "    background: white;"          // 默认背景
    "    color: black;"              // 默认文字
    "}"

    "QListWidget::item:hover {"
    "    background: #e6f2ff;"       // 悬停背景
    "}"
    );
    LoadPlan();

    // 加载之前的计划
    // 之前的计划可以使用json保存
    QListWidgetItem *item1 = new QListWidgetItem("Item 1", ui->plan_list);
    item1->setCheckState(Qt::Unchecked);  // 设置初始状态为未选中
    // 创建窗口之后，如果有任务，应该加载并显示
    // 如果没有任务则不显示


    static auto add = new QListWidgetItem("[+]", ui->plan_list);
    add->setTextAlignment(Qt::AlignCenter);

    connect(ui->plan_list, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        // 点击的是添加按钮
        if (item == add) {
            bool ok;
            QString text = QInputDialog::getText(this, "添加新计划",
                                               "请输入计划内容:",
                                               QLineEdit::Normal,
                                               "", &ok);
            if (ok && !text.isEmpty()) {
                int insert_index = ui->plan_list->row(add);
                ui->plan_list->insertItem(insert_index, new QListWidgetItem(text));
                ui->plan_list->item(insert_index)->setCheckState(Qt::Unchecked);
            }

        }
        // 点击的是其他选项
        else {
            if (item->checkState() == Qt::Checked) {
                item->setCheckState(Qt::Unchecked);
            }
            else {
                item->setCheckState(Qt::Checked);
            }
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

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

struct Plan {
    QString plan_name{};
};

class Data {
    int y{}, m{}, d{};
    std::vector<std::shared_ptr<Plan>> done_plans;  // 每日完成的计划列表

public:
    bool operator==(const Data &other) const {
        return d == other.d && m == other.m && y == other.y;
    }

    bool find(const QString &plan_name) const {
        return std::ranges::any_of(done_plans, [&plan_name](const auto &plan) {
            return plan->plan_name == plan_name;
        });
    }

};

struct Calendar {
    std::vector<Data> calendar_;
};


QString plan_json_file_path = "./plan.json";
QJsonObject usr_plan_obj;

void LoadPlan() {
    QFile file(plan_json_file_path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.exists())
        return;

    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        throw std::runtime_error("Error parsing JSON");
    }
    usr_plan_obj = doc.object();
}

void SavePlan() {
    QFile file(plan_json_file_path);

    // 显式 open()，WriteOnly | Text 模式会覆盖原文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件：" << file.errorString();
        return;
    }

    QJsonDocument doc(usr_plan_obj);
    file.write(doc.toJson());
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
    "    font-size: 20pt;"
    "    height: 30px;"
    "}"

    "QListWidget::item:hover {"
    "    background: #e6f2ff;"       // 悬停背景
    "}"
    );

    LoadPlan();

    // 加载之前的计划
    // 之前的计划可以使用json保存
    for (const auto& key : usr_plan_obj.keys()) {
        const QJsonValue& value = usr_plan_obj.value(key);

        auto item = new QListWidgetItem(key, ui->plan_list);
        item->setCheckState(Qt::Unchecked);
    }
    static auto add = new QListWidgetItem("[+]", ui->plan_list);
    add->setTextAlignment(Qt::AlignCenter);

    connect(ui->plan_list, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        // 每次添加或者删除的时候重新保存usr_plan_json文件
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

                usr_plan_obj.insert(text, "");
                SavePlan();
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

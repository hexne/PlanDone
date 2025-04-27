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
#include <memory>
#include <iostream>
#include <QJsonArray>
#include <QString>

#include "User.h"

import Calendar;
import Time;
import Plan;


QString plan_json_file_path = "./plan.json";

QJsonObject PlanToJson(const std::shared_ptr<Plan> &plan) {
    if (!plan) {
        return QJsonObject();
    }

    QJsonObject json;
    json["plan_type"] = static_cast<int>(plan->plan_type);
    json["plan_name"] = QString::fromStdString(plan->plan_name);
    json["need_delete"] = plan->need_delete;
    json["value"] = static_cast<int>(plan->value);
    json["fixed_type"] = static_cast<int>(plan->fixed_type);
    json["start_date"] = QString::fromStdString(plan->start_date.to_string());

    return json;
}


std::shared_ptr<Plan> JsonToPlan(QJsonObject json) {

    if (json.isEmpty()) {
        return nullptr;
    }

    Plan::PlanType type = static_cast<Plan::PlanType>(json["plan_type"].toInt());
    std::shared_ptr<Plan> plan;

    switch (type) {
    case Plan::PlanType::OneTimePlan:
        plan = std::make_shared<OneTimePlan>();
        break;
    case Plan::PlanType::IntervalDaysPlan:
        plan = std::make_shared<IntervalDaysPlan>(json["value"].toInt());
        break;
    case Plan::PlanType::FixedDatePlan:
        plan = std::make_shared<FixedDatePlan>(static_cast<Plan::FixedType>(json["fixed_type"].toInt()), json["fixed_value"].toInt());
        break;
    case Plan::PlanType::DurationPlan:
        plan = std::make_shared<DurationPlan>(json["value"].toInt());
        break;
    default:
        return nullptr;
    }

    plan->plan_name = json["plan_name"].toString().toStdString();
    plan->need_delete = json["need_delete"].toBool();
    plan->value = json["value"].toInt();
    plan->fixed_type = static_cast<Plan::FixedType>(json["fixed_type"].toInt());
    plan->start_date = nl::Time(json["start_date"].toString().toStdString());

    return plan;

}


void MainWindow::Load() {
    std::vector<std::shared_ptr<Plan>> plans;
    QFile file(plan_json_file_path);
    if (!file.open(QIODevice::ReadOnly)) {
        // qWarning() << "Failed to open file:" << plan_json_file_path;
        return;
    }

    QByteArray json_data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(json_data);

    if (doc.isArray()) {
        QJsonArray jsonArray = doc.array();
        for (const QJsonValue &value : jsonArray) {
            if (value.isObject()) {
                if (auto plan = JsonToPlan(value.toObject())) {
                    plans.push_back(plan);
                }
            }
        }
    }
    user_->plans = plans;

}
void MainWindow::Save() {
    QJsonArray jsonArray;

    for (const auto& plan : user_->plans) {
        auto json = PlanToJson(plan);
        jsonArray.append(json);
    }

    QFile file(plan_json_file_path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << plan_json_file_path;
        return;
    }

    file.write(QJsonDocument(jsonArray).toJson());
}

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    user_ = std::make_shared<User>();

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

    Load();

    // 加载之前的计划
    // 之前的计划可以使用json保存
    for (const auto& plan : user_->plans) {

        auto item = new QListWidgetItem(plan->plan_name.data(), ui->plan_list);
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
                auto new_plan = std::make_shared<OneTimePlan>();
                new_plan->plan_name = text.toStdString();
                new_plan->start_date = nl::Time::now();
                user_->plans.push_back(new_plan);

                Save();
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

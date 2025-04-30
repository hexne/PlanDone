/*******************************************************************************
 * @Author : yongheng
 * @Data   : 2025/04/05 13:28
*******************************************************************************/


#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <memory>
#include <iostream>

#include <QInputDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QJsonArray>
#include <QString>
#include <QSystemTrayIcon>

#include "AddWindow/AddWindow.h"

import Calendar;
import User;
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
    json["begin_date"] = QString::fromStdString(plan->begin_date.to_string());
    json["reminder_time"] = QString::fromStdString(plan->reminder_time.to_string());

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
    plan->begin_date = nl::Time(json["begin_date"].toString().toStdString());
    plan->reminder_time = nl::Time(json["reminder_time"].toString().toStdString());

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
                    user_->plans.push_back(plan);
                }
            }
        }
    }

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

    auto icon = QIcon(QPixmap(50, 50));
    system_tray_icon_.setIcon(icon);
    system_tray_icon_.show();
    setWindowIcon(icon);



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
    for (const auto& plan : user_->get_cur_date_plans()) {

        auto item = new QListWidgetItem(plan->plan_name.data(), ui->plan_list);
        item->setCheckState(Qt::Unchecked);
    }
    static auto add = new QListWidgetItem("[+]", ui->plan_list);
    add->setTextAlignment(Qt::AlignCenter);

    connect(ui->plan_list, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        // 每次添加或者删除的时候重新保存usr_plan_json文件
        if (item == add) {
            auto add_window = new AddWindow(this);
            add_window->exec();

            // 如果没有成功添加计划, 点击了取消啊
            if (!add_window->plan) {
                std::cout << "取消" << std::endl;
                return;
            }
            int insert_index = ui->plan_list->row(add);
            ui->plan_list->insertItem(insert_index, new QListWidgetItem(add_window->plan->plan_name.c_str()));
            ui->plan_list->item(insert_index)->setCheckState(Qt::Unchecked);

            user_->plans.push_back(add_window->plan);

            Save();

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


    QObject::connect(this, &MainWindow::Reminder, this, [this](std::shared_ptr<Plan> plan) {
			if (!QSystemTrayIcon::isSystemTrayAvailable()) {
                std::cout << "abiailabel is false" << std::endl;

				return;
            }

			system_tray_icon_.showMessage(
				"任务未完成提醒",
				plan->plan_name.c_str(),
				QSystemTrayIcon::Information,
				3000
			);
        }, 
        Qt::QueuedConnection
    );

    listen_timer_.moveToThread(&listen_thread_);
    QObject::connect(&listen_thread_, &QThread::started, [this] {
			listen_timer_.start(1000);
        }
    );

    QObject::connect(&listen_timer_, &QTimer::timeout, [this] {
        nl::Time now;
        for (auto plan : user_->plans) {
            if (!plan->need_reminder(now)) 
                continue;
			emit Reminder(plan);
        }
	});

    listen_thread_.start();

}


MainWindow::~MainWindow() {
    listen_timer_.stop();
    listen_thread_.quit();
    listen_thread_.wait();
    delete ui;
}

/*******************************************************************************
 * @Author : hexne
 * @Data   : 2025/04/05 13:28
*******************************************************************************/

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <memory>
#include <QJsonObject>
#include <QString>
#include <QSystemTrayIcon>

#include "AddWindow/AddWindow.h"
#include "tools.h"
#include <QFileInfo>
#include <QJsonArray>

import Calendar;
import User;
import Time;
import Plan;

QString DefaultConfigPath = "./config.json";

void CreateDefaultConfigFile(const QString &path) {
    QJsonObject json;
    json["plans"] = "./plans.json";
    json["calendar"] = "./calendar.json";
    SaveJsonFile(json, path);
}

void MainWindow::Load() {
	QJsonObject config_json = LoadJsonFile(DefaultConfigPath);

	auto plan_path = config_json["plans"].toString();

	QJsonObject plans_json = LoadJsonFile(plan_path);
    auto current_plans = plans_json["current_plans"].toArray();
    for (auto plan : current_plans)
        user_->current_plans.push_back(CreatePlan(plan.toObject()));

    auto done_plans = plans_json["done_plans"].toArray();
    for (auto plan : done_plans)
        user_->done_plans.push_back(CreatePlan(plan.toObject()));


	auto calendar_path = config_json["calendar"].toString();
    auto calendar_json = LoadJsonFile(calendar_path);
    user_->calendar = CreateCalendar(calendar_json);

}

void MainWindow::Save() {
	QJsonObject config_json = LoadJsonFile(DefaultConfigPath);

	auto plan_path = config_json["plans"].toString();

    QJsonObject plans_json;
	QJsonArray current_plans, done_plans;

    for (const auto &plan : user_->current_plans)
        current_plans.append(plan->to_json());

    for (const auto &plan : user_->done_plans)
        done_plans.append(plan->to_json());

    plans_json["current_plans"] = current_plans;
    plans_json["done_plans"] = done_plans;
    SaveJsonFile(plans_json, plan_path);

    auto calendar_path = config_json["calendar"].toString();
    SaveJsonFile(user_->calendar.to_json(), calendar_path);
}

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    auto icon = QIcon(QPixmap(50, 50));
    system_tray_icon_.setIcon(icon);
    system_tray_icon_.show();
    setWindowIcon(icon);

    beautify_the_ui();

    QFileInfo file_info(DefaultConfigPath);
    if (!file_info.exists()) {
        CreateDefaultConfigFile(DefaultConfigPath);
    }

    user_ = std::make_shared<User>();

    Load();

    // 加载之前的计划
    // 之前的计划可以使用json保存
    for (const auto& plan : user_->get_cur_date_plans()) {
        auto item = new QListWidgetItem(plan->plan_name.data(), ui->plan_list);
        item->setCheckState(Qt::Unchecked);
    }
    static auto add = new QListWidgetItem("[+]", ui->plan_list);
    add->setTextAlignment(Qt::AlignCenter);

    // 左键点击
    connect(ui->plan_list, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        cur_click_item_ = item;
        // 每次添加或者删除的时候重新保存usr_plan_json文件
        if (item == add) {
            auto add_window = new AddWindow(this);
            add_window->exec();

            // 如果没有成功添加计划, 点击了取消啊
            if (!add_window->plan) {
                return;
            }
            int insert_index = ui->plan_list->row(add);
            ui->plan_list->insertItem(insert_index, new QListWidgetItem(add_window->plan->plan_name.c_str()));
            ui->plan_list->item(insert_index)->setCheckState(Qt::Unchecked);

            user_->current_plans.push_back(add_window->plan);

            Save();

        }
        // 点击的是其他选项, 点击了说明完成了这个计划
        else {
            click_timer_.start(200);
        }
    });
    QObject::connect(&click_timer_, &QTimer::timeout, [this]() {
        click_timer_.stop();
        

		if (cur_click_item_->checkState() == Qt::Checked) {
			cur_click_item_->setCheckState(Qt::Unchecked);
		}
		else {
			cur_click_item_->setCheckState(Qt::Checked);
		}


	});

    // 双击更改计划名称
    connect(ui->plan_list, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item) {
        cur_click_item_ = item;
        click_timer_.stop();
	});


    QObject::connect(this, &MainWindow::Reminder, this, [this](const std::shared_ptr<Plan> &plan) {
			if (!QSystemTrayIcon::isSystemTrayAvailable()) {
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
        for (auto plan : user_->current_plans) {
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

void MainWindow::beautify_the_ui() {

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
}

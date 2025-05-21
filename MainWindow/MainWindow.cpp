/*******************************************************************************
 * @Author : hexne
 * @Data   : 2025/04/05 13:28
*******************************************************************************/

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <memory>
#include <QJsonObject>
#include <QString>
#include <vector>
#include <ranges>
#include <QSystemTrayIcon>
#include <QFileInfo>
#include <QJsonArray>
#include <QPainter>
#include <QStyledItemDelegate>
#include <random>

#include "AddWindow/AddWindow.h"
#include "tools.h"

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

    auto done_plans = plans_json["finish_plans"].toArray();
    for (auto plan : done_plans)
        user_->finish_plans.push_back(CreatePlan(plan.toObject()));


	auto calendar_path = config_json["calendar"].toString();
    auto calendar_json = LoadJsonFile(calendar_path);
    user_->calendar = CreateCalendar(calendar_json);

}

void MainWindow::Save() {
	QJsonObject config_json = LoadJsonFile(DefaultConfigPath);

	auto plan_path = config_json["plans"].toString();

    QJsonObject plans_json;
	QJsonArray current_plans, done_plans;

    // 当前存在的任务
    for (const auto &plan : user_->current_plans)
        current_plans.append(plan->to_json());

    // 当前已经结束的任务
    for (const auto &plan : user_->finish_plans)
        done_plans.append(plan->to_json());

    plans_json["current_plans"] = current_plans;
    plans_json["finish_plans"] = done_plans;
    SaveJsonFile(plans_json, plan_path);

    auto calendar_path = config_json["calendar"].toString();
    SaveJsonFile(user_->calendar.to_json(), calendar_path);
}


void MainWindow::update_height() {
    setMinimumHeight(height_);
    setMaximumHeight(height_);
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
        height_ += ui->plan_list->height();
        update_height();
    }

    static auto add = new QListWidgetItem("[+]", ui->plan_list);
    add->setTextAlignment(Qt::AlignCenter);
    height_ += ui->plan_list->height();
    update_height();

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
            ui->plan_list->insertItem(insert_index, new QListWidgetItem(add_window->plan->plan_name.data()));
            ui->plan_list->item(insert_index)->setCheckState(Qt::Unchecked);
            height_ += ui->plan_list->height();
            update_height();

            user_->current_plans.push_back(add_window->plan);

            Save();

        }
        // 点击的是其他选项, 点击了说明完成了这个计划
        else {
            click_timer_.start(200);
        }
    });

    // 单击任务，暂时认为是任务被完成
    QObject::connect(&click_timer_, &QTimer::timeout, [this]() {
        click_timer_.stop();

        auto plan_name = cur_click_item_->text().toStdString();

        // 该任务今天已经完成
        auto done_plan = std::ranges::find_if(user_->current_plans, [&plan_name] (auto plan){
            return plan->plan_name == plan_name;
		});

        delete ui->plan_list->takeItem(ui->plan_list->row(cur_click_item_));
        cur_click_item_ = nullptr;

        // @TODO 该计划被完成
        user_->calendar[Time::now()].second.push_back(plan_name);
        Save();

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
        Time now;
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


std::tuple<QColor, QColor> GetRandomColor() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(150, 255);

    return {
        QColor(dis(gen), dis(gen), dis(gen)),
        QColor(dis(gen), dis(gen), dis(gen))
    };
}

class RainbowDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        const QString originalText = opt.text;
        opt.text.clear();

        // 强制禁用状态颜色，确保基类不会使用默认文本色
        if (opt.state & QStyle::State_Selected)
            opt.palette.setColor(QPalette::Text, Qt::transparent);
        if (opt.state & QStyle::State_HasFocus)
            opt.palette.setColor(QPalette::Text, Qt::transparent);

        QStyledItemDelegate::paint(painter, opt, index);

        painter->save();

        // 获取原始文本区域（已包含样式表的内边距）
        const QRect textRect = opt.widget->style()->subElementRect(
            QStyle::SE_ItemViewItemText, &opt, opt.widget
        );

        // 创建渐变（调整为从文本区域左侧开始）
        QLinearGradient gradient(
            textRect.left(), textRect.center().y(),
            textRect.right(), textRect.center().y()
        );
        auto [color1, color2] = GetRandomColor();
        auto [color3, color4] = GetRandomColor();
        auto [color5, color6] = GetRandomColor();

        gradient.setColorAt(0.0, color1);
        gradient.setColorAt(0.2, color2);
        gradient.setColorAt(0.4, color3);
        gradient.setColorAt(0.6, color4);
        gradient.setColorAt(0.8, color5);
        gradient.setColorAt(1.0, color6);

        // 设置绘制参数
        painter->setFont(opt.font);
        painter->setPen(QPen(gradient, 1));

        painter->drawText(
            textRect.adjusted(3, 0, 0, 0),
            opt.displayAlignment | Qt::AlignVCenter,
            originalText
        );

        painter->restore();
    }
};


void MainWindow::beautify_the_ui() {

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::WindowStaysOnBottomHint | Qt::FramelessWindowHint);

    ui->plan_list->setItemDelegate(new RainbowDelegate(ui->plan_list));

    ui->plan_list->setStyleSheet(
        "QListWidget {"
        "    background: transparent;"
        "    border: none;"
        "}"
        "QListWidget::item {"
        "    padding-left: 10px;"  // 减小左侧内边距
        "    padding-right: 10px;"
        "    background: transparent;"
        "    font-size: 20pt;"
        "    height: 30px;"
        "}"
    );


}

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
#include <ranges>
#include <chrono>
#include <iostream>

struct Plan {
    QString plan_name{};
};

struct Date {
    int year{}, month{}, day{};
    std::vector<std::shared_ptr<Plan>> done_plans;  // 每日完成的计划列表

    Date(int y, int m, int d) : year(y), month(m), day(d) {  }
    Date(std::chrono::time_point<std::chrono::system_clock> time_point) {
        auto ymd = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(time_point)};
        year = ymd.year().operator int();
        month = ymd.month().operator unsigned();
        day = ymd.day().operator unsigned();
    }
    bool operator==(const Date &other) const {
        return day == other.day && month == other.month && year == other.year;
    }

    bool find(const QString &plan_name) const {
        return std::ranges::any_of(done_plans, [&plan_name](const auto &plan) {
            return plan->plan_name == plan_name;
        });
    }

    static int count_month_day(int year, int month) {
        if (month == 2) {
            return (year % 400 == 0) || (year % 100 != 0 && year % 4 == 0) ? 29 : 28;
        }
        constexpr int days[] = {0, 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        return days[month];
    }
};

class Calendar {
    using Month = std::vector<Date>;
    using Year = std::vector<Month>;
    std::vector<Year> calendar_;
public:
    Date &find(const Date &date) {
        for (auto &day : calendar_ | std::ranges::views::join | std::ranges::views::join) {
            if (date == day)
                return day;
        }
        throw std::runtime_error("This Day found");
    }


    Calendar() {
        Date end_time = std::chrono::system_clock::now();

        for (int year = 2025; year <= end_time.year; year++) {
            Year push_year;
            for (int month = 1; month < 12; month++) {
                Month push_month;
                if (year == end_time.year && month > end_time.month)
                    break;
                for (int day = 1; day <= Date::count_month_day(year, month); day++) {
                    if (year == end_time.year && month == end_time.month && day > end_time.day)
                        break;
                    Date date {year, month, day};
                    push_month.push_back(date);
                }
                push_year.push_back(push_month);
            }
            calendar_.push_back(push_year);
        }
    }

    void print() {

        for (const auto &day : calendar_ | std::ranges::views::join | std::ranges::views::join) {
            std::cout << day.year << " " << day.month << " " << day.day << std::endl;
        }

    }

};

class User {
    bool is_local_ = true;
    size_t id_;
    Calendar calendar_;
public:
    bool is_local() const {
        return is_local_;
    }



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

    Calendar calendar;
    calendar.print();


}

MainWindow::~MainWindow() {
    delete ui;
}

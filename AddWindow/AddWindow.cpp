
#include "AddWindow.h"
#include "ui_AddWindow.h"

#include <iostream>

import Time;
import Plan;

size_t ToDay(size_t val, const Plan::FixedType type) {
    if (type == Plan::FixedType::Day) {
        return val;
    }
    else if (type == Plan::FixedType::Week) {
        return val * 7;
    }
    else if (type == Plan::FixedType::Month) {
        return val * 30;
    }
    else if (type == Plan::FixedType::Year) {
        return val * 365;
    }
    return 0;
}


AddWindow::AddWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddWindow)
{
    ui->setupUi(this);


    ui->type->addItems({ "一次性计划", "循环计划", "特定日期计划", "持续计划" });
    ui->begin_date->setDate(QDate::currentDate());
	ui->reminder_time->setTime(QTime(18, 0, 0));
	ui->value->setEnabled(false);
    ui->value_type->addItems({ "天", "周" , "月", "年" });
	ui->value_type->setEnabled(false);


    QObject::connect(ui->type, &QComboBox::currentIndexChanged, [this] (int index) {
        if (index == static_cast<int>(Plan::PlanType::OneTimePlan)) {
            ui->value->setEnabled(false);
            ui->value_type->setEnabled(false);
        }

        // 对于特定时间的循环一次的计划，value 处传入 x (单位) : 1 天
        else if (index == static_cast<int>(Plan::PlanType::IntervalDaysPlan)) {
			ui->value->setEnabled(true);
			ui->value_type->setEnabled(true);
            ui->value->setValue(1);
	    }
        // 对于特定时间触发的计划，传入 x (单位) ：10 日，4 周 , 2 月 （每月10 号，每月第四周，每年二月均触发）
        else if (index == static_cast<int>(Plan::PlanType::FixedDatePlan)) {
			ui->value->setEnabled(true);
			ui->value_type->setEnabled(true);
        }
        // 对于持续特定时间的计划， 传入 x （单位）： 10 天， 1 年
        else if (index == static_cast<int>(Plan::PlanType::DurationPlan)) {

			ui->value->setEnabled(true);
			ui->value_type->setEnabled(true);
        }
        else {

        }


	});

    // 点击确认
    QObject::connect(ui->buttonBox, &QDialogButtonBox::accepted, [this] {

        auto type_index = ui->type->currentIndex();
        if (type_index == static_cast<int>(Plan::PlanType::OneTimePlan)) {
            plan = std::make_shared<OneTimePlan>();
        }
        else if (type_index == static_cast<int>(Plan::PlanType::IntervalDaysPlan)) {
            plan = std::make_shared<IntervalDaysPlan>(ToDay(ui->value->value(), static_cast<Plan::FixedType>(ui->value_type->currentIndex())));
        }
        else if (type_index == static_cast<int>(Plan::PlanType::FixedDatePlan)) {
            plan = std::make_shared<FixedDatePlan>(static_cast<Plan::FixedType>(ui->value_type->currentIndex()), ui->value->value());
        }
        else if (type_index == static_cast<int>(Plan::PlanType::DurationPlan)) {
            plan = std::make_shared<DurationPlan>(ToDay(ui->value->value(), static_cast<Plan::FixedType>(ui->value_type->currentIndex())));
        }
        else {
            ;
        }

        plan->begin_date = Time::FromDate(ui->begin_date->date().toString("yyyy/MM/dd").toStdString());
        plan->reminder_time = Time::FromTime(ui->reminder_time->time().toString("HH:mm:ss").toStdString());

        plan->plan_name = ui->name->text().toStdString();

	});

}

AddWindow::~AddWindow()
{
    delete ui;
}

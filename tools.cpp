#include "tools.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

import Plan;
import Calendar;


QJsonObject LoadJsonFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file for reading:" << path;
        return QJsonObject();
    }

    QByteArray data = file.readAll();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (doc.isNull()) 
        throw std::runtime_error(std::format("Json parse error at offset : {}" , error.offset));

    if (!doc.isObject()) 
        throw std::runtime_error("JSON document is not an object");

    return doc.object();
}

void SaveJsonFile(const QJsonObject& obj, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) 
		throw std::runtime_error("Could not open file for writing");

    QJsonDocument doc(obj);
    file.write(doc.toJson(QJsonDocument::Indented));
}


std::shared_ptr<Plan> CreatePlan(const QJsonObject &json) {

        if (json.isEmpty())
            return {};

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




QJsonObject CalendarToJson(Calendar &calendar) {
    QJsonObject json;
    for (auto [date, plans] : calendar.to_iterator()) {
        QJsonArray plan_array;
        for (auto plan : plans) {
            plan_array.append(plan->plan_name.c_str());
        }
        json[date.to_date_string().c_str()] = plan_array;
    }
    return json;
}

// Calendar JsonToCalendar(QJsonObject& json);



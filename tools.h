//
// QT界面类中不能直接使用模块，此头文件提供了二者的交互函数
// 


#pragma once
#include <QJsonObject>


class Plan;
class User;
class Calendar;


QJsonObject LoadJsonFile(const QString& path);
void SaveJsonFile(const QJsonObject&, const QString& path);
void SaveJsonFile(const QJsonDocument& obj, const QString& path);
//QJsonObject CalendarToJson(Calendar &calendar);
//Calendar JsonToCalendar(QJsonObject &json);
//
//std::shared_ptr<User> LoadConfig(const QString& path);
//void SaveConfig(const std::shared_ptr<User> &user);

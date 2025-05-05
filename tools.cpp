#include "tools.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

import Plan;
import Calendar;
import Time;


// 加载配置文件时，如果文件不存在，返回空配置
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

    if (!(doc.isObject() || doc.isArray()))
        throw std::runtime_error("JSON document is not an object");

    return doc.object();
}

void SaveJsonFile(const QJsonDocument& obj, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        throw std::runtime_error("Could not open file for writing");

    file.write(QJsonDocument(obj).toJson());
}

void SaveJsonFile(const QJsonObject& obj, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) 
		throw std::runtime_error("Could not open file for writing");

    QJsonDocument doc(obj);
    SaveJsonFile(doc, path);
}

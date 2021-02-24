#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QList>
#include <sstream>

class Message {
public:
    Message(const QString& id, const int value, const QString& time, const bool valid)
        : m_id(id), m_value(value), m_time(time), m_valid(valid) {}

    QString m_id;
    int m_value;
    QString m_time;
    bool m_valid;
};


class DataBase
{
public:
    DataBase(const QString &dbName, const QString &dbFile);
    ~DataBase();
    void GetAllMessage(QList<Message>& list);
    bool SetDataBaseData(Message& msg);
    bool UpdateDB();
    bool DelAllData();

    const QString &Error() const { return m_error; }

private:
    bool linkSQLite(const QString &dbName, const QString &dbFile);  //初始化数据库连接
    void setErr(const QString& val);
private:
    QSqlDatabase db;
    QString m_error;
};

#endif // DATABASE_H

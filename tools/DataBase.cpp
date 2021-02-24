#include "DataBase.h"

#include "QDebug"

DataBase::DataBase(const QString &dbName, const QString &dbFile)
{
    // 初始化数据库连接
    linkSQLite(dbName,dbFile);

    db = QSqlDatabase::database(dbName);
    if(!db.open()){
        qDebug() << "MenuTestResult db didnt open!";
        return;
    }
    qDebug() << "MenuTestResult db open!";
}

DataBase::~DataBase()
{
    db.close();
}

void DataBase::GetAllMessage(QList<Message> &list)
{
    QSqlQuery query(db);
    QString sqlSelect;
    sqlSelect = QString("select * from j_random_table");
    if(!query.exec(sqlSelect))
    {
        qDebug()<< "query didnt exec!" << query.lastError();
        return;
    }
    bool isDel;
    int num = 0;
    while(query.next())
    {
        isDel = query.value(3).toInt();
        Message message(query.value(0).toString(),
                        query.value(1).toInt(),
                        query.value(2).toString(),
                        query.value(3).toBool());
        list.push_back(message);

        num++;
    }
    qDebug()<<"database data_count="<<num << list.size();
}

bool DataBase::SetDataBaseData(Message& message)
{
    QSqlQuery query(db);
    QString sqlSelect;
    sqlSelect = QString("insert into j_random_table(j_id,j_value,j_time,j_valid) values(\"%1\",%2,\"%3\",1)"
                        ).arg(message.m_id).arg(message.m_value).arg(message.m_time);
    qDebug() << sqlSelect;
    if(!query.exec(sqlSelect))
    {
        qDebug()<< "insert query didn't exec!" << query.lastError();
        m_error = "insert query didn't exec! err: " + query.lastError().text();
        return false;
    }
    return true;
}

bool DataBase::UpdateDB()
{
    QSqlQuery query(db);
    QString sqlSelect;
    sqlSelect = QString("update j_random_table set j_valid=0");
    if(!query.exec(sqlSelect))
    {
        qDebug()<< "update query didn't exec!" << query.lastError();
        m_error = "update query didn't exec! err: " + query.lastError().text();
        return false;
    }
    return true;
}

bool DataBase::DelAllData()
{
    QSqlQuery query(db);
    QString sqlSelect;
    sqlSelect = QString("delete from j_random_table");
    if(!query.exec(sqlSelect))
    {
        qDebug()<< "delete query didn't exec!" << query.lastError();
        m_error = "delete query didn't exec! err: " + query.lastError().text();
        return false;
    }
    return true;
}

bool DataBase::linkSQLite(const QString &dbName, const QString &dbFile)
{
    if(!QFileInfo::exists(dbFile)){
        qDebug() << QString("%1 db didn't exists").arg(dbFile);
        return false;
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(dbFile);
    if(!db.open()){
        qDebug() << "db didn't open!" << db.lastError();
        m_error = "db didn't open! err: " + db.lastError().text();
        return false;
    }
    qDebug()<<"db open success";
    return true;
}

void DataBase::setErr(const QString &val)
{
    m_error = val;
}



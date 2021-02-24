#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tools/DataBase.h"

#include <QMainWindow>
#include <QMap>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void InitWidget();
    void InitTableWidget();
    QTableWidgetItem* InitWidgetItem(const QString& text);
    void InitDB();

    void reflash();
    int getRandomNum(QList<int>& randomList);

private slots:
    void on_generateBtn_clicked();
    void on_delBtn_clicked();

private:
    Ui::MainWindow *ui;

    QList<Message> m_list;
    DataBase* m_db;
};
#endif // MAINWINDOW_H

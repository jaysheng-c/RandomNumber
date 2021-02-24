#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTableWidgetItem>
#include <QStringList>
#include <random>
#include <QMap>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>

class GMap {
public:
    using Iterator = QMap<int, bool>::iterator;

    GMap(const int count = 100);
    void setMapUsed(const QList<Message>& list);
    void setMapUsed(const int key);
    void reset();
    Iterator find(const int key);
    void setValue(const int key, const bool val);

    int getUsedCount() { return m_used; }
    int size() { return m_map.size(); }

private:
    QMap<int, bool> m_map;
    int m_used; // 可用数量
};

GMap::GMap(const int count)
    : m_used(count){
    for(int i = 1; i <= count; ++i) {
        m_map.insert(i,false);
    }
}

void GMap::setMapUsed(const QList<Message>& list) {
    for(auto msg : list) {
        if(msg.m_valid &&
                m_map.find(msg.m_value) != m_map.end()) {
            m_map[msg.m_value] = true;
            m_used--;
        }
    }
}
void GMap::setMapUsed(const int key) {
    if(m_used <= 0 ||
            m_map.find(key) == m_map.end()) {
        return;
    }
    m_map[key] = true;
    m_used--;
}

void GMap::reset() {
    for(auto it = m_map.begin(); it != m_map.end(); ++it) {
        m_map[it.key()] = false;
    }
    m_used = m_map.size();
}

GMap::Iterator GMap::find(const int key) {
    return m_map.find(key);
}

void GMap::setValue(const int key, const bool val) {
    m_map[key] = val;
}

// static

static int count = 100;
static GMap g_map(count);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitDB();
    InitWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitWidget()
{
    this->setWindowTitle("RandomNumber Version 1.0");
    ui->numLabel->setText("");
    ui->pushButton->hide();
    ui->delBtn->setText("删除历史");
    InitTableWidget();
    reflash();
}

void MainWindow::InitTableWidget()
{
    QStringList hearderText;
    hearderText << "Id";
    hearderText << "Value";
    hearderText << "Date";
    hearderText << "Valid";

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    // 不可编辑
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);   // 整行选中
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);      // 不可选中
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setColumnCount(hearderText.size());
    ui->tableWidget->setHorizontalHeaderLabels(hearderText);
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{font:13pt '宋体';color: black;};");
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->resizeRowsToContents();        // 设置自动行

    int width = ui->tableWidget->width() / hearderText.size();
    for(int i = 0; i < hearderText.size(); ++i) {
        ui->tableWidget->setColumnWidth(i, width);
    }
}

QTableWidgetItem *MainWindow::InitWidgetItem(const QString& text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}

void MainWindow::InitDB()
{
    m_db = new DataBase("Jaysheng_DB", "./jaysheng.db");
    m_db->GetAllMessage(m_list);
    g_map.setMapUsed(m_list);
}

void MainWindow::reflash()
{
    int last_index = ui->tableWidget->rowCount();
    // qDebug() << "reflash1 " << last_index << m_list.size();
    ui->tableWidget->setRowCount(m_list.size());
    for(int i = last_index; i < m_list.size(); ++i) {
        ui->tableWidget->setItem(i, 0, InitWidgetItem(m_list.at(i).m_id));
        ui->tableWidget->setItem(i, 1, InitWidgetItem(QString("%1").arg(m_list.at(i).m_value)));
        ui->tableWidget->setItem(i, 2, InitWidgetItem(m_list.at(i).m_time));
        QString valid = "invalid";
        if(m_list.at(i).m_valid) {
            valid = "valid";
        }
        ui->tableWidget->setItem(i, 3, InitWidgetItem(valid));
        // qDebug() << "reflash";
    }
}

template <class T>
bool repeat(QList<T> &randomList, T val) {
    for(auto v : randomList) {
        if(v == val) {
            return true;
        }
    }
    return false;
}

int MainWindow::getRandomNum(QList<int> &randomList) {
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    int value = qrand() % (g_map.size()+1);
    while(!value && repeat<int>(randomList, value)) {
        value = qrand() % (g_map.size()+1);
    }
    return value;
}

void MainWindow::on_generateBtn_clicked()
{
    // 判断今天是否生成随机数
    QString last_time = "";
    if(m_list.size()) {
        last_time = (m_list.end()-1)->m_time;
    }
    QDateTime cur = QDateTime::currentDateTime();
    QString now = cur.toString("yyyyMMdd");
    if(now == last_time) {
        QMessageBox::information(this, "提示", "今天已经生成过随机数");
        return;
    }

    // 开始生成随机数
    QList<int> randomList;
    int value = getRandomNum(randomList);
    auto it = g_map.find(value);
    // 判断是否重复
    while(it.value() && g_map.getUsedCount() > 0) {
        randomList.push_back(it.key());
        value = getRandomNum(randomList);
        it = g_map.find(value);
        qDebug() << "in while" << value;
    }
    if(g_map.getUsedCount() <= 0) {
        // 重新开始新的一轮
        qDebug() << "\n\n    new loop\n\n";
        QMessageBox::information(this, "Tips", "start new loop");
        m_db->UpdateDB();
        m_list.clear();
        m_db->GetAllMessage(m_list);
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);
        g_map.reset();
    }

    // 确定值
    g_map.setMapUsed(value);
    ui->numLabel->setText(QString("%1").arg(value));

    QString id = QString("%1").arg(m_list.size());
    Message msg(id, value, now, true);
    if(!m_db->SetDataBaseData(msg)) {
        QMessageBox::information(this, "Error", m_db->Error());
    }
    m_list.push_back(msg);
    reflash();
}

void MainWindow::on_delBtn_clicked()
{
    if(!m_db->DelAllData()) {
        QMessageBox::information(this, "Error", m_db->Error());
        return;
    }
    m_list.clear();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    g_map.reset();
    reflash();
}

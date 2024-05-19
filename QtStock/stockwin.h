#ifndef STOCKWIN_H
#define STOCKWIN_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>

#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StockWin; }
QT_END_NAMESPACE

class StockIndex // 索引
{
public:
    QString code; // 股票代码
    QString month; // 年月
    qint64 pos; // 偏移量
};

class StockData  // 股票数据
{
public:
    QString ts_code; // 股票代码
    QDateTime trade_date; // 交易日期
    QString date; // QString类型的交易时间
    double open; // 开盘价
    double high; // 最高价
    double low; // 最低价
    double close; // 收盘价
    double pre_close; // 临近收盘价
    double change; // 涨跌幅
    double pct_chg; // 涨跌幅百分比
    double vol; // 交易量
    double amount; // 交易金额
    double IR; // 隔天收益率
};

class StockWin : public QMainWindow
{
    Q_OBJECT

public:
    StockWin(QWidget *parent = nullptr);
    ~StockWin();
    Ui::StockWin* getUi() { return ui; }
    QString getStockFileName() { return StockFileName; }
    QString getIndexFileName() { return IndexFileName; }
    bool isFilesReady();
    bool isCurrentDataReady();
    void RefreshStatusTextBrowser();

private slots:
    // 文件Action
    void actionOpenStockFileSlot();
    void actionOpenAndSortStockFileSlot();
    // 索引Action
    void actionOpenIndexFileSlot();
    void actionCreateIndexFileSlot();
    // K线图和柱状图可视化
    void on_CreateFinancialChartButton_clicked();
    // DockWidget
    void on_ConfirmButton_clicked();
    void on_ClearButton_clicked();
    void on_ConsoleStockCodeComboBox_currentTextChanged(const QString &arg1);
    void on_ConsoleStockCodeComboBox_currentIndexChanged(const QString &arg1);
    // 相关系数热力图
    void on_MapConfirmButton_clicked();
    void on_MapClearButton_clicked();
    // 模拟股票价格变动
    void on_CreateSimulationGraphButton_clicked();

private:
    Ui::StockWin *ui;
    QString StockFileName; // 打开的股票文件的路径
    QString IndexFileName; // 打开的索引文件的路径

    QString CurrentStockCode; // 当前股票代码
    QString CurrentMonth;  // 当前年月

    QVector<StockIndex> IndexVector; // 内存中的索引文件
    QVector<StockData> StockDataVector; // 内存中的股票数据文件

    QCPAxisRect *volumeAxisRect;
};

#endif // STOCKWIN_H

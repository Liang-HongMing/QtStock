#include "stockwin.h"
#include "ui_stockwin.h"

#include "externalsort.h" // 外排序的头文件
#include "index.h"  // 创建索引文件的头文件
#include "financialchart.h" // K线图和柱状图的头文件
#include "pearsoncorrelationmap.h" // 热力图的头文件
#include "simulationgraph.h" // 模拟价格变动的头文件

StockWin::StockWin(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::StockWin)
{
    ui->setupUi(this);

    // 文件的Action
    connect(ui->actionOpenStockFile, &QAction::triggered, this, &StockWin::actionOpenStockFileSlot);
    connect(ui->actionOpenAndSortStockFile, &QAction::triggered, this, &StockWin::actionOpenAndSortStockFileSlot);
    // 索引的Action
    connect(ui->actionOpenIndexFile, &QAction::triggered, this, &StockWin::actionOpenIndexFileSlot);
    connect(ui->actionCreateIndexFile, &QAction::triggered, this, &StockWin::actionCreateIndexFileSlot);

    // 设置股票数据可视化中的交易量柱状图表 // 吐槽：搞了起码七八个小时，原来放在构造函数就完事了
    volumeAxisRect = new QCPAxisRect(ui->FinancialChart);
    ui->FinancialChart->plotLayout()->addElement(1, 0, volumeAxisRect);

    // Debug用
//    StockFileName = "C:\\hm.liang\\QtProjects\\Files\\output.csv";
//    IndexFileName = "C:\\hm.liang\\QtProjects\\Files\\index.txt";
//    CurrentStockCode = "000833.SZ";
//    CurrentMonth = "200210";
//    LoadStockIndex(IndexVector, IndexFileName);
//    LoadStockFile(StockDataVector, StockFileName, SearchData(IndexVector, CurrentStockCode, CurrentMonth));
//    SetMapComboBox(ui, IndexVector);
//    SetConsoleStockCodeComboBox(ui, IndexVector);
    // 刷新状态框
    RefreshStatusTextBrowser();
}

StockWin::~StockWin()
{
    delete ui;
}

bool StockWin::isFilesReady()
{
    bool status = true;
    if (StockFileName.isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr("未打开股票数据文件"));
        status = false;
    }
    else if (IndexFileName.isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr("未打开索引文件"));
        status = false;
    }
    return status;
}

bool StockWin::isCurrentDataReady()
{
    bool status = true;
    if (CurrentStockCode.isEmpty() || CurrentMonth.isEmpty())
    {
        QMessageBox::information(this,tr("提示"),tr("未输入股票数据文件"));
        status = false;
    }
    return status;
}

void StockWin::RefreshStatusTextBrowser()
{
    QString Status;

    QString StockFileNameStatus = "股票数据文件：";
    if (StockFileName.isEmpty())
        StockFileNameStatus += "未选择股票数据文件";
    else
        StockFileNameStatus += StockFileName;

    QString IndexFileNameStatus = "索引文件：";
    if (IndexFileName.isEmpty())
        IndexFileNameStatus += "未选择索引文件";
    else
        IndexFileNameStatus += IndexFileName;

    QString CrurrentStockCodeStatus = "股票代码：";
    if (CurrentStockCode.isEmpty())
        CrurrentStockCodeStatus += "未输入股票代码";
    else
        CrurrentStockCodeStatus += CurrentStockCode;

    QString CrurrentMonthStatus = "年月：";
    if (CurrentMonth.isEmpty())
        CrurrentMonthStatus += "未输入年月";
    else
        CrurrentMonthStatus += CurrentMonth;

    Status += StockFileNameStatus + '\n'
            + IndexFileNameStatus + '\n'
            + '\n'
            + CrurrentStockCodeStatus + '\n'
            + CrurrentMonthStatus + '\n';

    ui->StatusTextBrowser->setText(Status);
    qDebug() << "RefreshStatusTextBrowser: Done";
}

void StockWin::actionOpenStockFileSlot()
{
    QString OpenFileName =
        QFileDialog::getOpenFileName(this,
                                     tr("打开股票数据文件"),
                                     QCoreApplication::applicationFilePath(),
                                     tr("csv files (*.csv);;Text files (*.txt)"));
    if (OpenFileName.isEmpty())
    {
        QMessageBox::information(this, tr("提示"), tr("未选择文件"));
        return;
    }
    StockFileName = OpenFileName;
    RefreshStatusTextBrowser();
}

void StockWin::actionOpenAndSortStockFileSlot()
{
    QString OpenFileName =
        QFileDialog::getOpenFileName(this,
                                     tr("打开股票数据文件并排序"),
                                     QCoreApplication::applicationFilePath(),
                                     tr("csv files (*.csv);;Text files (*.txt)"));
    if (OpenFileName.isEmpty())
    {
        QMessageBox::information(this, tr("提示"), tr("未选择文件"));
        return;
    }
    QString SaveFileName =
        QFileDialog::getSaveFileName(this,
                                     tr("保存排序后的股票数据文件"),
                                     QCoreApplication::applicationFilePath(),
                                     tr("Text files (*.txt);;csv files (*.csv)"));
    if (SaveFileName.isEmpty())
    {
        QMessageBox::information(this, tr("提示"), tr("未选择保存路径，请重新打开文件"));
        return;
    }
    // 更新状态栏
    StockFileName = "正在对股票数据文件进行外排序排序，排序结束前请勿进行任何操作";
    RefreshStatusTextBrowser();
    // 外排序
    // 分割文件
    QVector<QString> SplitedFileVector;
    SplitFile(SaveFileName, OpenFileName, SplitedFileVector);
    // 对每个文件排好序
    QVector<QString> SortedFileVector;
    InternalSort(SplitedFileVector, SortedFileVector);
    // 归并
    if (SortedFileVector.size() <= 1)
    {
        QFile OutputFile(SortedFileVector[0]);
        QFileInfo SortedFileInfo(SortedFileVector[0]);
        SaveFileName = SortedFileInfo.absolutePath() + "/output.csv";
        OutputFile.rename(SaveFileName);
    }
    else
    {
        SaveFileName = TwoWayMergeSort(SortedFileVector[0], SortedFileVector[1]);
        for (int i = 2; i < SortedFileVector.size(); ++i)
        {
            SaveFileName = TwoWayMergeSort(SaveFileName, SortedFileVector[i]);
        }
    }
    StockFileName = SaveFileName;
    QMessageBox::information(this, tr("提示"), tr("排序已完成"));
    RefreshStatusTextBrowser();
    qDebug() << "actionOpenAndSortStockFileSlot: Done";
}

void StockWin::actionOpenIndexFileSlot()
{
    if (StockFileName.isEmpty())
    {
        QMessageBox::information(this, tr("提示"), tr("未找到股票数据文件,\n请打开股票数据文件"));
        return;
    }
    QString OpenFileName =
        QFileDialog::getOpenFileName(this,
                                     tr("打开索引文件"),
                                     QCoreApplication::applicationFilePath(),
                                     tr("Text files (*.txt);;csv files (*.csv)"));
    if (OpenFileName.isEmpty())
    {
        QMessageBox::information(this, tr("提示"), tr("未选择文件"));
        return;
    }
    IndexFileName = OpenFileName;
    LoadStockIndex(IndexVector, IndexFileName);

    SetMapComboBox(ui, IndexVector);
    SetConsoleStockCodeComboBox(ui, IndexVector);

    QMessageBox::information(this, tr("提示"), tr("索引文件已被读入内存"));

    RefreshStatusTextBrowser();
}

void StockWin::actionCreateIndexFileSlot()
{
    if (StockFileName.isEmpty())
    {
        QMessageBox::information(this, tr("提示"), tr("未找到股票数据文件,\n请打开股票数据文件"));
        return;
    }
    QString SaveFileName =
        QFileDialog::getSaveFileName(this,
                                     tr("选择索引文件保存位置"),
                                     QCoreApplication::applicationFilePath(),
                                     tr("Text files (*.txt)"));
    if (SaveFileName.isEmpty())
    {
        QMessageBox::information(this, tr("提示"), tr("未选择创建文件的位置"));
        return;
    }
    // 更新状态栏
    IndexFileName = "正在对股票数据文件创建索引文件，创建索引文件结束前结束前请勿进行任何操作";
    RefreshStatusTextBrowser();
    CreateIndexFile(StockFileName, SaveFileName); // 调用创建索引文件的函数
    IndexFileName = SaveFileName;                 // 保存索引文件的路径
    // 把索引载入内存
    LoadStockIndex(IndexVector, IndexFileName);

    SetMapComboBox(ui, IndexVector);
    SetConsoleStockCodeComboBox(ui, IndexVector);

    QMessageBox::information(this, tr("提示"), tr("索引文件创建完成，并且已被读入内存"));


    RefreshStatusTextBrowser();
}

void StockWin::on_ConfirmButton_clicked()
{
    if (!isFilesReady()) return;
    QString TempStockCode;
    QString TempMonth;
    if (!ui->ConsoleStockCodeComboBox->currentText().isEmpty())
        TempStockCode = ui->ConsoleStockCodeComboBox->currentText();
    else
        TempStockCode = CurrentStockCode;
    if (!ui->ConsoleMonthComboBox->currentText().isEmpty())
        TempMonth = ui->ConsoleMonthComboBox->currentText();
    else
        TempMonth = CurrentMonth;

    // 把指定股票以及指定的月份载入内存
    qint64 startpos = SearchData(IndexVector, TempStockCode, TempMonth);
    if (startpos == -1)
    {
        QMessageBox::information(this, tr("提示"), "未找到股票代码"+TempStockCode);
    }
    else if (startpos == -2)
    {
        QMessageBox::information(this, tr("提示"), TempStockCode+"该股票代码不存在"+TempMonth+"时间的数据");
    }
    else
    {
        CurrentStockCode = TempStockCode;
        CurrentMonth = TempMonth;
        LoadStockFile(StockDataVector, StockFileName, startpos);
        QMessageBox::information(this,tr("提示"),tr("股票代码和月份选择完毕\n对应月份的股票数据已被读入内存"));
    }
    RefreshStatusTextBrowser();
}

void StockWin::on_ClearButton_clicked()
{
    ui->ConsoleMonthComboBox->setCurrentIndex(-1);
    ui->ConsoleStockCodeComboBox->setCurrentIndex(-1);
}

void StockWin::on_CreateFinancialChartButton_clicked()
{
    if (!isFilesReady()) return;
    if (!isCurrentDataReady()) return;
    SetFinancialChart(StockDataVector, ui->FinancialChart, volumeAxisRect);
}

void SetConsoleStockCodeComboBox(Ui::StockWin *ui, QVector<StockIndex> &IndexVector)
{
    QStringList StockCodeList;
    StockCodeList.append(IndexVector[0].code);
    for (int i = 1; i < IndexVector.size(); ++i)
    {
        if (IndexVector[i].code != IndexVector[i-1].code)
        StockCodeList.append(IndexVector[i].code);
    }
    ui->ConsoleStockCodeComboBox->clear();
    ui->ConsoleStockCodeComboBox->addItems(StockCodeList);
}

void SetMapComboBox(Ui::StockWin* ui, QVector<StockIndex> &IndexVector)
{
    QStringList StockCodeList;
    StockCodeList.append(IndexVector[0].code);
    for (int i = 1; i < IndexVector.size(); ++i)
    {
        if (IndexVector[i].code != IndexVector[i-1].code)
        StockCodeList.append(IndexVector[i].code);
    }
    ui->ComboBox1->clear();
    ui->ComboBox2->clear();
    ui->ComboBox3->clear();
    ui->ComboBox4->clear();
    ui->ComboBox5->clear();
    ui->ComboBox6->clear();
    ui->ComboBox7->clear();
    ui->ComboBox8->clear();
    ui->ComboBox9->clear();
    ui->ComboBox10->clear();
    ui->ComboBox1->addItems(StockCodeList);
    ui->ComboBox2->addItems(StockCodeList);
    ui->ComboBox3->addItems(StockCodeList);
    ui->ComboBox4->addItems(StockCodeList);
    ui->ComboBox5->addItems(StockCodeList);
    ui->ComboBox6->addItems(StockCodeList);
    ui->ComboBox7->addItems(StockCodeList);
    ui->ComboBox8->addItems(StockCodeList);
    ui->ComboBox9->addItems(StockCodeList);
    ui->ComboBox10->addItems(StockCodeList);
}

void StockWin::on_ConsoleStockCodeComboBox_currentIndexChanged(const QString &arg1)
{
    int size = IndexVector.size();
    QStringList MonthList;
    for (int i = 0; i < size; ++i)
    {
        if (IndexVector[i].code == arg1)
            MonthList << IndexVector[i].month;
    }
    ui->ConsoleMonthComboBox->clear();
    ui->ConsoleMonthComboBox->addItems(MonthList);
}

void StockWin::on_ConsoleStockCodeComboBox_currentTextChanged(const QString &arg1)
{
    int size = IndexVector.size();
    QStringList MonthList;
    for (int i = 0; i < size; ++i)
    {
        if (IndexVector[i].code == arg1)
            MonthList << IndexVector[i].month;
    }
    ui->ConsoleMonthComboBox->clear();
    ui->ConsoleMonthComboBox->addItems(MonthList);
}

void StockWin::on_MapClearButton_clicked()
{
    ui->ComboBox1->setCurrentIndex(-1);
    ui->ComboBox2->setCurrentIndex(-1);
    ui->ComboBox3->setCurrentIndex(-1);
    ui->ComboBox4->setCurrentIndex(-1);
    ui->ComboBox5->setCurrentIndex(-1);
    ui->ComboBox6->setCurrentIndex(-1);
    ui->ComboBox7->setCurrentIndex(-1);
    ui->ComboBox8->setCurrentIndex(-1);
    ui->ComboBox9->setCurrentIndex(-1);
    ui->ComboBox10->setCurrentIndex(-1);
}

void StockWin::on_MapConfirmButton_clicked()
{
   if (!isFilesReady()) return;
   QString MapMonth = ui->MapMonthInputLineEdit->text();
   if (MapMonth.isEmpty())
   {
       QMessageBox::information(this, tr("提示"), tr("未选择月份"));
       return;
   }
   QVector<QString> MapStockCodes(10);
   MapStockCodes[0] = ui->ComboBox1->currentText();
   MapStockCodes[1] = ui->ComboBox2->currentText();
   MapStockCodes[2] = ui->ComboBox3->currentText();
   MapStockCodes[3] = ui->ComboBox4->currentText();
   MapStockCodes[4] = ui->ComboBox5->currentText();
   MapStockCodes[5] = ui->ComboBox6->currentText();
   MapStockCodes[6] = ui->ComboBox7->currentText();
   MapStockCodes[7] = ui->ComboBox8->currentText();
   MapStockCodes[8] = ui->ComboBox9->currentText();
   MapStockCodes[9] = ui->ComboBox10->currentText();
   QVector< QVector<StockData> > TenCodeDatas(10);
   for (int i = 0; i < 10; ++i)
   {
       qint64 startpos = SearchData(IndexVector, MapStockCodes[i], MapMonth);
       if (startpos == -1)
       {
           QMessageBox::information(this, tr("提示"), "未找到股票代码"+MapStockCodes[i]);
           MapStockCodes[i].clear();
           return;
       }
       else if (startpos == -2)
       {
           QMessageBox::information(this, tr("提示"), MapStockCodes[i]+"该股票代码不存在"+MapMonth+"时间的数据");
           CurrentMonth.clear();
           return;
       }
       else
       {
           LoadStockFile(TenCodeDatas[i], StockFileName, startpos);
           CulIR(TenCodeDatas[i]);
       }
   }

   double PearsonCorrealtionMatrix[10][10];
   for (int i = 0; i < 10; ++i)
   {
       for (int j = 0; j < 10; ++j)
       {
           PearsonCorrealtionMatrix[i][j] = CulPearsonCorrelation(TenCodeDatas[i], TenCodeDatas[j]);
       }
   }
   SetPearsonCorrelationMap(ui->PearsonCorrelationMap, PearsonCorrealtionMatrix, MapStockCodes);
}

void StockWin::on_CreateSimulationGraphButton_clicked()
{
    if (!isFilesReady()) return;
    if (!isCurrentDataReady()) return;
    // 处理字符串，获取下个月
    QString NextMonth;
    if (CurrentMonth.mid(4 ,2) == "12")
        NextMonth = QString::number(CurrentMonth.toInt()+ 100 - 11);
    else
        NextMonth = QString::number(CurrentMonth.toInt()+1);
    // 搜索并载入数据
    QVector<StockData> NextMonthDataVector;
    qint64 NextMonthPos = SearchData(IndexVector, CurrentStockCode, NextMonth);
    if (NextMonthPos == -1)
    {
        QMessageBox::information(this, tr("提示"), "未找到股票代码"+CurrentStockCode);
    }
    if (NextMonthPos == -2)
    {
        QMessageBox::information(this, tr("提示"), CurrentStockCode+"该股票代码不存在"+NextMonth+"的数据");
        return;
    }
    else
    {
        LoadStockFile(NextMonthDataVector, StockFileName, NextMonthPos);
    }
    SetSimulationGraph(ui->SimulationGraph, StockDataVector, NextMonthDataVector); // 画图
}

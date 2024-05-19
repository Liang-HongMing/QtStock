#include "index.h" // 包含创建索引的头文件
using namespace std;

void CreateIndexFile(const QString &ChosenStockFileName, QString &SaveFileName)
{
    // 通过ifstream打开股票文件
    ifstream StockFileIn;
    StockFileIn.open(ChosenStockFileName.toStdString(), ios::in | ios::binary);
    if (!StockFileIn)
    {
        qDebug() << "CreateIndexFile: StockFileIn Open Fail";
        return;
    }
    // 利用QFile类来创建索引文件
    QFile FileOut(SaveFileName);
    if (!FileOut.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "CreateIndexFile: FileOut Open Fail";
        return;
    }
    QTextStream FileOutStream(&FileOut);

    char *buffer = new char[256]; // 保存getline读取的行

    streampos FormerLinePos = StockFileIn.tellg(); // 保存前一行的偏移量
    StockFileIn.getline(buffer, 256);
    QStringList FormerLine = QString(buffer).split(','); // 读取前一行
    // 把第一行偏移量写入索引文件
    FileOutStream << FormerLine.at(0) << ','
                  << FormerLine.at(1).left(6)
                  << ',' << FormerLinePos << '\n';

    streampos LatterLinePos = StockFileIn.tellg(); // 保存后一行的偏移量
    StockFileIn.getline(buffer, 256);
    QStringList LatterLine = QString(buffer).split(','); // 读取后一行

    do
    {
        if (FormerLine.at(1).left(6) != LatterLine.at(1).left(6)) // 如果前后两行的月份不同
        {
            FileOutStream << LatterLine.at(0) << ','
                          << LatterLine.at(1).left(6)
                          << ',' << LatterLinePos << '\n';
            //qDebug() << LatterLine;
        }
        // 把FormerLine往后移
        FormerLine = LatterLine;
        FormerLinePos = LatterLinePos;
        // 把LatterLine往后移
        LatterLinePos = StockFileIn.tellg();
        StockFileIn.getline(buffer, 256);
        LatterLine = QString(buffer).split(',');
    } while (!StockFileIn.eof());
    // 关闭文件
    StockFileIn.close();
    FileOut.close();

    qDebug() << "CreateIndexFile Done";
}

void LoadStockIndex(QVector<StockIndex> &IndexVector, const QString IndexFileName)
{
    // 打开索引文件
    QFile IndexFileIn(IndexFileName);
    if (!IndexFileIn.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "LoadStockIndex: IndexFileIn Open Fail";
        return;
    }
    QTextStream IndexFileInStream(&IndexFileIn); // 利用流来处理
    IndexVector.clear(); // 清空
    // 将文件中每一行的数据全部读入内存中
    while (!IndexFileInStream.atEnd())
    {
        QStringList Line = IndexFileInStream.readLine().split(',');
        StockIndex temp;
        temp.code = Line.at(0);
        temp.month = Line.at(1);
        temp.pos = Line.at(2).toULongLong();
        IndexVector.push_back(temp);
    }
    IndexFileIn.close(); // 关闭文件
    qDebug() << "LoadStockIndex: Done";
}

void LoadStockFile(QVector<StockData> &StockVector, const QString StockFileName, qint64 pos)
{
    // 打开股票文件
    ifstream StockFileIn;
    StockFileIn.open(StockFileName.toStdString(),ios::in);
    if (!StockFileIn)
    {
        qDebug() << "LoadStockFile: StockFileIn Open Fail";
        return;
    }
    StockVector.clear();
    StockData temp;

    char* buffer = new char[256];
    StockFileIn.seekg(pos); // 调整偏移量
    StockFileIn.getline(buffer,256);
    QStringList FormerLine = QString(buffer).split(','); // 前一行
    StockFileIn.getline(buffer,256);
    QStringList LatterLine = QString(buffer).split(','); // 后一行
    do
    {
        temp.ts_code = FormerLine.at(0);
        temp.date = FormerLine.at(1);
        temp.trade_date = QDateTime(
                    QDate(temp.date.left(4).toInt(nullptr,10),
                          temp.date.mid(4,2).toInt(nullptr,10),
                          temp.date.right(2).toInt(nullptr,10))
                    );
        temp.open = FormerLine.at(2).toDouble();
        temp.high = FormerLine.at(3).toDouble();
        temp.low = FormerLine.at(4).toDouble();
        temp.close = FormerLine.at(5).toDouble();
        temp.pre_close = FormerLine.at(6).toDouble();
        temp.change = FormerLine.at(7).toDouble();
        temp.pct_chg = FormerLine.at(8).toDouble();
        temp.vol = FormerLine.at(9).toDouble();
        temp.amount = FormerLine.at(10).toDouble();
        StockVector.push_back(temp);
        // 判断是否相等
        if (FormerLine.at(1).left(6) != LatterLine.at(1).left(6))
            break; // 如果前后月份不相同则break
        // 如果相同的话
        FormerLine = LatterLine;
        StockFileIn.getline(buffer,256);
        LatterLine = QString(buffer).split(',');
    } while (!StockFileIn.eof());
    StockFileIn.close();
}

qint64 SearchData(QVector<StockIndex> &IndexVector, QString TargetStockCode, QString TargetMonth)
{
    int size = IndexVector.size();
    qint64 pos = -1;
    for (int i = 0; i < size; ++i)
    {
        if (IndexVector[i].code == TargetStockCode)
        {
            pos = -2;
            if (IndexVector[i].month == TargetMonth)
            {
                pos = IndexVector[i].pos;
                break;
            }
        }
    }
    return pos;
    qDebug() << "SearchData: Done";
}

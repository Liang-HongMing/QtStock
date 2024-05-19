#include "externalsort.h" //包含外排序的头文件

void SplitFile(const QString SaveFileName, const QString &FileInName, QVector<QString> &SplitedFileVector)
{
    QFile FileIn(FileInName);
    if (!FileIn.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "SplitFile: FileIn Open Fail";
        return;
    }
    QTextStream FileInStream(&FileIn);
    SplitedFileVector.clear(); // 清空准备保存拆分后各个文件的路径的QVector
    // 去掉表头
    QString *FirstLine = new QString;
    *FirstLine = FileInStream.readLine();
    delete FirstLine;
    qDebug() << "去表头";
    // 分割的参数
    const int NumberOfStockOnce = 80;
    const int NumberOfEachStock = 5000;
    const int NumberOfLine = NumberOfEachStock * NumberOfStockOnce;
    // const int NumberOfLine = 100;
    //   文件列表名索引
    int IndexOfFile = 0;
    QFileInfo SaveFileInfo(SaveFileName);
    QString path = SaveFileInfo.absolutePath();
    while (!FileInStream.atEnd())
    {
        QString SplitedFileName = "SplitedFile" + QString::number(IndexOfFile + 1) + ".csv";
        SplitedFileVector.push_back(path + '/' + SplitedFileName);
        // 创建文件
        QFile FileOut(SplitedFileVector[IndexOfFile]);
        if (!FileOut.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "SplitFile: FileOut Open Fail";
            return;
        }
        QTextStream FileOutStream(&FileOut);
        // 逐行写入
        for (int i = 0; i < NumberOfLine && !FileInStream.atEnd(); ++i)
        {
            FileOutStream << FileInStream.readLine() << "\n";
        }
        if (FileOut.exists())
        {
            QFileInfo fileoutInfo(FileOut);
            qDebug() << "SplitededFilePath:" << fileoutInfo.absolutePath();
        }
        FileOut.close();
        // 索引自增
        IndexOfFile++;
    }
    FileIn.close();

    qDebug() << "SplitFile: Done";
}

void InternalSort(QVector<QString> &SplitedFileVector, QVector<QString> &SortedFileVector)
{
    SortedFileVector.clear(); // 清空准备保存排序过后文件的路径的QVector
    size_t SizeOfList = SplitedFileVector.size();
    int IndexOfFile = 0;
    for (size_t i = 0; i < SizeOfList; ++i, ++IndexOfFile)
    {
        // 打开并读入文件
        QFile FileIn(SplitedFileVector[i]);
        if (!FileIn.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "InternalSort: FileIn Open Fail";
            return;
        }
        QTextStream FileInStream(&FileIn);

        QVector<QString> StockString; // 保存股票信息，在内存中排序
        while (!FileInStream.atEnd())
        {
            StockString.push_back(FileInStream.readLine());
        }
        FileIn.close();
        QFileInfo FileInInfo(FileIn);
        // 对StockString进行sort
        std::sort(StockString.begin(), StockString.end());

        QString SortedFileName = "SortedFile" +
                                 QString::number(IndexOfFile + 1) + ".csv";
        // 保存SortedFile的路径到SortedFileVector
        QFileInfo SplitedFileInfo(SplitedFileVector[IndexOfFile]);
        SortedFileVector.push_back(SplitedFileInfo.absolutePath() + '/' + SortedFileName);
        QFile FileOut(SortedFileVector[IndexOfFile]);
        if (!FileOut.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "InternalSort: FileOut Open Fail";
            return;
        }
        QTextStream FileOutStream(&FileOut);
        for (int i = 0; i < StockString.size(); ++i)
        {
            FileOutStream << StockString[i] << "\n";
        }

        if (FileOut.exists())
        {
            QFileInfo FileOutInfo(FileOut);
            qDebug() << "SortedFilePath:" << FileOutInfo.absolutePath();
        }
        FileOut.close();
        if (FileIn.remove())
            qDebug() << "Remove Successfully";
    }
    qDebug() << "InternalSort: Done";
}

QString TwoWayMergeSort(const QString &MergingFile1_Name, const QString &MergingFile2_Name)
{
    // 以只读模式打开MergingFile1
    QFile MergingFile1_In(MergingFile1_Name);
    if (!MergingFile1_In.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "TwoWayMergeSort: MergingFile1_In Open Fail";
        exit(1);
    }
    QTextStream MergingFile1_Stream(&MergingFile1_In);
    // 以只读模式打开MergingFile2
    QFile MergingFile2_In(MergingFile2_Name);
    if (!MergingFile2_In.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "TwoWayMergeSort: MergingFile2_In Open Fail";
        exit(1);
    }
    QTextStream MergingFile2_Stream(&MergingFile2_In);
    // 创建FileOut文件
    QFileInfo MergingFile1_Info(MergingFile1_In);
    QString FileOutName = MergingFile1_Info.absolutePath() + "/" + "NewOutput.txt";
    QFile FileOut(FileOutName);
    if (!FileOut.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "TwoWayMergeSort: FileOut Open Fail";
        exit(1);
    }
    QTextStream FileOutStream(&FileOut);
    // 排序
    bool isEnd = 0;
    QString MergingLine1 = MergingFile1_Stream.readLine();
    QString MergingLine2 = MergingFile2_Stream.readLine();
    while (!isEnd)
    {
        if (MergingLine1 == NULL)
        {
            isEnd = 1;
            FileOutStream << MergingLine2 << "\n";
            while (!MergingFile2_Stream.atEnd())
            {
                FileOutStream << MergingFile2_Stream.readLine() << "\n";
            }
            break;
        }
        if (MergingLine2 == NULL)
        {
            isEnd = 1;
            FileOutStream << MergingLine1 << "\n";
            while (!MergingFile1_Stream.atEnd())
            {
                FileOutStream << MergingFile1_Stream.readLine() << "\n";
            }
            break;
        }
        if (!isEnd)
        {
            if (MergingLine1 < MergingLine2)
            {
                FileOutStream << MergingLine1 << "\n";
                MergingLine1 = MergingFile1_Stream.readLine();
            }
            else if (MergingLine1 > MergingLine2)
            {
                FileOutStream << MergingLine2 << "\n";
                MergingLine2 = MergingFile2_Stream.readLine();
            }
        }
    }
    // Close files
    MergingFile1_In.close();
    MergingFile2_In.close();
    FileOut.close();
    // Remove files
    MergingFile1_In.remove(); // 删除了原有的output.txt
    MergingFile2_In.remove();
    QFileInfo FileOutInfo(FileOut);
    FileOutName = FileOutInfo.absolutePath() + "/" + "output.txt";
    FileOut.rename(FileOutName); // 把NewOutput.txt改为output.txt

    return FileOutName;
}

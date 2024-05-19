#ifndef CREATEINDEX_H
#define CREATEINDEX_H

#include <fstream>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QFileDialog>

#include "stockwin.h"

void CreateIndexFile(const QString &ChosenStockFileName, QString &SaveFileName);
void LoadStockIndex(QVector<StockIndex> &IndexVector, const QString IndexFileName);
void LoadStockFile( QVector<StockData> &StockVector, const QString StockFileName, qint64 pos);
qint64 SearchData(QVector<StockIndex> &IndexVector, QString TargetStockCode, QString TargetMonth);
void SetConsoleStockCodeComboBox(Ui::StockWin *ui, QVector<StockIndex> &IndexVector);
#endif // CREATEINDEX_H

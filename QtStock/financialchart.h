#ifndef FINANCIALCHART_H
#define FINANCIALCHART_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <QDateTime>

#include "qcustomplot.h"
#include "stockwin.h"

void SetFinancialChart(QVector<StockData> StockDataVector, QCustomPlot* customPlot, QCPAxisRect *volumeAxisRect);
#endif // FINANCIALCHART_H

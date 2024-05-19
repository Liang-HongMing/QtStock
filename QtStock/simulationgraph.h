#ifndef SIMULATIONGRAPH_H
#define SIMULATIONGRAPH_H

#include "stockwin.h"

void SetSimulationGraph(QCustomPlot* customPlot, const QVector<StockData>& StockDataVector, const QVector<StockData>& NextMonthDataVector);
double CalculateRMSE(QVector<double>& PredictedValues, QVector<double>& ActualValues);
#endif // SIMULATIONGRAPH_H

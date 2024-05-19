#ifndef PEARSONCORRELATIONMAP_H
#define PEARSONCORRELATIONMAP_H

#include "stockwin.h"

void SetMapComboBox(Ui::StockWin* ui, QVector<StockIndex>& IndexVector);
void CulIR(QVector<StockData>& StockDatas);
double CulPearsonCorrelation(QVector<StockData>& StockData1, QVector<StockData>& StockData2);
void SetPearsonCorrelationMap(QCustomPlot* customPlot, double PearsonCorrelationMatrix[][10], QVector<QString> &MapStockCodes);
#endif // PEARSONCORRELATIONMAP_H

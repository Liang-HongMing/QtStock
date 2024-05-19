#include "simulationgraph.h"

void SetSimulationGraph(QCustomPlot* customPlot, const QVector<StockData> &StockDataVector, const QVector<StockData> &NextMonthDataVector)
{
    customPlot->clearGraphs();
    customPlot->clearItems();
    // 模拟价格的图像
    customPlot->addGraph();
    QCPGraph* g1 = customPlot->graph(0);
    g1->setName(NextMonthDataVector[0].ts_code+' '
            +NextMonthDataVector[0].date.left(6)+' '+"模拟价格");
    g1->setPen(QPen(Qt::blue));
    g1->setLineStyle(QCPGraph::LineStyle::lsLine);
    // 原本文件数据的图像
    customPlot->addGraph();
    QCPGraph* g2 = customPlot->graph(1);
    g2->setName(NextMonthDataVector[0].ts_code+' '
            +NextMonthDataVector[0].date.left(6)+' '+"原本价格");
    g2->setPen(QPen(Qt::red));
    g2->setLineStyle(QCPGraph::LineStyle::lsLine);
    // 处理数据
    int n = NextMonthDataVector.size();
    QVector<double> time(n), price1(n), price2(n);
    srand(1000000);
    for (int i = 0; i < n; ++i)
    {
        price2[i] = NextMonthDataVector[i].close;
        // 补充用于预测的算法、函数。为了演示，这里在真实值上添加随机数
        // price1[i] = PredictPrice();
        price1[i] = price2[i] * (1 + (rand()/(double)RAND_MAX-0.5) * 0.03);
        time[i] = NextMonthDataVector[i].trade_date.toSecsSinceEpoch();
    }
    // 载入数据
    g1->setData(time, price1);
    g2->setData(time, price2);
    // 设置key轴的时间
    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeSpec(Qt::UTC);
    dateTimeTicker->setDateTimeFormat("yyyy-MM-dd");
    customPlot->xAxis->setTicker(dateTimeTicker);
    // 计算并显示RMSE
    double RMSE = CalculateRMSE(price1, price2);
    QCPItemText* RMSELabel = new QCPItemText(customPlot);
    RMSELabel->setPositionAlignment(Qt::AlignCenter);
    RMSELabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    RMSELabel->position->setCoords(0.9, 0.95);
    RMSELabel->setText("RMSE:"+QString::number(RMSE, 'f', 6));
    RMSELabel->setPen(QPen(Qt::black));
    RMSELabel->setFont(QFont("Arial", 12));
    RMSELabel->setPadding(QMargins(4, 4, 4, 4));
    RMSELabel->setBrush(Qt::NoBrush);
    // 不明所以，直接把这两行抄下来了
    QCPMarginGroup *group = new QCPMarginGroup(customPlot);
    customPlot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(Qt::white);

    customPlot->rescaleAxes();
    customPlot->xAxis->scaleRange(1.05, customPlot->xAxis->range().center());
    customPlot->yAxis->scaleRange(1.5, customPlot->yAxis->range().upper);

    customPlot->replot();

    qDebug() << "SetSimulationGraph: Done";
}

double CalculateRMSE(QVector<double>& PredictedValues, QVector<double>& ActualValues)
{
    double sum = -1;
    int n = PredictedValues.size();
    if (n != ActualValues.size())
    {
        qDebug() << "CaculateRMSE: PredictedValues.size() != ActualValues.size()";
        return sum;
    }
    sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        sum += (PredictedValues[i] - ActualValues[i]) * (PredictedValues[i] - ActualValues[i]);
    }
    double RMSE = sqrt(sum/n);
    return RMSE;
}

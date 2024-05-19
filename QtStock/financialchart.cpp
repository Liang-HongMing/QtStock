#include "financialchart.h"

void SetFinancialChart(QVector<StockData> StockDataVector, QCustomPlot *customPlot, QCPAxisRect* volumeAxisRect)
{
    int size = StockDataVector.size();
    QCPFinancialDataContainer datas;
    for (int i = 0; i < size; ++i)
    {
        QCPFinancialData data;
        StockDataVector[i].trade_date.setTimeSpec(Qt::UTC);
        data.key = StockDataVector[i].trade_date.toSecsSinceEpoch();
        data.open = StockDataVector[i].open;
        data.close = StockDataVector[i].close;
        data.high = StockDataVector[i].high;
        data.low = StockDataVector[i].low;
        datas.add(data);
    }

    // create candlestick chart:
    QCPFinancial *candlesticks = new QCPFinancial(customPlot->xAxis, customPlot->yAxis);
    // candlesticks->setName("Candlestick:"+StockDataVector[0].ts_code+','+StockDataVector[0].date.left(6));
    candlesticks->setName("Candlestick");
    candlesticks->setChartStyle(QCPFinancial::csCandlestick);
    // candlesticks->data()->clear();
    candlesticks->data()->set(datas);

    candlesticks->setWidth(3600*24*0.7);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(255, 0, 0));
    candlesticks->setBrushNegative(QColor(0, 255, 0));
    candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
    candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));

    // create ohlc chart:
    QCPFinancial *ohlc = new QCPFinancial(customPlot->xAxis, customPlot->yAxis);
    //ohlc->setName("OHLC:"+StockDataVector[0].ts_code+','+StockDataVector[0].date.left(6));
    ohlc->setName("OHLC");
    ohlc->setChartStyle(QCPFinancial::csOhlc);
    // ohlc->data()->clear();
    ohlc->data()->set(datas);

    ohlc->setWidth(3600*24*0.7);
    ohlc->setTwoColored(true);

    // create bottom axis rect for volume bar chart:
    volumeAxisRect->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
    volumeAxisRect->axis(QCPAxis::atBottom)->setLayer("axes");
    volumeAxisRect->axis(QCPAxis::atBottom)->grid()->setLayer("grid");
    // bring bottom and main axis rect closer together:
    customPlot->plotLayout()->setRowSpacing(0);
    volumeAxisRect->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
    volumeAxisRect->setMargins(QMargins(0, 0, 0, 0));
    // create two bar plottables, for positive (red) and negative (green) volume bars:
    customPlot->setAutoAddPlottableToLegend(false);
    QCPBars *volumePos = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    QCPBars *volumeNeg = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    for (int i=0; i < size; ++i)
    {
      (StockDataVector[i].open > StockDataVector[i].close ? volumeNeg : volumePos)->addData(StockDataVector[i].trade_date.toSecsSinceEpoch(), StockDataVector[i].vol); // add data to either volumeNeg or volumePos, depending on sign of v
    }
    volumePos->setWidth(3600*5);
    volumePos->setPen(Qt::NoPen);
    volumePos->setBrush(QColor(255, 0, 0));
    volumeNeg->setWidth(3600*5);
    volumeNeg->setPen(Qt::NoPen);
    volumeNeg->setBrush(QColor(0, 255, 0));

    // interconnect x axis ranges of main and bottom axis rects:
    QObject::connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), volumeAxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    QObject::connect(volumeAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis, SLOT(setRange(QCPRange)));
    // configure axes of both main and bottom axis rect:
    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeSpec(Qt::UTC);
    dateTimeTicker->setDateTimeFormat("yyyy-MM-dd");
    volumeAxisRect->axis(QCPAxis::atBottom)->setTicker(dateTimeTicker);
    customPlot->xAxis->setBasePen(Qt::NoPen);
    customPlot->xAxis->setTickLabels(true);
    customPlot->xAxis->setTicks(true);
    customPlot->xAxis->setTicker(dateTimeTicker);
    customPlot->rescaleAxes();
    //customPlot->xAxis->scaleRange(1.05, customPlot->xAxis->range().center());
    //customPlot->yAxis->scaleRange(1.05, customPlot->yAxis->range().center());

    // make axis rects' left side line up:
    QCPMarginGroup *group = new QCPMarginGroup(customPlot);
    customPlot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    volumeAxisRect->setMarginGroup(QCP::msLeft|QCP::msRight, group);

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    customPlot->legend->setVisible(true);

    customPlot->replot();
    qDebug() << "SetFinancialChart: Done";
}

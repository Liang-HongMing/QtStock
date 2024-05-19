#include "pearsoncorrelationmap.h"

void CulIR(QVector<StockData>& StockDatas)
{
    int size = StockDatas.size();
    for (int i = 1; i < size; ++i)
    {
        StockDatas[i].IR = (StockDatas[i].close - StockDatas[i-1].close)/StockDatas[i-1].close;
    }
}

double CulPearsonCorrelation(QVector<StockData>& StockData1, QVector<StockData>& StockData2)
{
    double r = -1.01;
    if (StockData1.size() != StockData2.size())
    {
        qDebug() << "CulPearsonCorrelation: "
                 << StockData1[0].ts_code << " != "
                 << StockData2[0].ts_code;
        return r;
    }
    int n = StockData1.size();
    double sum1 = 0.0;
    double sum2 = 0.0;
    for (int i = 0; i < n; ++i)
    {
        sum1 += StockData1[i].IR;
        sum2 += StockData2[i].IR;
    }
    double average1 = sum1/n;
    double average2 = sum1/n;
    double c1 = 0.0;
    double c2 = 0.0;
    double c = 0.0;
    for (int i = 1; i < n; ++i)
    {
        c1 += (StockData1[i].IR - average1)*(StockData1[i].IR - average1);
        c2 += (StockData2[i].IR - average2)*(StockData2[i].IR - average2);
        c += (StockData1[i].IR - average1)*(StockData2[i].IR - average2);
    }
    r = c/(sqrt(c1*c2));
    return r;
}

void SetPearsonCorrelationMap(QCustomPlot* customPlot, double PearsonCorrelationMatrix[][10], QVector<QString> &MapStockCodes)
{
    // configure axis rect:
    //customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    customPlot->yAxis->setRangeReversed(true);

    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);

    int nsize = 1000;
    int unitSize = nsize/10;

    colorMap->data()->setSize(nsize, nsize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(-0.5, 9.5), QCPRange(-0.5, 9.5)); // and span the coordinate range 0 10 in both key (x) and value (y) dimensions
    // now we assign some data, by accessing the QCPColorMapData instance of the color map:
    double x, y, z;
    for (int xIndex=0; xIndex<nsize; ++xIndex)
    {
        for (int yIndex=0; yIndex<nsize; ++yIndex)
        {
            colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
            int xBlock = xIndex/unitSize;
            int yBlock = yIndex/unitSize;
            z = PearsonCorrelationMatrix[xBlock][yBlock];
            colorMap->data()->setCell(xIndex, yIndex, z);
        }
    }
    // set textTicker
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    for (int i = 0; i < 10; i++)
    {
        textTicker->addTick(i, MapStockCodes[i]);
    }
    customPlot->xAxis->setTicker(textTicker);
    customPlot->yAxis->setTicker(textTicker);
    customPlot->xAxis->setLabel("皮尔森相关系数热力图\nPearson Correlation Map");
    // Add Pearson Correlation
    QVector< QVector<QCPItemText*> > PearsonCorrelation(10);
    for (int i = 0; i < 10; ++i)
    {
        PearsonCorrelation[i].resize(10);
        for (int j = 0; j < 10; j++)
        {
            PearsonCorrelation[i][j] = new QCPItemText(customPlot);
            PearsonCorrelation[i][j]->setText(QString::number(PearsonCorrelationMatrix[i][j],'f',2));
            PearsonCorrelation[i][j]->position->setCoords(i, j);
            if (PearsonCorrelationMatrix[i][j] <= 0.5)
                PearsonCorrelation[i][j]->setColor(QColor(Qt::white));
            else
                PearsonCorrelation[i][j]->setColor(QColor(Qt::black));
            PearsonCorrelation[i][j]->setPositionAlignment(Qt::AlignCenter);
        }
    }
    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(customPlot);
    customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    // colorScale->axis()->setLabel("皮尔森相关系数热力图");

    colorMap->setInterpolate(false); // 形成方格
    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(customPlot);
    customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    customPlot->rescaleAxes();
    customPlot->replot();

    qDebug() << "SetPearsonCorrelationMap: Done";
}

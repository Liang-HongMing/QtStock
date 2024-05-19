#ifndef EXTERNALSORT_H
#define EXTERNALSORT_H

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QVector>
#include <QDebug>
#include <algorithm>

void SplitFile(const QString SaveFileName, const QString &FileInName, QVector<QString> &SplitedFileVector);
void InternalSort(QVector<QString> &SplitedFileVector, QVector<QString> &SortedFileVector);
QString TwoWayMergeSort(const QString &MergingFile1_Name, const QString &MergingFile2_Name);
#endif // EXTERNALSORT_H

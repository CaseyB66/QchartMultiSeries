// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef QCHARTREALTIME_H
#define QCHARTREALTIME_H

#include <QPointF>
#include <QtCharts/QChart>

QT_FORWARD_DECLARE_CLASS(QGestureEvent)

class Qchartrealtime : public QChart
{
private:
    Q_OBJECT
public:
    explicit Qchartrealtime(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = {});

public slots:
    void displayPlotValueFunction1(const QPointF &, bool);
    void displayClicked1(const QPointF &);

protected:
    QPointF &seriesPoint(short nr, const QPointF);
    bool sceneEvent(QEvent *event);

private:
    bool gestureEvent(QGestureEvent *event);

};

#endif

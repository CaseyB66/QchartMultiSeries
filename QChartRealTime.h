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
    void updateValues(short nr, qreal x, qreal y);
    QPointF getMinRange(short nr)
    {
        QPointF rtrn = QPointF(m_minX[nr], m_minY[nr]);
        return rtrn;
    }
    QPointF getMaxRange(short nr)
    {
        QPointF rtrn = QPointF(m_maxX[nr], m_maxY[nr]);
        return rtrn;
    }
    void setAxisRange(short nr, QPointF minrng, QPointF maxrng);

public slots:
    void displayPlotValueFunction1(const QPointF &, bool);
    void displayClicked1(const QPointF &);

private slots:
    void pointsReplaced1();
    void pointsReplaced2();

protected:
    QPointF &seriesPoint(short nr, const QPointF);
    bool sceneEvent(QEvent *event);

private:
    bool gestureEvent(QGestureEvent *event);

    qreal m_maxX[2], m_maxY[2], m_minX[2], m_minY[2];
};

#endif

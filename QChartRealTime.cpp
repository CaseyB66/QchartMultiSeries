// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "QChartRealTime.h"

#include <QGesture>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QToolTip>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueaxis>
#include <QtCharts/QXYSeries>
#include <QtMath>

Qchartrealtime::Qchartrealtime(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QChart(QChart::ChartTypeCartesian, parent, wFlags)
{
    // Seems that QGraphicsView (QChartView) does not grab gestures.
    // They can only be grabbed here in the QGraphicsWidget (QChart).
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
}

//![1]
bool Qchartrealtime::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent *>(event));
    return QChart::event(event);
}

bool Qchartrealtime::gestureEvent(QGestureEvent *event)
{
    if (QGesture *gesture = event->gesture(Qt::PanGesture)) {
        auto pan = static_cast<QPanGesture *>(gesture);
        QChart::scroll(-(pan->delta().x()), pan->delta().y());
    }

    if (QGesture *gesture = event->gesture(Qt::PinchGesture)) {
        auto pinch = static_cast<QPinchGesture *>(gesture);
        if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged)
            QChart::zoom(pinch->scaleFactor());
    }

    return true;
}

QPointF &Qchartrealtime::seriesPoint(short nr, const QPointF spt)
{
    static QPointF rtrn(0.0, 0.0);
    QList<QAbstractSeries *> sers = series();
    QList<QAbstractSeries *>::Iterator srsItr = sers.begin();
    qreal minD = 1E30;
    QString srsName = QString("FVLoops");
    while (srsItr != sers.end()) {
        if ((*srsItr)->name().contains(srsName, Qt::CaseInsensitive)) {
            QList<QPointF> srsPts = qobject_cast<QXYSeries *>(*srsItr)->points();
            QList<QPointF>::const_iterator ptsItr = srsPts.cbegin();
            while (ptsItr != srsPts.cend()) {
                qreal dist = qSqrt(qPow(spt.x() - (*ptsItr).x(), 2)
                                   + qPow(spt.y() - (*ptsItr).y(), 2));
                if (dist < minD) {
                    minD = dist;
                    rtrn.setX((*ptsItr).x());
                    rtrn.setY((*ptsItr).y());
                }
                ptsItr++;
            }
        }
        srsItr++;
    }
    return rtrn;
}

void Qchartrealtime::displayPlotValueFunction1(const QPointF &pt, bool state)
{
    if (state) {
        const QPoint globalMouseLocation(QCursor::pos());
        QPointF pltPt = seriesPoint(0, pt);
        QString ttStr = QString("%1, %2").arg(pltPt.x(), 0, 'f', 2).arg(pltPt.y(), 0, 'f', 2);
        if (qSqrt(qPow(pt.x() - pltPt.x(), 2) + qPow(pt.y() - pltPt.y(), 2)) < 0.03) {
            QToolTip::showText(globalMouseLocation, ttStr);
        }
    }
}

void Qchartrealtime::displayClicked1(const QPointF &pt)
{
    const QPoint globalMouseLocation(QCursor::pos());
    QString ttStr = QString("%1, %2").arg(pt.x(), 0, 'f', 2).arg(pt.y(), 0, 'f', 2);
    QToolTip::showText(globalMouseLocation, ttStr);
}

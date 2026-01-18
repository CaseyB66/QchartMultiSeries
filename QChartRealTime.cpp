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
    m_maxX[0] = m_maxX[1] = 0.0;
    m_maxY[0] = m_maxY[1] = 0.0;
    m_minX[0] = m_minX[1] = 0.0;
    m_minY[0] = m_minY[1] = 0.0;
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

void Qchartrealtime::updateValues(short nr, qreal x, qreal y)
{
    if (x < m_minX[nr])
        m_minX[nr] = x;
    if (x > m_maxX[nr])
        m_maxX[nr] = x;
    if (y < m_minY[nr])
        m_minY[nr] = y;
    if (y > m_maxY[nr])
        m_maxY[nr] = y;
}

void Qchartrealtime::setAxisRange(short nr, QPointF minrng, QPointF maxrng)
{
    m_maxX[nr] = maxrng.rx();
    m_maxY[nr] = maxrng.ry();
    m_minX[nr] = minrng.rx();
    m_minY[nr] = minrng.ry();
    QList<QAbstractSeries *> sers = series();
    QList<QAbstractSeries *>::Iterator srsItr = sers.begin();
    QList<QAbstractAxis *> axs;
    QList<QAbstractAxis *>::iterator axsItr;
    qreal minx, maxx, miny, maxy;
    QString srsName = QString("FVLoops");

    if (nr == 0) {
        srsName = QString("FVLoops");
    } else {
        srsName = QString("TriWave");
    }
    minx = m_minX[nr];
    maxx = m_maxX[nr];
    miny = m_minY[nr];
    maxy = m_maxY[nr];
    QString lpName;
    while (srsItr != sers.end()) {
        lpName = QString((*srsItr)->name());
        if (lpName.contains(srsName, Qt::CaseInsensitive)) {
            axs = axes(Qt::Horizontal, *srsItr);
            for (axsItr = axs.begin(); axsItr != axs.end(); axsItr++) {
                if (nr == 0) {
                    if ((*axsItr)->alignment() == Qt::AlignBottom)
                        break;
                } else {
                    if ((*axsItr)->alignment() == Qt::AlignTop)
                        break;
                }
            }
            if (qobject_cast<QValueAxis *>(*axsItr)->max() < maxx)
                qobject_cast<QValueAxis *>(*axsItr)->setMax(maxx + (maxx - minx) / 5.0);
            if (qobject_cast<QValueAxis *>(*axsItr)->min() > minx)
                qobject_cast<QValueAxis *>(*axsItr)->setMin(minx - (maxx - minx) / 5.0);
            // qobject_cast<QValueAxis *>(*axsItr)->setTickType(QValueAxis::TicksDynamic);
            // qobject_cast<QValueAxis *>(*axsItr)->setTickCount(6);
            // qobject_cast<QValueAxis *>(*axsItr)->setTickInterval((maxx - minx) / 6);
            // qobject_cast<QValueAxis *>(*axsItr)->applyNiceNumbers();
            axs = axes(Qt::Vertical, *srsItr);
            for (axsItr = axs.begin(); axsItr != axs.end(); axsItr++) {
                if (nr == 0) {
                    if ((*axsItr)->alignment() == Qt::AlignLeft)
                        break;
                } else {
                    if ((*axsItr)->alignment() == Qt::AlignRight)
                        break;
                }
            }
            if (qobject_cast<QValueAxis *>(*axsItr)->max() < maxy)
                qobject_cast<QValueAxis *>(*axsItr)->setMax(maxy + (maxy - miny) / 5.0);
            if (qobject_cast<QValueAxis *>(*axsItr)->min() > miny)
                qobject_cast<QValueAxis *>(*axsItr)->setMin(miny - (maxy - miny) / 5.0);
            break;
        }
        srsItr++;
    }
}

void Qchartrealtime::pointsReplaced1()
{
    QList<QAbstractSeries *> sers = series();
    QList<QAbstractSeries *>::Iterator srsItr = sers.begin();
    QString srsName = "FVLoops";
    QString lpName;
    while (srsItr != sers.end()) {
        lpName = QString((*srsItr)->name());
        if (lpName.contains(srsName, Qt::CaseInsensitive)) {
            QList<QPointF> pts = qobject_cast<QLineSeries *>(*srsItr)->points();
            if (pts.isEmpty())
                return;
            qreal maxx, minx, maxy, miny;
            QPointF fst = pts[0];
            maxx = minx = fst.rx();
            maxy = miny = fst.ry();
            foreach (QPointF vlu, pts) {
                if (maxx < vlu.rx())
                    maxx = vlu.rx();
                if (maxy < vlu.ry())
                    maxy = vlu.ry();
                if (minx > vlu.rx())
                    minx = vlu.rx();
                if (miny > vlu.ry())
                    miny = vlu.ry();
            }
            QList<QAbstractAxis *> axs = axes(Qt::Horizontal, *srsItr);
            qreal axsmaxx = qobject_cast<QValueAxis *>(axs[0])->max();
            qreal axsminx = qobject_cast<QValueAxis *>(axs[0])->min();
            axs = axes(Qt::Vertical, *srsItr);
            qreal axsmaxy = qobject_cast<QValueAxis *>(axs[0])->max();
            qreal axsminy = qobject_cast<QValueAxis *>(axs[0])->min();
            QPointF maxRng, minRng;
            if (maxx > m_maxX[0])
                maxRng.setX(maxx);
            else
                maxRng.setX(m_maxX[0]);
            if (minx < m_minX[0])
                minRng.setX(minx);
            else
                minRng.setX(m_minX[0]);
            if (maxy > m_maxY[0])
                maxRng.setY(maxy);
            else
                maxRng.setY(m_maxY[0]);
            if (miny < m_minY[0])
                minRng.setY(miny);
            else
                minRng.setY(m_minY[0]);

            setAxisRange(0, minRng, maxRng);
            break;
        }
        srsItr++;
    }
}

void Qchartrealtime::pointsReplaced2()
{
    QList<QAbstractSeries *> sers = series();
    QList<QAbstractSeries *>::Iterator srsItr = sers.begin();
    QString srsName = "TriWave";
    QString lpName;
    while (srsItr != sers.end()) {
        lpName = QString((*srsItr)->name());
        if (lpName.contains(srsName, Qt::CaseInsensitive)) {
            QList<QPointF> pts = qobject_cast<QLineSeries *>(*srsItr)->points();
            if (pts.isEmpty())
                return;
            qreal maxx, minx, maxy, miny;
            QPointF fst = pts[0];
            maxx = minx = fst.rx();
            maxy = miny = fst.ry();
            foreach (QPointF vlu, pts) {
                if (maxx < vlu.rx())
                    maxx = vlu.rx();
                if (maxy < vlu.ry())
                    maxy = vlu.ry();
                if (minx > vlu.rx())
                    minx = vlu.rx();
                if (miny > vlu.ry())
                    miny = vlu.ry();
            }
            // QList<QAbstractAxis *> axs = axes(Qt::Horizontal, *srsItr);
            // qreal axsmaxx = qobject_cast<QValueAxis *>(axs[0])->max();
            // qreal axsminx = qobject_cast<QValueAxis *>(axs[0])->min();
            // axs = axes(Qt::Vertical, *srsItr);
            // qreal axsmaxy = qobject_cast<QValueAxis *>(axs[0])->max();
            // qreal axsminy = qobject_cast<QValueAxis *>(axs[0])->min();
            QPointF maxRng, minRng;
            if (maxx > m_maxX[1])
                maxRng.setX(maxx);
            else
                maxRng.setX(m_maxX[1]);
            if (minx < m_minX[1])
                minRng.setX(minx);
            else
                minRng.setX(m_minX[1]);
            if (maxy > m_maxY[1])
                maxRng.setY(maxy);
            else
                maxRng.setY(m_maxY[1]);
            if (miny < m_minY[1])
                minRng.setY(miny);
            else
                minRng.setY(m_minY[1]);

            setAxisRange(1, minRng, maxRng);
            break;
        }
        srsItr++;
    }
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

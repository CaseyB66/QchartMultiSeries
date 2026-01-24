#include "MultiLineseries.h"
#include <QValueAxis>

MultiLineSeries::MultiLineSeries(QObject *parent)
    : QLineSeries{parent}
    , m_maxX(0.0)
    , m_maxY(0.0)
    , m_minX(0.0)
    , m_minY(0.0)
    , m_buffSz(1024)

{
    m_endPos = -1;
}

void MultiLineSeries::updateValues(qreal x, qreal y)
{
    if (x < m_minX)
        m_minX = x;
    if (x > m_maxX)
        m_maxX = x;
    if (y < m_minY)
        m_minY = y;
    if (y > m_maxY)
        m_maxY = y;
}
void MultiLineSeries::setAxesRangeToData()
{
    QList<QPointF> pts = points();
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
    QList<QAbstractAxis *> axs = attachedAxes();
    if (axs.length() < 1)
        return;
    QList<QAbstractAxis *>::iterator haxsItr = axs.begin();
    while (haxsItr != axs.end()) {
        if ((*haxsItr)->orientation() == Qt::Horizontal)
            break;
        haxsItr++;
    }
    QList<QAbstractAxis *>::iterator vaxsItr = axs.begin();
    while (vaxsItr != axs.end()) {
        if ((*vaxsItr)->orientation() == Qt::Vertical)
            break;
        vaxsItr++;
    }
    if ((haxsItr != axs.end()) && (vaxsItr != axs.end())) {
        QPointF maxRng, minRng;
        maxRng.setX(maxx);
        minRng.setX(minx);
        maxRng.setY(maxy);
        minRng.setY(miny);

        setAxesRange(minRng, maxRng, true);
    }
}

void MultiLineSeries::appendNewData(QPointF pp)
{
    m_data.append(pp);
    // ui->theChart1 -
    if (m_buffData.count() >= m_buffSz) {
        size_t dsz = m_data.count();
        for (int ii = 0; ii < m_buffSz; ii++) {
            m_buffData[ii] = m_data[dsz - ii - 1];
        }
        // flwseries->replace(m_buffData);
    } else {
        m_buffData.append(pp);
    }
    replace(m_buffData);
}
void MultiLineSeries::clearData()
{
    if (m_data.count() > 0) {
        m_data.clear();
        if (m_buffData.count() > 0)
            m_buffData.clear();
    }
    m_maxX = m_maxY = m_minX = m_minY = 0.0;
    QPointF min(0.0, 0.0);
    QPointF max(2.0, 2.0);
    setAxesRange(min, max, true);
    replace(m_buffData);
}

void MultiLineSeries::onPointsReplaced()
{
    QList<QPointF> pts = points();
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
    QList<QAbstractAxis *> axs = attachedAxes();
    if (axs.length() < 1)
        return;
    QList<QAbstractAxis *>::iterator haxsItr = axs.begin();
    while (haxsItr != axs.end()) {
        if ((*haxsItr)->orientation() == Qt::Horizontal)
            break;
        haxsItr++;
    }
    QList<QAbstractAxis *>::iterator vaxsItr = axs.begin();
    while (vaxsItr != axs.end()) {
        if ((*vaxsItr)->orientation() == Qt::Vertical)
            break;
        vaxsItr++;
    }
    if ((haxsItr != axs.end()) && (vaxsItr != axs.end())) {
        QPointF maxRng, minRng;
        if (maxx > m_maxX)
            maxRng.setX(maxx);
        else
            maxRng.setX(m_maxX);
        if (minx < m_minX)
            minRng.setX(minx);
        else
            minRng.setX(m_minX);
        if (maxy > m_maxY)
            maxRng.setY(maxy);
        else
            maxRng.setY(m_maxY);
        if (miny < m_minY)
            minRng.setY(miny);
        else
            minRng.setY(m_minY);

        setAxesRange(minRng, maxRng);
    }
}

void MultiLineSeries::setAxesRange(QPointF minrng, QPointF maxrng, bool frc)
{
    m_maxX = maxrng.rx();
    m_maxY = maxrng.ry();
    m_minX = minrng.rx();
    m_minY = minrng.ry();
    QList<QAbstractAxis *> axs;
    qreal minx, maxx, miny, maxy;
    minx = m_minX;
    maxx = m_maxX;
    miny = m_minY;
    maxy = m_maxY;
    axs = attachedAxes();
    if (axs.length() < 1)
        return;
    QList<QAbstractAxis *>::iterator haxsItr = axs.begin();
    while (haxsItr != axs.end()) {
        if ((*haxsItr)->orientation() == Qt::Horizontal)
            break;
        haxsItr++;
    }
    QList<QAbstractAxis *>::iterator vaxsItr = axs.begin();
    while (vaxsItr != axs.end()) {
        if ((*vaxsItr)->orientation() == Qt::Vertical)
            break;
        vaxsItr++;
    }
    if (frc) {
        qobject_cast<QValueAxis *>(*haxsItr)->setMax(maxx);
        qobject_cast<QValueAxis *>(*haxsItr)->setMin(minx);
        qobject_cast<QValueAxis *>(*vaxsItr)->setMax(maxy);
        qobject_cast<QValueAxis *>(*vaxsItr)->setMin(miny);
    } else {
        if (qobject_cast<QValueAxis *>(*haxsItr)->max() < maxx)
            qobject_cast<QValueAxis *>(*haxsItr)->setMax(maxx + (maxx - minx) / 5.0);
        if (qobject_cast<QValueAxis *>(*haxsItr)->min() > minx)
            qobject_cast<QValueAxis *>(*haxsItr)->setMin(minx - (maxx - minx) / 5.0);
        if (qobject_cast<QValueAxis *>(*vaxsItr)->max() < maxy)
            qobject_cast<QValueAxis *>(*vaxsItr)->setMax(maxy + (maxy - miny) / 5.0);
        if (qobject_cast<QValueAxis *>(*vaxsItr)->min() > miny)
            qobject_cast<QValueAxis *>(*vaxsItr)->setMin(miny - (maxy - miny) / 5.0);
    }
}

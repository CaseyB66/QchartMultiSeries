#ifndef MULTILINESERIES_H
#define MULTILINESERIES_H

#include <QLineSeries>
#include <QObject>

class MultiLineSeries : public QLineSeries
{
    Q_OBJECT
public:
    explicit MultiLineSeries(QObject *parent = nullptr);
    void setBuffSz(qsizetype z)
    {
        m_buffSz = z;
        m_buffData.reserve(m_buffSz);
    };
    QPointF getMinRange()
    {
        QPointF rtrn = QPointF(m_minX, m_minY);
        return rtrn;
    }
    QPointF getMaxRange()
    {
        QPointF rtrn = QPointF(m_maxX, m_maxY);
        return rtrn;
    }
    void setAxesRange(QPointF min, QPointF max, bool frc = false);
    void setAxesRangeToData();
    void updateValues(qreal x, qreal y);
    void appendNewData(QPointF pp);
    void replaceBuffData() { replace(m_buffData); };
    void replaceData() { replace(m_data); };
    void clearData();
signals:

public slots:
    void onPointsReplaced();

protected:
    qreal m_maxX, m_maxY, m_minX, m_minY;
    QList<QPointF> m_data, m_buffData;
    qsizetype m_buffSz;
    qsizetype m_endPos;
};

#endif // MULTILINESERIES_H

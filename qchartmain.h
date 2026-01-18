#ifndef QCHARTMAIN_H
#define QCHARTMAIN_H

#include <QMainWindow>
#include <QTimerEvent>
#include <QtCharts/QXYSeries>

QT_BEGIN_NAMESPACE
namespace Ui {
class QchartMain;
}
QT_END_NAMESPACE

class QchartMain : public QMainWindow
{
    Q_OBJECT

public:
    QchartMain(QWidget *parent = nullptr);
    ~QchartMain();
    void addData();
    void setupChart(short nr);
    // QPointF getXRange(short nr);
    // QPointF getYRange(short nr);

private slots:
    void on_goBtn_clicked();

    void on_stopBtn_clicked();

    void on_clearBtn_clicked();

protected slots:
    void timerEvent(QTimerEvent *event) override;

private:
    Ui::QchartMain *ui;
    QList<QPointF> m_data[3], m_buffData[3];
    qsizetype m_buffSz;
    qsizetype m_strtPos, m_endPos;
    QXYSeries *flwseries[3];
    qreal m_cyclePerd_sec;
    int m_timerID, m_timerMsec;
    int m_sampleTime_msec;
    qreal m_sampleRate_Hz;
    qreal m_triWavePerd;
    qreal m_yAmp, m_triWaveAmp;
};
#endif // QCHARTMAIN_H

#include "qchartmain.h"
#include <QRandomGenerator>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtMath>
#include "./ui_qchartmain.h"
// #include "QScatterSeries"
#include <QtCharts/QLineSeries>
#include "QChartRealTime.h"
#include <cmath>
#include <vector>

QchartMain::QchartMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QchartMain)
    , m_timerID(-1)
    , m_timerMsec(50)
    , m_sampleTime_msec(10)
    , m_cyclePerd_sec(2.0)
    , m_triWavePerd(4.0)
    , m_yAmp(1.0)
    , m_triWaveAmp(10.0)
{
    ui->setupUi(this);
    m_sampleRate_Hz = int(1000 / m_sampleTime_msec);
    m_strtPos = 0;
    m_endPos = -1;

    m_buffSz = 8 * (1000 / m_sampleTime_msec) * 2;

    // this->setCentralWidget(ui->theChart1);
    // this->resize(400, 300);
    this->grabGesture(Qt::PanGesture);
    this->grabGesture(Qt::PinchGesture);
    // max 8 sec per stroke, buff keeps at least 2 strokes.
    m_buffSz = 8 * (1000 / m_sampleTime_msec) * 2;
    setupChart(1);
    setupChart(2);
    m_endPos = m_timerMsec / 10;
    connect(flwseries[0],
            SIGNAL(hovered(const QPointF &, bool)),
            qobject_cast<Qchartrealtime *>(ui->theChart1->chart()),
            SLOT(displayPlotValueFunction1(const QPointF &, bool)));
    connect(flwseries[1],
            SIGNAL(hovered(const QPointF &, bool)),
            qobject_cast<Qchartrealtime *>(ui->theChart2->chart()),
            SLOT(displayPlotValueFunction1(const QPointF &, bool)));
    connect(flwseries[0],
            SIGNAL(clicked(const QPointF &)),
            qobject_cast<Qchartrealtime *>(ui->theChart1->chart()),
            SLOT(displayClicked1(const QPointF &)));
    connect(flwseries[1],
            SIGNAL(clicked(const QPointF &)),
            qobject_cast<Qchartrealtime *>(ui->theChart2->chart()),
            SLOT(displayClicked1(const QPointF &)));
    connect(flwseries[0],
            SIGNAL(pointsReplaced()),
            qobject_cast<Qchartrealtime *>(ui->theChart1->chart()),
            SLOT(pointsReplaced1()));
    connect(flwseries[1],
            SIGNAL(pointsReplaced()),
            qobject_cast<Qchartrealtime *>(ui->theChart2->chart()),
            SLOT(pointsReplaced1()));
    connect(flwseries[2],
            SIGNAL(pointsReplaced()),
            qobject_cast<Qchartrealtime *>(ui->theChart2->chart()),
            SLOT(pointsReplaced2()));
    qobject_cast<Qchartrealtime *>(ui->theChart1->chart())
        ->setAxisRange(0, QPointF(-2.0, -5.0), QPointF(2, 5));
    qobject_cast<Qchartrealtime *>(ui->theChart2->chart())
        ->setAxisRange(0, QPointF(-2.0, -5.0), QPointF(2, 5));
}

QchartMain::~QchartMain()
{
    delete ui;
}

void QchartMain::setupChart(short nr)
{
    m_buffData[nr - 1].reserve(m_buffSz);
    flwseries[nr - 1] = new QLineSeries;
    QString srsName = QString("FVLoops");
    flwseries[nr - 1]->setName(srsName);

    Qchartrealtime *chart = new Qchartrealtime;
    chart->setTitle("FV Loop RealTime");
    chart->addSeries(flwseries[nr - 1]);
    QValueAxis *axs = new QValueAxis;
    axs->setMax(2.0);
    axs->setMin(-2.0);
    axs->setTitleText("Volume, liter");
    chart->addAxis(axs, Qt::AlignBottom);
    flwseries[nr - 1]->attachAxis(axs);
    QValueAxis *axsv = new QValueAxis;
    axsv->setMax(2.0);
    axsv->setMin(-2.0);
    axsv->setTitleText("Flow, l/sec");
    chart->addAxis(axsv, Qt::AlignLeft);
    flwseries[nr - 1]->attachAxis(axsv);

    flwseries[nr - 1]->setMarkerSize(2);
    QPen srsPen;
    srsPen.setWidth(2);
    flwseries[nr - 1]->setPen(srsPen);
    flwseries[nr - 1]->setPointsVisible(true);
    chart->setAnimationOptions(QChart::NoAnimation); // QChart::SeriesAnimations);
    chart->legend()->hide();

    if (nr == 2) {
        srsName = QString("TriWave");
        m_buffData[nr].reserve(m_buffSz);
        flwseries[nr] = new QLineSeries;
        flwseries[nr]->setName(srsName);

        chart->addSeries(flwseries[nr]);
        QValueAxis *axs = new QValueAxis;
        axs->setMax(2.0);
        axs->setMin(0.0);
        axs->setTitleText("Time, msec");
        chart->addAxis(axs, Qt::AlignTop);
        flwseries[nr]->attachAxis(axs);
        QValueAxis *axsv = new QValueAxis;
        axsv->setMax(2.0);
        axsv->setMin(-2.0);
        axsv->setTitleText("Flow, l/sec");
        chart->addAxis(axsv, Qt::AlignRight);
        flwseries[nr]->attachAxis(axsv);

        flwseries[nr]->setMarkerSize(2);
        QPen srsPen;
        srsPen.setColor(Qt::darkRed);
        srsPen.setWidth(2);
        flwseries[nr]->setPen(srsPen);
        flwseries[nr]->setPointsVisible(true);
        chart->setAnimationOptions(QChart::NoAnimation); // QChart::SeriesAnimations);
        chart->legend()->show();
    }
    switch (nr) {
    case 1:
        ui->theChart1->setChart(chart);
        ui->theChart1->setRenderHint(QPainter::Antialiasing);
        ui->theChart1->setRubberBand(QChartView::RectangleRubberBand);
        break;
    case 2:
        ui->theChart2->setChart(chart);
        ui->theChart2->setRenderHint(QPainter::Antialiasing);
        ui->theChart2->setRubberBand(QChartView::RectangleRubberBand);

    default:
        break;
    }
}

void QchartMain::timerEvent(QTimerEvent *event)
{
    if (event->timerId() > -1 && event->timerId() == m_timerID) {
        m_endPos += m_timerMsec / m_sampleTime_msec;
        addData();
    }
}
void QchartMain::triangleWave()
{
    const double m_triWavePerd = 4.0; // Seconds
    const double m_triWaveAmp = 5.0;  // Peak m_triWaveAmp
    int totalSamples = 1;
    for (int i = 0; i < totalSamples; ++i) {
        // 1. Calculate current time
        double t = i / m_sampleRate_Hz;

        // 2. Calculate the position within the cycle (0.0 to 1.0)
        // fmod helps wrap the time around the m_triWavePerd
        double phase = fmod(t, m_triWavePerd) / m_triWavePerd;

        // 3. Triangle wave formula:
        // Standard shape: 4 * abs(x - 0.5) - 1  (results in -1 to 1)
        double value = 2.0 * fabs(2.0 * (phase - floor(phase + 0.5))) * m_triWaveAmp;

        // Offset to make it centered at zero (-5 to 5)
        double centeredValue = 2.0 * m_triWaveAmp * (fabs(2.0 * phase - 1.0) - 0.5);

        // For this example, let's use a simple rising/falling approach:
        double tri;
        if (phase < 0.25) {
            tri = (phase / 0.25) * m_triWaveAmp; // 0 to 5
        } else if (phase < 0.75) {
            tri = m_triWaveAmp - ((phase - 0.25) / 0.5) * (2 * m_triWaveAmp); // 5 to -5
        } else {
            tri = -m_triWaveAmp + ((phase - 0.75) / 0.25) * m_triWaveAmp; // -5 to 0
        }

    }
}

void QchartMain::addData()
{
    short nr = 1;
    for (int i = m_endPos - m_timerMsec / m_sampleTime_msec; i < m_endPos; i++) {
        m_yAmp *= 1.001;
        qreal rads = ((qreal(i) * m_sampleTime_msec / 1000.0) / m_cyclePerd_sec) * 2 * M_PI;
        QPointF p(qCos(rads) + m_endPos * 0.003, m_yAmp * qSin(rads));
        p.ry() += QRandomGenerator::global()->bounded(0.2);
        qobject_cast<Qchartrealtime *>(ui->theChart1->chart())->updateValues(0, p.rx(), p.ry());
        m_data[nr - 1].append(p);
        // ui->theChart1 -
        if (m_buffData[nr - 1].count() >= m_buffSz) {
            size_t dsz = m_data[nr - 1].count();
            for (int ii = 0; ii < m_buffSz; ii++) {
                m_buffData[nr - 1][ii] = m_data[nr - 1][dsz - ii - 1];
            }
            // flwseries->replace(m_buffData);
        } else {
            m_buffData[nr - 1].append(p);
        }
    }
    flwseries[nr - 1]->replace(m_buffData[nr - 1]);
    if (true) {
        nr = 2;
        for (int i = m_endPos - m_timerMsec / m_sampleTime_msec; i < m_endPos; i++) {
            m_yAmp *= 1.001;
            qreal rads = ((qreal(i) * m_sampleTime_msec / 1000.0) / (2 * m_cyclePerd_sec / 3)) * 2
                         * M_PI;
            QPointF p(qCos(rads) + m_endPos * 0.00003, m_yAmp * qSin(rads));
            p.ry() += QRandomGenerator::global()->bounded(0.2);
            qobject_cast<Qchartrealtime *>(ui->theChart2->chart())->updateValues(0, p.rx(), p.ry());
            m_data[nr - 1].append(p);
            // ui->theChart1 -
            if (m_buffData[nr - 1].count() >= m_buffSz) {
                size_t dsz = m_data[nr - 1].count();
                for (int ii = 0; ii < m_buffSz; ii++) {
                    m_buffData[nr - 1][ii] = m_data[nr - 1][dsz - ii - 1];
                }
                // flwseries->replace(m_buffData);
            } else {
                m_buffData[nr - 1].append(p);
            }
        }
        flwseries[nr - 1]->replace(m_buffData[nr - 1]);
    }
    if (true) {
        nr = 3;
        for (int i = m_endPos - m_timerMsec / m_sampleTime_msec; i < m_endPos; i++) {
            // 1. Calculate current time
            m_triWaveAmp *= 1.0003;
            double t_sec = double(i) / m_sampleRate_Hz;
            qreal triSlope = m_triWaveAmp / (m_triWavePerd / m_sampleRate_Hz);

            // 2. Calculate the position within the cycle (0.0 to 1.0)
            // fmod helps wrap the time around the m_triWavePerd
            double phase = fmod(t_sec, m_triWavePerd) / m_triWavePerd;

            // 3. Triangle wave formula:
            // Standard shape: 4 * abs(x - 0.5) - 1  (results in -1 to 1)
            double vlu = 2.0 * fabs(2.0 * (phase - floor(phase + 0.5))) * m_triWaveAmp;

            // Offset to make it centered at zero (-5 to 5)
            double cntrVlu = 2.0 * m_triWaveAmp * (fabs(2.0 * phase - 1.0) - 0.5);
            QPointF p(t_sec, cntrVlu);
            p.ry() += QRandomGenerator::global()->bounded(0.2);
            // For this example, let's use a simple rising/falling approach:
            // double tri;
            // if (phase < 0.25) {
            //     tri = (phase / 0.25) * m_triWaveAmp; // 0 to 5
            // } else if (phase < 0.75) {
            //     tri = m_triWaveAmp - ((phase - 0.25) / 0.5) * (2 * m_triWaveAmp); // 5 to -5
            // } else {
            //     tri = -m_triWaveAmp + ((phase - 0.75) / 0.25) * m_triWaveAmp; // -5 to 0
            // }

            qobject_cast<Qchartrealtime *>(ui->theChart2->chart())->updateValues(1, p.rx(), p.ry());
            m_data[nr - 1].append(p);
            if (m_buffData[nr - 1].count() >= m_buffSz) {
                size_t dsz = m_data[nr - 1].count();
                for (int ii = 0; ii < m_buffSz; ii++) {
                    m_buffData[nr - 1][ii] = m_data[nr - 1][dsz - ii - 1];
                }
            } else {
                m_buffData[nr - 1].append(p);
            }
        }
        flwseries[nr - 1]->replace(m_buffData[nr - 1]);
    }
}

void QchartMain::on_goBtn_clicked()
{
    flwseries[0]->replace(m_buffData[0]);
    flwseries[1]->replace(m_buffData[1]);
    flwseries[2]->replace(m_buffData[2]);
    m_timerID = startTimer(m_timerMsec, Qt::PreciseTimer);
    qobject_cast<Qchartrealtime *>(ui->theChart1->chart())
        ->setAxisRange(0, QPointF(-2.0, -5.0), QPointF(2, 5));
    qobject_cast<Qchartrealtime *>(ui->theChart2->chart())
        ->setAxisRange(0, QPointF(-2.0, -5.0), QPointF(2, 5));
    qobject_cast<Qchartrealtime *>(ui->theChart2->chart())
        ->setAxisRange(1, QPointF(0.0, -5.0), QPointF(2, 5));
    m_timerID = startTimer(m_timerMsec, Qt::PreciseTimer);
}

void QchartMain::on_stopBtn_clicked()
{
    this->killTimer(m_timerID);
    flwseries[0]->replace(m_data[0]);
    flwseries[1]->replace(m_data[1]);
    flwseries[2]->replace(m_data[2]);
    QPointF minRng = qobject_cast<Qchartrealtime *>(ui->theChart1->chart())->getMinRange(0);
    QPointF maxRng = qobject_cast<Qchartrealtime *>(ui->theChart1->chart())->getMaxRange(0);
    qreal xtntX = maxRng.rx() - minRng.rx();
    qreal xtntY = maxRng.ry() - minRng.ry();
    qobject_cast<Qchartrealtime *>(ui->theChart1->chart())
        ->setAxisRange(0,
                       QPointF(minRng.rx() - xtntX / 8, minRng.ry() - xtntY / 8),
                       QPointF(maxRng.rx() + xtntX / 8, maxRng.ry() + xtntY / 8));
    minRng = qobject_cast<Qchartrealtime *>(ui->theChart2->chart())->getMinRange(0);
    maxRng = qobject_cast<Qchartrealtime *>(ui->theChart2->chart())->getMaxRange(0);
    xtntX = maxRng.rx() - minRng.rx();
    xtntY = maxRng.ry() - minRng.ry();
    qobject_cast<Qchartrealtime *>(ui->theChart2->chart())
        ->setAxisRange(0,
                       QPointF(minRng.rx() - xtntX / 8, minRng.ry() - xtntY / 8),
                       QPointF(maxRng.rx() + xtntX / 8, maxRng.ry() + xtntY / 8));
}

void QchartMain::on_clearBtn_clicked()
{
    short nr;
    for (nr = 0; nr < 3; nr++) {
        if (m_data[nr].count() > 0) {
            m_data[nr].clear();
            if (m_buffData[nr].count() > 0)
                m_buffData[nr].clear();
        }
    }
    QList<QAbstractSeries *>::iterator srsItr;
    QList<QAbstractSeries *> srs = qobject_cast<Qchartrealtime *>(ui->theChart1->chart())->series();
    srsItr = srs.begin();
    while (srsItr != srs.end()) {
        qobject_cast<QLineSeries *>(*srsItr)->points().clear();
        srsItr++;
    }
    srs = qobject_cast<Qchartrealtime *>(ui->theChart2->chart())->series();
    srsItr = srs.begin();
    while (srsItr != srs.end()) {
        qobject_cast<QLineSeries *>(*srsItr)->points().clear();
        srsItr++;
    }
    m_yAmp = 1.0;
    m_triWaveAmp = 10;
    m_strtPos = 0;
    m_endPos = -1;
}

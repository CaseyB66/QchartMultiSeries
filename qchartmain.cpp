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
    QRect avgeom = QGuiApplication::primaryScreen()->availableGeometry();
    QSize newsz = QSize(avgeom.width() * 0.6, avgeom.height() * 0.88);
    resize(newsz);

    m_sampleRate_Hz = int(1000 / m_sampleTime_msec);
    m_endPos = 0;
    m_buffSz = 8 * (1000 / m_sampleTime_msec) * 2;

    // this->setCentralWidget(ui->theChart1);
    // this->resize(400, 300);
    this->grabGesture(Qt::PanGesture);
    this->grabGesture(Qt::PinchGesture);
    // max 8 sec per stroke, buff keeps at least 2 strokes.
    m_buffSz = 8 * (1000 / m_sampleTime_msec) * 2;
    setupChart(0);
    setupChart(1);
}

QchartMain::~QchartMain()
{
    delete ui;
}

void QchartMain::setupChart(short nr)
{
    if (nr > 1)
        return;

    flwseries[nr] = new MultiLineSeries;
    flwseries[nr]->setBuffSz(m_buffSz);
    QString srsName = QString("FVLoops");
    flwseries[nr]->setName(srsName);

    Qchartrealtime *chart = new Qchartrealtime;
    chart->setTitle("FV Loop RealTime");
    chart->addSeries(flwseries[nr]);
    QValueAxis *axs = new QValueAxis;
    axs->setMax(2.0);
    axs->setMin(-2.0);
    axs->setTitleText("Volume, liter");
    chart->addAxis(axs, Qt::AlignBottom);
    flwseries[nr]->attachAxis(axs);
    QValueAxis *axsv = new QValueAxis;
    axsv->setMax(2.0);
    axsv->setMin(-2.0);
    axsv->setTitleText("Flow, l/sec");
    chart->addAxis(axsv, Qt::AlignLeft);
    flwseries[nr]->attachAxis(axsv);

    flwseries[nr]->setMarkerSize(2);
    QPen srsPen;
    srsPen.setWidth(2);
    flwseries[nr]->setPen(srsPen);
    flwseries[nr]->setPointsVisible(true);
    chart->setAnimationOptions(QChart::NoAnimation); // QChart::SeriesAnimations);
    chart->legend()->hide();

    if (nr == 1) {
        srsName = QString("TriWave");
        flwseries[nr + 1] = new MultiLineSeries;
        flwseries[nr + 1]->setBuffSz(m_buffSz);
        flwseries[nr + 1]->setName(srsName);

        chart->addSeries(flwseries[nr + 1]);
        QValueAxis *axs = new QValueAxis;
        axs->setMax(2.0);
        axs->setMin(0.0);
        axs->setTitleText("Time, msec");
        chart->addAxis(axs, Qt::AlignTop);
        flwseries[nr + 1]->attachAxis(axs);
        QValueAxis *axsv = new QValueAxis;
        axsv->setMax(2.0);
        axsv->setMin(-2.0);
        axsv->setTitleText("Flow, l/sec");
        chart->addAxis(axsv, Qt::AlignRight);
        flwseries[nr + 1]->attachAxis(axsv);

        flwseries[nr + 1]->setMarkerSize(2);
        QPen srsPen;
        srsPen.setColor(Qt::darkRed);
        srsPen.setWidth(2);
        flwseries[nr + 1]->setPen(srsPen);
        flwseries[nr + 1]->setPointsVisible(true);
        chart->setAnimationOptions(QChart::NoAnimation); // QChart::SeriesAnimations);
        chart->legend()->show();
    }
    switch (nr) {
    case 0:
        ui->theChart1->setChart(chart);
        ui->theChart1->setRenderHint(QPainter::Antialiasing);
        ui->theChart1->setRubberBand(QChartView::RectangleRubberBand);
        connect(flwseries[0],
                SIGNAL(hovered(const QPointF &, bool)),
                qobject_cast<Qchartrealtime *>(ui->theChart1->chart()),
                SLOT(displayPlotValueFunction1(const QPointF &, bool)));
        connect(flwseries[0],
                SIGNAL(clicked(const QPointF &)),
                qobject_cast<Qchartrealtime *>(ui->theChart1->chart()),
                SLOT(displayClicked1(const QPointF &)));
        connect(flwseries[0], SIGNAL(pointsReplaced()), flwseries[0], SLOT(onPointsReplaced()));
        break;
    case 1:
        ui->theChart2->setChart(chart);
        ui->theChart2->setRenderHint(QPainter::Antialiasing);
        ui->theChart2->setRubberBand(QChartView::RectangleRubberBand);
        connect(flwseries[1],
                SIGNAL(hovered(const QPointF &, bool)),
                qobject_cast<Qchartrealtime *>(ui->theChart1->chart()),
                SLOT(displayPlotValueFunction1(const QPointF &, bool)));
        connect(flwseries[1],
                SIGNAL(clicked(const QPointF &)),
                qobject_cast<Qchartrealtime *>(ui->theChart1->chart()),
                SLOT(displayClicked1(const QPointF &)));
        connect(flwseries[1], SIGNAL(pointsReplaced()), flwseries[1], SLOT(onPointsReplaced()));
        connect(flwseries[2],
                SIGNAL(hovered(const QPointF &, bool)),
                qobject_cast<Qchartrealtime *>(ui->theChart1->chart()),
                SLOT(displayPlotValueFunction1(const QPointF &, bool)));
        connect(flwseries[2],
                SIGNAL(clicked(const QPointF &)),
                qobject_cast<Qchartrealtime *>(ui->theChart1->chart()),
                SLOT(displayClicked1(const QPointF &)));
        connect(flwseries[2], SIGNAL(pointsReplaced()), flwseries[2], SLOT(onPointsReplaced()));

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
void QchartMain::addData()
{
    short nr = 0;
    // go back one timer click...
    for (int i = m_endPos - m_timerMsec / m_sampleTime_msec; i < m_endPos; i++) {
        m_yAmp *= 1.001;
        qreal rads = ((qreal(i) * m_sampleTime_msec / 1000.0) / m_cyclePerd_sec) * 2 * M_PI;
        QPointF p(qCos(rads) + m_endPos * 0.003, m_yAmp * qSin(rads));
        p.ry() += QRandomGenerator::global()->bounded(0.2);
        flwseries[nr]->updateValues(p.rx(), p.ry());
        flwseries[nr]->appendNewData(p);
    }
    if (true) {
        nr = 1;
        for (int i = m_endPos - m_timerMsec / m_sampleTime_msec; i < m_endPos; i++) {
            m_yAmp *= 1.001;
            qreal rads = ((qreal(i) * m_sampleTime_msec / 1000.0) / (2 * m_cyclePerd_sec / 3)) * 2
                         * M_PI;
            QPointF p(qCos(rads) + m_endPos * 0.00003, m_yAmp * qSin(rads));
            p.ry() += QRandomGenerator::global()->bounded(0.2);
            flwseries[nr]->updateValues(p.rx(), p.ry());
            flwseries[nr]->appendNewData(p);
        }
    }
    if (true) {
        nr = 2;
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

            flwseries[nr]->updateValues(p.rx(), p.ry());
            flwseries[nr]->appendNewData(p);
        }
    }
}

void QchartMain::on_goBtn_clicked()
{
    flwseries[0]->replaceBuffData();
    flwseries[1]->replaceBuffData();
    flwseries[2]->replaceBuffData();
    // qobject_cast<Qchartrealtime *>(ui->theChart1->chart())
    //     ->setAxisRange(0, QPointF(-2.0, -5.0), QPointF(2, 5));
    // qobject_cast<Qchartrealtime *>(ui->theChart2->chart())
    //     ->setAxisRange(0, QPointF(-2.0, -5.0), QPointF(2, 5));
    // qobject_cast<Qchartrealtime *>(ui->theChart2->chart())
    //     ->setAxisRange(1, QPointF(0.0, -5.0), QPointF(2, 5));
    m_timerID = startTimer(m_timerMsec, Qt::PreciseTimer);
}

void QchartMain::on_stopBtn_clicked()
{
    this->killTimer(m_timerID);
    flwseries[0]->replaceData();
    flwseries[1]->replaceData();
    flwseries[2]->replaceData();
    flwseries[0]->setAxesRangeToData();
    flwseries[1]->setAxesRangeToData();
    flwseries[2]->setAxesRangeToData();
}

void QchartMain::on_clearBtn_clicked()
{
    short nr;
    for (nr = 0; nr < 3; nr++) {
        flwseries[nr]->clearData();
    }
    m_yAmp = 1.0;
    m_triWaveAmp = 10;
    m_endPos = -1;
    ui->theChart1->update();
    ui->theChart2->update();
}

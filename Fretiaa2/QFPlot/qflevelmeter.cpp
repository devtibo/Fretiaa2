#include "qflevelmeter.h"


QFLevelMeter::QFLevelMeter(const Qt::Orientation orientation, QWidget *parent) : QWidget(parent),currenLevel(0)
{
    this->setLayout(getOrientedLayout(orientation));
}

void QFLevelMeter::setLevel(const double val)
{
    currenLevel = val;
    newbars->setData(QVector<double>{0},QVector<double>{val});

    mPlot->replot(QCustomPlot::rpQueuedReplot);
}

QLayout *QFLevelMeter::getOrientedLayout(const Qt::Orientation orientation)
{

    qDeleteAll(this->children());

    // Create proper oriented layout
    QBoxLayout *mLayout;
    if(orientation==Qt::Orientation::Vertical)

        mLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    else
        mLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);

    QRadioButton *enbale_check = new QRadioButton("On",this);
    enbale_check->setChecked(true);
    enbale_check->setStyleSheet({"color: #ffaa00;"});
    connect(enbale_check,SIGNAL(toggled(bool)),this,SLOT(onEnableToggle(bool)));
    mLayout->addWidget(enbale_check);

    // Create plot
    mPlot = new QCustomPlot(this);
    mPlot->setBackground(Qt::black);
    mPlot->yAxis2->setTickLabelColor(Qt::white);
    mPlot->xAxis->setTickLabelColor(Qt::white);

    // Hide/Show axes
    mPlot->yAxis->setVisible(false);
    if(orientation==Qt::Orientation::Vertical)
    {
        mPlot->yAxis2->setRange(0,-96);
        mPlot->yAxis->setRange(0,-96);
        mPlot->yAxis2->setVisible(true);
        mPlot->xAxis->setRange(-0.5,.5);
        mPlot->xAxis->setVisible(false);
    }
    else
    {
        mPlot->xAxis->setRange(0,-96);
        mPlot->xAxis->setVisible(true);
        mPlot->yAxis2->setVisible(true);
        mPlot->yAxis->setRange(-0.5,.5);
    }

    // Create Bar to hide background
    if(orientation==Qt::Orientation::Vertical)
        newbars=new QCPBars(mPlot->xAxis ,mPlot->yAxis);
    else
        newbars=new QCPBars(mPlot->yAxis ,mPlot->xAxis);
    newbars->setBrush(QBrush(QColor(0,0,0,180)));
    newbars->setPen(QPen(Qt::black));
    newbars->setWidth(1.1);

    newbars->setData(QVector<double>{0}, QVector<double>{currenLevel});


    // Create and set background gradient
    QLinearGradient gradient;
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setStart(0,0);
    if(orientation==Qt::Orientation::Vertical)
    {
        gradient.setStart(0,0);
        gradient.setFinalStop(0, 1);
    }
    else
    {
        gradient.setStart(1,0);
        gradient.setFinalStop(0, 0);
    }
    gradient.setColorAt(0, QColor::fromRgbF(1, 0, 0, 1));
    gradient.setColorAt(0.1, QColor::fromRgbF(1, 0, 0, 1));
    gradient.setColorAt(0.2, QColor::fromRgbF(1, 1, 0, 1));
    gradient.setColorAt(0.8, QColor::fromRgbF(0, 1, 0, 1));
    mPlot->axisRect()->setBackground(QBrush(gradient));

    // Set Ticks interval
    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    if(orientation==Qt::Orientation::Vertical)
        mPlot->yAxis2->setTicker(fixedTicker);
    else
        mPlot->xAxis->setTicker(fixedTicker);
    fixedTicker->setTickStep(5.0); // tick step shall be 1.0


    // Place labels and hide/show axes and ticks
    if(orientation==Qt::Orientation::Vertical)
    {
        mPlot->xAxis2->setLabel("dBFS");
        mPlot->xAxis2->setVisible(true);
        mPlot->xAxis2->setTickPen(QPen(Qt::NoPen));
        mPlot->xAxis2->setSubTickPen(QPen(Qt::NoPen));
        mPlot->xAxis2->setLabelColor(Qt::white);
    }
    else
    {
        mPlot->yAxis2->setLabel("dBFS");
        mPlot->yAxis2->setVisible(true);
        mPlot->yAxis2->setTickPen(QPen(Qt::NoPen));
        mPlot->yAxis2->setSubTickPen(QPen(Qt::NoPen));
        mPlot->yAxis2->setTickLabels(false);
        mPlot->yAxis2->setLabelColor(Qt::white);
    }

    //Adjust size
    if(orientation==Qt::Orientation::Vertical)
    {
        this->setMinimumWidth(this->minimumWidth());
        this->resize(this->minimumWidth(),this->height());
    }
    else
    {
        this->setMinimumHeight(this->minimumWidth());
        this->resize(this->width(),this->minimumWidth());
    }


    // -- Debug --
    /*   QVector<double> x;
    x.append(0);
    QVector<double> y;
    y.append(-20);
    newbars->setData(x,y);*/

    mLayout->addWidget(mPlot,1); // ..,1 for Stretch
    return mLayout;


}

void QFLevelMeter::changeOrientation(const Qt::DockWidgetArea area)
{

    if (Qt::DockWidgetArea::TopDockWidgetArea == area ||Qt::DockWidgetArea::BottomDockWidgetArea == area )
        this->setLayout(getOrientedLayout(Qt::Orientation::Horizontal));//setHorizontalOrientation();

    if (Qt::DockWidgetArea::RightDockWidgetArea == area ||Qt::DockWidgetArea::LeftDockWidgetArea == area )
        this->setLayout(getOrientedLayout(Qt::Orientation::Vertical));//setVerticalOrientation();

}

void QFLevelMeter::onEnableToggle(bool checked)
{
    qDebug() << "Enable is " << checked;
    this->setLevel(-96);
    mPlot->setEnabled(checked);
    enableChanged(checked);
}

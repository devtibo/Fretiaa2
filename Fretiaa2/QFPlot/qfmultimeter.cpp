#include "qfmultimeter.h"
#include <QGridLayout>


QFMultiMeter::QFMultiMeter( QWidget *parent) : QWidget(parent), _unit("Pa"), _maxValue(0), _minValue(0), _avgValue(0), _absValue(0)
{
    // Create widgetsavefilefile and the layout

    this->setLayout(createLayout(Qt::Orientation::Horizontal));

}



/** =============================== **/
/** =========== SLOTS ============= **/
/** =============================== **/

void QFMultiMeter::meterChangeOrientation(Qt::Orientation orientation)
{

    qDeleteAll(this->children());
    this->setLayout(createLayout(orientation));

}

QLayout *QFMultiMeter::createLayout(Qt::Orientation orientation)
{


    lAbsValue = new QLabel();
    lAvgValue = new QLabel();
    lMinValue = new QLabel();
    lMaxValue = new QLabel();
updateLabels();

    setUnit(_unit);

    QBoxLayout *m_layout ;

    if(orientation==Qt::Orientation::Vertical)
        m_layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    else
        m_layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);

    QFont f("monospace");
    f.setItalic(true);
    f.setPointSize(f.pointSize()-4);

    QFont f2("monospace");
    f2.setItalic(false);
    f2.setPointSize(f2.pointSize()-3);

    QLabel *lMaxText = new QLabel("Max",this);
    lMaxText->setFont(f);
    lMaxUnit->setFont(f2);
    lMaxValue->setFont(f2);
    lMaxValue->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *lMinText = new QLabel("Min",this);
    lMinText->setFont(f);
    lMinUnit->setFont(f2);
    lMinValue->setFont(f2);
    lMinValue->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *lAvgText = new QLabel("Avg",this);
    lAvgText->setFont(f);
    lAvgUnit->setFont(f2);
    lAvgValue->setFont(f2);
    lAvgValue->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *lAbsText = new QLabel("RMS",this);
    lAbsText->setFont(f);
    lAbsUnit->setFont(f2);
    lAbsValue->setFont(f2);
    lAbsValue->setTextInteractionFlags(Qt::TextSelectableByMouse);




    QGridLayout *maxLayout = new QGridLayout();

    maxLayout->addWidget(lMaxText,0,0,1,2,Qt::AlignCenter);
    maxLayout->addWidget(lMaxValue,1,0,1,1,Qt::AlignRight);
    maxLayout->addWidget(lMaxUnit,1,1,1,1,Qt::AlignLeft);
    maxLayout->setSpacing(0);
    maxLayout->setMargin(0);

    m_layout->addLayout(maxLayout);


    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    m_layout->addWidget(line);


    QGridLayout *minLayout = new QGridLayout();
    minLayout->addWidget(lMinText,0,0,1,2,Qt::AlignCenter);
    minLayout->addWidget(lMinValue,1,0,1,1,Qt::AlignRight);
    minLayout->addWidget(lMinUnit,1,1,1,1,Qt::AlignLeft);
    minLayout->setSpacing(0);
    minLayout->setMargin(0);

    m_layout->addLayout(minLayout);


    QFrame *line2 = new QFrame();
    line2->setFrameShape(QFrame::VLine);
    line2->setFrameShadow(QFrame::Sunken);
    m_layout->addWidget(line2);

    QGridLayout *avgLayout = new QGridLayout();
    avgLayout->addWidget(lAvgText,0,0,1,2,Qt::AlignCenter);
    avgLayout->addWidget(lAvgValue,1,0,1,1,Qt::AlignRight);
    avgLayout->addWidget(lAvgUnit,1,1,1,1,Qt::AlignLeft);
    avgLayout->setSpacing(0);
    avgLayout->setMargin(0);

    m_layout->addLayout(avgLayout);


    QFrame *line3 = new QFrame();
    line3->setFrameShape(QFrame::VLine);
    line3->setFrameShadow(QFrame::Sunken);
    m_layout->addWidget(line3);


    QGridLayout *absLayout = new QGridLayout();
    absLayout->addWidget(lAbsText,0,0,1,2,Qt::AlignCenter);
    absLayout->addWidget(lAbsValue,1,0,1,1,Qt::AlignRight);
    absLayout->addWidget(lAbsUnit,1,1,1,1,Qt::AlignLeft);
    absLayout->setSpacing(0);
    absLayout->setMargin(0);

    m_layout->addLayout(absLayout);

    m_layout->setAlignment(Qt::AlignTop);


    if(orientation == Qt::Orientation::Vertical)
    {
        m_layout->setDirection(QBoxLayout::Direction::TopToBottom);
        line->setFrameShape(QFrame::HLine);
        line2->setFrameShape(QFrame::HLine);
        line3->setFrameShape(QFrame::HLine);
    }
    else
    {
        m_layout->setDirection(QBoxLayout::Direction::LeftToRight);
        line->setFrameShape(QFrame::VLine);
        line2->setFrameShape(QFrame::VLine);
        line3->setFrameShape(QFrame::VLine);
    }


    return m_layout;
}

void QFMultiMeter::updateLabels()
{
    lAbsValue->setText(QString().sprintf("%+02.2f", _absValue));
    lAvgValue->setText(QString().sprintf("%+02.2f", _avgValue));
    lMinValue->setText(QString().sprintf("%+02.2f", _minValue));
    lMaxValue->setText(QString().sprintf("%+02.2f", _maxValue));

}

QString QFMultiMeter::unit() const
{
    return _unit;
}

void QFMultiMeter::setUnit(const QString &unit)
{
    _unit = unit;
    lAbsUnit = new QLabel(" " + _unit);
    lAvgUnit= new QLabel(" " + _unit);
    lMinUnit= new QLabel(" " + _unit);
    lMaxUnit= new QLabel(" " + _unit);
}

double QFMultiMeter::absValue() const
{
    return _absValue;
}

void QFMultiMeter::setAbsValue(double absValue)
{
    _absValue = absValue;
updateLabels();
}

double QFMultiMeter::avgValue() const
{
    return _avgValue;
}

void QFMultiMeter::setAvgValue(double avgValue)
{
    _avgValue = avgValue;
 updateLabels();
}

double QFMultiMeter::minValue() const
{
    return _minValue;
}

void QFMultiMeter::setMinValue(double minValue)
{
    _minValue = minValue;
updateLabels();
}

double QFMultiMeter::maxValue() const
{
    return _maxValue;
}

void QFMultiMeter::setMaxValue(double maxValue)
{
    _maxValue = maxValue;
updateLabels();
}


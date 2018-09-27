#include "gainswidget.h"

GainsWidget::GainsWidget(QWidget *parent) : QWidget(parent)
{
    setLayout(createLayout(Qt::Orientation::Horizontal));
}


QLayout *GainsWidget::createLayout(Qt::Orientation orientation)
{
    // Delete existing layout
    qDeleteAll(this->children());

    // Set the Orientation
    QBoxLayout *m_layout ;
    if(orientation==Qt::Orientation::Vertical)
        m_layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    else
        m_layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);

    // Double type input RegExp
    QRegExpValidator* rxv = new QRegExpValidator(QRegExp("[+-]?\\d*[\\.]?\\d+"), this); // Because QDoubleValidator imposes coma separator which cause double convertion error


    // === mic Sensisitivity ===
    QLabel *micSensitivity_label = new QLabel("Mic Sensitivity", this);
    micSensitivity_edit = new QLineEdit("1",this);
    micSensitivity_edit->setValidator(rxv);
    micSensitivity_combo = new QComboBox(this);
    micSensitivity_combo->addItems(QStringList() << "V/Pa" << "mV/Pa");

    QBoxLayout *micSensitivity_layout ;
    // if(orientation==Qt::Orientation::Vertical)
    //     micSensitivity_layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    // else
    micSensitivity_layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
    micSensitivity_layout->addWidget(micSensitivity_edit);
    micSensitivity_layout->addWidget(micSensitivity_combo);
    connect(micSensitivity_edit,SIGNAL(editingFinished()),this,SLOT(onUpdate()));
    connect(micSensitivity_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(onUpdate()));


    // === PreAamplification Gain ===
    QLabel *preAmpGain_label = new QLabel("PreAmp. Gain", this);
    preAmpGain_edit = new QLineEdit("0",this);
    preAmpGain_edit->setValidator(rxv);
    preAmpGain_combo = new QComboBox(this);
    preAmpGain_combo->addItems(QStringList() <<"dB" << "dBu" << "dBV" << "Lin.");
    QBoxLayout *preAmpGain_layout ;
    //if(orientation==Qt::Orientation::Vertical)
    //     preAmpGain_layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    //else
    preAmpGain_layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
    preAmpGain_layout->addWidget(preAmpGain_edit);
    preAmpGain_layout->addWidget(preAmpGain_combo);
    connect(preAmpGain_edit,SIGNAL(editingFinished()),this,SLOT(onUpdate()));
    connect(preAmpGain_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(onUpdate()));


    // === ADC input Gain ===
    QLabel *adcInGain_label = new QLabel("ADC Input Gain", this);
    adcInGain_edit = new QLineEdit("0",this);
    adcInGain_edit->setValidator(rxv);
    adcInGain_combo = new QComboBox(this);
    adcInGain_combo->addItems(QStringList() <<"dB" << "dBu" << "dBV" << "Lin.");
    QBoxLayout *adcInGain_layout ;
    // if(orientation==Qt::Orientation::Vertical)
    //    adcInGain_layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    //else
    adcInGain_layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
    adcInGain_layout->addWidget(adcInGain_edit);
    adcInGain_layout->addWidget(adcInGain_combo);
    connect(adcInGain_edit,SIGNAL(editingFinished()),this,SLOT(onUpdate()));
    connect(adcInGain_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(onUpdate()));

    // === Main Layout ====
    m_layout->addWidget(micSensitivity_label);
    m_layout->addLayout(micSensitivity_layout);
    m_layout->addStretch();

    m_layout->addWidget(preAmpGain_label);
    m_layout->addLayout(preAmpGain_layout);
    m_layout->addStretch();

    m_layout->addWidget(adcInGain_label);
    m_layout->addLayout(adcInGain_layout);
    m_layout->addStretch();

    return m_layout;
}

double GainsWidget::gain() const
{
    return _gain;
}

void GainsWidget::onUpdate()
{
    bool ok;
    double micSensitivity = micSensitivity_edit->text().toDouble(&ok);
    double preAmpGain = preAmpGain_edit->text().toDouble(&ok);
    double adcInGain = adcInGain_edit->text().toDouble(&ok);

    qDebug() << "Mic Sensitivity: " << QString::number(micSensitivity) << micSensitivity_combo->currentText()\
             << "PreAmpGain: " << QString::number(preAmpGain) << preAmpGain_combo->currentText()\
             << "ADCInGain: " << QString::number(adcInGain) << adcInGain_combo->currentText();


    if (micSensitivity_combo->currentIndex()==1) // i.e. mv/Pa
        micSensitivity /= 1000;


    switch(preAmpGain_combo->currentIndex())
    {
    case 0: // i.e. dB
        preAmpGain = qPow(10.0,preAmpGain/20.0);
        break;
    case 1: // i.e. dBu
        preAmpGain = 0.7746 * qPow(10.0,(preAmpGain)/20.0);
        break;
    case 2:// i.e. dBV
        preAmpGain = qPow(10.0,preAmpGain/20.0);
        break;
    case 3:// i.e. Lin.
        break;
    }


    switch(adcInGain_combo->currentIndex())
    {
    case 0: // i.e. dB
        adcInGain = qPow(10.0,adcInGain/20.0);
        break;
    case 1: // i.e. dBu
        adcInGain = 0.7746 * qPow(10.0,(adcInGain)/20.0);
        break;
    case 2:// i.e. dBV
        adcInGain = qPow(10.0,adcInGain/20.0);
        break;
    case 3:// i.e. Lin.
        break;
    }

    // Compute global gain
    _gain = micSensitivity * preAmpGain * adcInGain;

    // Emit signal
    emit gainChanged(_gain);
}

void GainsWidget::changeOrientation(const Qt::DockWidgetArea area)
{
    if (Qt::DockWidgetArea::TopDockWidgetArea == area ||Qt::DockWidgetArea::BottomDockWidgetArea == area )
        this->setLayout(createLayout(Qt::Orientation::Horizontal));//setHorizontalOrientation();

    if (Qt::DockWidgetArea::RightDockWidgetArea == area ||Qt::DockWidgetArea::LeftDockWidgetArea == area )
        this->setLayout(createLayout(Qt::Orientation::Vertical));//setVerticalOrientation();
}

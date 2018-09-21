#include "inputaudiodevicedialog.h"


InputAudioDeviceDialog::InputAudioDeviceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Audio Device Configuration");


    mDevicecombo  =new QComboBox;
    mBufferSizeCombo  =new QComboBox;
    mCodecCombo = new QComboBox;
    mFrequencyCombo = new QComboBox;
    mChannelCombo = new QComboBox;
    mSampleTypeCombo = new QComboBox;
    mSampleSizeCombo = new QComboBox;
    mEndianessCombo = new QComboBox;




    mBufferSizeCombo->addItem("128");
    mBufferSizeCombo->addItem("256");
    mBufferSizeCombo->addItem("512");
    mBufferSizeCombo->addItem("1024");
    mBufferSizeCombo->addItem("2048");
    mBufferSizeCombo->addItem("4096");
    mBufferSizeCombo->addItem("8192");



    /* Header */
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QHBoxLayout *headerLayout2 = new QHBoxLayout();
    QVBoxLayout *headerVLayout = new QVBoxLayout();
    QLabel *deviceLabel  =new QLabel("Device");

    QLabel *bufferSizeLabel = new QLabel("Buffer Size in bits");

    headerLayout->addWidget(deviceLabel);
    headerLayout->addWidget(mDevicecombo);
    headerLayout2->addWidget(bufferSizeLabel);
    headerLayout2->addWidget(mBufferSizeCombo);

    headerVLayout->addLayout(headerLayout);
    headerVLayout->addLayout(headerLayout2);
    QFrame *headerFrame = new QFrame();
    headerFrame->setLayout(headerVLayout);

    /* Config Frame */
    QVBoxLayout *mVLayout =  new QVBoxLayout;
    QVBoxLayout *mVLayout2 =  new QVBoxLayout;
    QHBoxLayout *mHLayout =  new QHBoxLayout;


    mVLayout->addWidget(mCodecCombo);
    mVLayout->addWidget(mFrequencyCombo);
    mVLayout->addWidget(mChannelCombo);
    mVLayout->addWidget(mSampleTypeCombo);
    mVLayout->addWidget(mSampleSizeCombo);
    mVLayout->addWidget(mEndianessCombo);

    QLabel *mCodecLabel = new QLabel("Codec");
    QLabel *mFrequencyLabel = new QLabel("Frequency");
    QLabel *mChannelLabel = new QLabel("Channel");
    QLabel *mSampleTypeLabel = new QLabel("Sample Type");
    QLabel *mSampleSizeLabel = new QLabel("Sample Size");
    QLabel *mEndianessLabel = new QLabel("Endianess");
    mVLayout2->addWidget(mCodecLabel);
    mVLayout2->addWidget(mFrequencyLabel);
    mVLayout2->addWidget(mChannelLabel);
    mVLayout2->addWidget(mSampleTypeLabel);
    mVLayout2->addWidget(mSampleSizeLabel);
    mVLayout2->addWidget(mEndianessLabel);

    mHLayout->addLayout(mVLayout2);
    mHLayout->addLayout(mVLayout);

    QFrame *configFrame = new QFrame;
    configFrame->setLayout(mHLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    /* Main Layout*/
    QVBoxLayout *mainVLayout = new QVBoxLayout;
    mainVLayout->addWidget(headerFrame);
    mainVLayout->addWidget(configFrame);
    mainVLayout->addWidget(buttonBox);
    configFrame->setFrameStyle(QFrame::Box);
    setLayout(mainVLayout);

    /* Connections */
    connect(mDevicecombo, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceChanged(int)));
    connect(mFrequencyCombo,SIGNAL(currentIndexChanged(int)), this, SLOT(onFrequencyChanged(int)));
    connect(mSampleTypeCombo,SIGNAL(currentIndexChanged(int)), this, SLOT(onSampleTypeChanged(int)));
    connect(mChannelCombo,SIGNAL(currentIndexChanged(int)), this, SLOT(onChannelChanged(int)));
    connect(mCodecCombo,SIGNAL(currentIndexChanged(int)), this, SLOT(onCodecChanged(int)));
    connect(mEndianessCombo,SIGNAL(currentIndexChanged(int)), this, SLOT(onEndiannessChanged(int)));
    connect(mSampleSizeCombo,SIGNAL(currentIndexChanged(int)), this, SLOT(onSampleSizeChanged(int)));
    connect(mBufferSizeCombo,SIGNAL(currentIndexChanged(int)), this, SLOT(onBufferSizeChanged(int)));


    mBufferSizeCombo->setCurrentIndex(2); // Need to be called after connextion


    QAudio::Mode mode;
    mode =QAudio::AudioInput;
    for (auto &deviceInfo: QAudioDeviceInfo::availableDevices(mode))
        mDevicecombo->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));

    mDevicecombo->setCurrentIndex(0);
    deviceChanged(0);

    // TODO: more elegant by searching if 16 bits exists
    if(mSampleSizeCombo->count()>=1) //Set 16 bits if exist
        mSampleSizeCombo->setCurrentIndex(1);


}

void InputAudioDeviceDialog::deviceChanged(int idx)
{
    if (mDevicecombo->count() == 0)
        return;

    // Show a progress bar because this can take a while
    QFuture<void> future = QtConcurrent::run(runProgress);

    /* Update ComboBoxes*/
    m_deviceInfo = mDevicecombo->itemData(idx).value<QAudioDeviceInfo>();

    // Update Sampling Frequency Combo
    mFrequencyCombo->clear();
    QList<int> sampleRatez = m_deviceInfo.supportedSampleRates();
    for (int i = 0; i < sampleRatez.size(); ++i)
        mFrequencyCombo->addItem(QString("%1").arg(sampleRatez.at(i)));
    if (sampleRatez.size())
        m_settings.setSampleRate(sampleRatez.at(0));

    // Update Number of Channel Combo
    mChannelCombo->clear();
    QList<int> chz = m_deviceInfo.supportedChannelCounts();
    for (int i = 0; i < chz.size(); ++i)
        mChannelCombo->addItem(QString("%1").arg(chz.at(i)));
    if (chz.size())
        m_settings.setChannelCount(chz.at(0));

    // Update Codec
    mCodecCombo->clear();
    QStringList codecs = m_deviceInfo.supportedCodecs();
    for (int i = 0; i < codecs.size(); ++i)
        mCodecCombo->addItem(QString("%1").arg(codecs.at(i)));
    if (codecs.size())
        m_settings.setCodec(codecs.at(0));

    // Update sample size
    mSampleSizeCombo->clear();
    QList<int> sampleSizez = m_deviceInfo.supportedSampleSizes();
    for (int i = 0; i < sampleSizez.size(); ++i)
        mSampleSizeCombo->addItem(QString("%1").arg(sampleSizez.at(i)));
    if (sampleSizez.size())
        m_settings.setSampleSize(sampleSizez.at(0));

    // Update Sample type
    mSampleTypeCombo->clear();
    QList<QAudioFormat::SampleType> sampleTypez = m_deviceInfo.supportedSampleTypes();

    for (int i = 0; i < sampleTypez.size(); ++i)
        mSampleTypeCombo->addItem(toString(sampleTypez.at(i)));
    if (sampleTypez.size())
        m_settings.setSampleType(sampleTypez.at(0));

    // Update Endianess
    mEndianessCombo->clear();
    QList<QAudioFormat::Endian> endianz = m_deviceInfo.supportedByteOrders();
    for (int i = 0; i < endianz.size(); ++i)
        mEndianessCombo->addItem(toString(endianz.at(i)));
    if (endianz.size())
        m_settings.setByteOrder(endianz.at(0));
}

void InputAudioDeviceDialog::getConfiguration(QAudioDeviceInfo &mAudioDeviceInfo, QAudioFormat &mAudioFormat, unsigned int &mBufferSize){
    mAudioDeviceInfo = m_deviceInfo;
    mAudioFormat = m_settings;
    mBufferSize = bufferSize;
}


void InputAudioDeviceDialog::setConfiguration(QAudioDeviceInfo &mAudioDeviceInfo, QAudioFormat &mAudioFormat, unsigned int &mBufferSize){
    m_deviceInfo  =mAudioDeviceInfo;;
    m_settings = mAudioFormat;
    bufferSize = mBufferSize;
}


QString InputAudioDeviceDialog::toString(QAudioFormat::SampleType sampleType)
{
    QString result("Unknown");
    switch (sampleType) {
    case QAudioFormat::SignedInt:
        result = "SignedInt";
        break;
    case QAudioFormat::UnSignedInt:
        result = "UnSignedInt";
        break;
    case QAudioFormat::Float:
        result = "Float";
        break;
    case QAudioFormat::Unknown:
        result = "Unknown";
    }
    return result;
}

QString InputAudioDeviceDialog::toString(QAudioFormat::Endian endian)
{
    QString result("Unknown");
    switch (endian) {
    case QAudioFormat::LittleEndian:
        result = "LittleEndian";
        break;
    case QAudioFormat::BigEndian:
        result = "BigEndian";
        break;
    }
    return result;
}

void runProgress()
{
    QProgressDialog dialog;
    dialog.setModal(true);
    dialog.setRange(0,0);
    dialog.show();
    QApplication::processEvents();
}


void InputAudioDeviceDialog::accept()
{
    qDebug() << "[INFO] Input audio config changed to : " << m_settings;
    if(m_deviceInfo.isFormatSupported(m_settings))
    {
        done(Accepted);
    } else
    {
        QMessageBox msgBox;
        QString msg;
        msg = "Format not supported";

        QAudioFormat nearest = m_deviceInfo.nearestFormat(m_settings);
        if(nearest.sampleRate() != m_settings.sampleRate())
        {msg += "\n - Change Sample Rate to " + QString::number(nearest.sampleRate()) + " Hz";}
        if(nearest.sampleSize() != m_settings.sampleSize())
        {msg += "\n - Change Sample Size to " + QString::number(nearest.sampleSize()) + "  bits";}
        if(nearest.sampleType() != m_settings.sampleType())
        {msg += "\n - Change Sample Type to " + toString(nearest.sampleType()); }
        if(nearest.codec() != m_settings.codec())
        {msg += "\n - Change Sample Codec to " + nearest.codec() ;}
        if(nearest.channelCount() != m_settings.channelCount())
        {msg += "\n - Change Channel to " + QString::number(nearest.channelCount())  ;}
        if(nearest.byteOrder() != m_settings.byteOrder())
        {msg += "\n - Change Endianess to " + toString(nearest.byteOrder());  ;}

        msgBox.setText(msg);
        msgBox.move(this->pos().x()+this->width()/2, this->pos().y()+this->height()/2);
        msgBox.exec();
    }
}

void InputAudioDeviceDialog::onFrequencyChanged(int idx)
{
    if(idx>=0)
        m_settings.setSampleRate(mFrequencyCombo->itemText(idx).toInt());
}
void InputAudioDeviceDialog::onSampleTypeChanged(int idx)
{
    if(idx>=0)
        m_settings.setSampleType(m_deviceInfo.supportedSampleTypes().at(idx));

    // TODO if float set to 32bits
    if(m_deviceInfo.supportedSampleTypes().at(idx)==QAudioFormat::Float)
        ;
}
void InputAudioDeviceDialog::onChannelChanged(int idx)
{
    if(idx>=0)
        m_settings.setChannelCount(mChannelCombo->itemText(idx).toInt());
}
void InputAudioDeviceDialog::onCodecChanged(int idx)
{
    if(idx>=0)
        m_settings.setCodec(mCodecCombo->itemText(idx));

}
void InputAudioDeviceDialog::onEndiannessChanged(int idx)
{
    if(idx>=0)
        m_settings.setByteOrder( m_deviceInfo.supportedByteOrders().at(idx));
}
void InputAudioDeviceDialog::onSampleSizeChanged(int idx)
{
    if(idx>=0)
        m_settings.setSampleSize(mSampleSizeCombo->itemText(idx).toInt());

    // TODO if 32-bit set to float
    if(mSampleSizeCombo->currentText().toInt()==32)
        ;
}

void InputAudioDeviceDialog::onBufferSizeChanged(int idx)
{
    if(idx>=0)
        bufferSize = mBufferSizeCombo->currentText().toUInt();

}

#ifndef InputAudioDeviceDialog_H
#define InputAudioDeviceDialog_H
#include <QDialog>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include<QAudioDeviceInfo>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDebug>

#include <QProgressDialog>
#include <QFuture>
#include <QtConcurrentRun>
#include <QApplication>
void runProgress();

class InputAudioDeviceDialog : public QDialog
{
    Q_OBJECT
public:
    InputAudioDeviceDialog(QWidget *parent = 0);


    void getConfiguration(QAudioDeviceInfo&, QAudioFormat&, unsigned int&);
    void setConfiguration(QAudioDeviceInfo&, QAudioFormat&, unsigned int&);



private:

    QComboBox *mDevicecombo;
    QComboBox *mBufferSizeCombo;
    QComboBox *mCodecCombo;
    QComboBox *mFrequencyCombo;
    QComboBox *mChannelCombo;
    QComboBox *mSampleTypeCombo;
    QComboBox *mSampleSizeCombo;
    QComboBox *mEndianessCombo;

    QAudioFormat m_settings;
    QAudioDeviceInfo m_deviceInfo ;
    unsigned int bufferSize;

    QString toString(QAudioFormat::SampleType sampleType);

    QString toString(QAudioFormat::Endian endian);


public slots:
    void deviceChanged(int idx);
    void accept();
    //  void reject();
    void onFrequencyChanged(int idx);
    void onSampleTypeChanged(int idx);
    void onChannelChanged(int idx);
    void onCodecChanged(int idx);
    void onEndiannessChanged(int idx);
    void onSampleSizeChanged(int idx);
    void onBufferSizeChanged(int idx);
};

#endif // InputAudioDeviceDialog_H

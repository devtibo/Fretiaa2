#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QLabel>
#include <QStringList>
#include <QVector>
#include "aboutdialog.h"
#include "QFPlot/qfplot.h"
#include "inputaudiodevicedialog.h"
#include "gainswidget.h"
#include <QAudioInput>
#include <QIODevice>
#include <QtMath>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    /*Main graphic*/
    QFOscillogram *oscillogramWidget;
    QFLevelMeter *levelMeterWidget;

    /* Create the menu bar of the application */
    void createMenuBar(void);

    /* Create application tool bar */
    void createToolBar(void);


    QLabel *audioConfig;


    // ----AUDIO ENGINE ----
    QIODevice *m_InputDevice;
    QAudioInput *m_AudioInput;
    int soundBufferSize;
    QVector<double> xVecBase;
    char *dataSound;
    void initAudio();
    double Fs;

    // === data file ===
    QFile *mFileData;

    void closeEvent(QCloseEvent *event);


public slots:

    /*Menu Bar slots*/
    void about();
    void onAudioConfigActionTriggered(bool);


    void onLiveViewActionToggled(bool);
    void onAppToolBarOrienationChanged(Qt::Orientation);
    void onTriggerActionToggled(bool);
    void onROIActionToggled(bool);

    void onReadyRead();

    void updateLevelMeter();
    void onLevelMeterEnableChanged(bool);

    void onRefreshGraphs();


};



#endif // MAINWINDOW_H

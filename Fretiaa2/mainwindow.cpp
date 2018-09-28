#include "mainwindow.h"
#include <QMenuBar>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // === Central Widget: Oscillogram ===
    oscillogramWidget = new QFOscillogram(this);
    setCentralWidget(oscillogramWidget);
    connect(oscillogramWidget,SIGNAL(availableSelectRectangleData()),this,SLOT(updateLevelMeter()));

    // === Open Temporary file to store data in order to reduce reduce reploting computation load and to not saturated the RAMemory ===
    mFileData = new QFile("data.bin");
    mFileData->open(QFile::ReadWrite | QIODevice::Truncate);

    // === Status bar ===
    audioConfig = new QLabel("audio config...");
    audioConfig->setStyleSheet("QLabel {color : #151515; }");
    audioConfig->setAlignment(Qt::AlignLeft);
    QStatusBar *mStatusBar = new QStatusBar(this);
    mStatusBar->addWidget(audioConfig,1);
    mStatusBar->setStyleSheet(" color : blue;");
    this->setStatusBar(mStatusBar);

    // === DockWidget: LevelMeter ===
    levelMeterWidget = new QFLevelMeter(Qt::Orientation::Vertical,this);

    //Enable/Disable connection
    connect(levelMeterWidget,SIGNAL(enableChanged(bool)),this,SLOT(onLevelMeterEnableChanged(bool)));

    //TODO disable widget when closed!
    QDockWidget *levelMeterDock = new QDockWidget("VU-Meter",this);
    levelMeterDock->setWidget(levelMeterWidget);
    this->addDockWidget(Qt::RightDockWidgetArea, levelMeterDock);

    int w = levelMeterDock->sizeHint().width();
    levelMeterDock->setMinimumWidth(w); // Set minimum size
    levelMeterWidget->setMinimumWidth(w);
    levelMeterWidget->resize(QSize(0,0));
    levelMeterDock->resize(QSize(0,0));
    levelMeterWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    levelMeterDock->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    connect(levelMeterDock,SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),levelMeterWidget,SLOT(changeOrientation(Qt::DockWidgetArea)));
    levelMeterWidget->setLevel(-96);

    // === DockWidget: Gain ===
    GainsWidget *gainsWidget = new GainsWidget(this);

    QDockWidget *gainsDock = new QDockWidget("Gains settings",this);
    gainsDock->setWidget(gainsWidget);
    this->addDockWidget(Qt::BottomDockWidgetArea, gainsDock);

    connect(gainsDock,SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),gainsWidget,SLOT(changeOrientation(Qt::DockWidgetArea)));

    connect(gainsWidget,SIGNAL(gainChanged(double)),oscillogramWidget,SLOT(changeGain(double))); // Connect to gain widget
    // === ToolBar menu: Create ToolBar ===
    createToolBar();

    // === Menu Bar: Create menubar ===
    createMenuBar(); // Need to be called after DowkWidgets creation and after toolBar creation


    // === AudioInput: Open Input Device ===
    initAudio();


    //=== TIMER to refresh graphs ===
    QTimer *mTimer = new QTimer();
    mTimer->setInterval(100);
    mTimer->start();
    connect(mTimer,SIGNAL(timeout()), this, SLOT(onRefreshGraphs()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::createMenuBar()
{
    // === Menu "File" ===
    QMenu *fileMenu= menuBar()->addMenu(tr("&File"));
    // - Quit Action
    QAction *quitAction = new QAction("&Quit",fileMenu);
    quitAction->setIcon(QIcon(":icons/iconQuit.png"));
    fileMenu->addAction(quitAction);
    quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(quitAction,SIGNAL(triggered(bool)),this,SLOT(close()));

    // === Menu "Tools" ===
    QMenu *viewMenu= menuBar()->addMenu(tr("&View"));
    viewMenu->addActions(createPopupMenu()->actions());

    // === Menu "Tools" ===
    QMenu *toolsMenu= menuBar()->addMenu(tr("&Tools"));
    QAction *audioConfigAction = new QAction("Audio Config.");
    toolsMenu->addAction(audioConfigAction);
    audioConfigAction->setEnabled(false);
    //connect(audioConfigAction,SIGNAL(triggered(bool)),this,SLOT(onAudioConfigActionTriggered(bool)));

    // === Menu "Help" ===
    QMenu *helpMenu= menuBar()->addMenu(tr("&Help"));
    // - About Action
    QAction *aboutAction = new QAction("&About",helpMenu);
    aboutAction->setIcon(QIcon(":icons/iconAbout.png"));
    helpMenu->addAction(aboutAction);
    connect(aboutAction,SIGNAL(triggered(bool)),this,SLOT(about()));

}

void MainWindow::createToolBar()
{
    QSize appToolBar_IsonSize = QSize(30,30);
    QToolBar *appToolBar = new QToolBar("Controls",this);

    appToolBar->setFloatable(true);
    appToolBar->setIconSize(appToolBar_IsonSize);
    addToolBar(Qt::ToolBarArea::LeftToolBarArea,appToolBar);

    // === TOOL : "Live" ===
    QAction *liveViewAction = new QAction(appToolBar);
    liveViewAction->setIcon(QIcon(":icons/liveViewOnColoredOn.png"));
    liveViewAction->setCheckable(true);
    liveViewAction->setText("Live View On/Off");
    connect(liveViewAction,SIGNAL(toggled(bool)),this, SLOT(onLiveViewActionToggled(bool)));
    appToolBar->addAction(liveViewAction);

    // === TOOL : "Trigger" ===
    QAction *triggerAction = new QAction(appToolBar);
    triggerAction->setIcon(QIcon(":icons/iconTrigger.png"));
    triggerAction->setCheckable(true);
    triggerAction->setText("Trigger On/Off");
    appToolBar->addAction(triggerAction);
    connect(triggerAction,SIGNAL(toggled(bool)),this, SLOT(onTriggerActionToggled(bool)));
    triggerAction->setEnabled(false); //TODO  Trigger

    // === TOOL : "ROI" ===
    QAction *ROIAction = new QAction(appToolBar);
    ROIAction->setObjectName("ROIAction");
    ROIAction->setIcon(QIcon(":icons/iconMoceRect.png"));
    ROIAction->setCheckable(true);
    ROIAction->setText("Move Region Of Interest");
    appToolBar->addAction(ROIAction);
    connect(ROIAction,SIGNAL(toggled(bool)),this, SLOT(onROIActionToggled(bool)));
    ROIAction->setDisabled(true);
    connect(liveViewAction,SIGNAL(toggled(bool)),ROIAction,SLOT(setEnabled(bool))); // Not enable in livemode

    // === TOOL : "HP Filter" ===
    QBoxLayout *HPFilterActionLayout = new QBoxLayout(QBoxLayout::TopToBottom, appToolBar);
    HPFilterActionLayout->setObjectName("HPFilterActionLayout");

    QFrame * HPFilterFrame = new QFrame(appToolBar);
    HPFilterFrame->setFrameShape(QFrame::WinPanel);
    HPFilterFrame->setFrameShadow(QFrame::Raised);
    HPFilterFrame->setLayout(HPFilterActionLayout);

    QComboBox *HPFilterFcCombo = new QComboBox(HPFilterFrame);
    HPFilterFcCombo->addItems(QStringList() << "20" << "50" << "80");
    HPFilterFcCombo->setItemData(0,20); HPFilterFcCombo->setItemData(0,50); HPFilterFcCombo->setItemData(0,80);
    HPFilterFcCombo->setEnabled(false);
    QFont mFont = HPFilterFcCombo->font();
    mFont.setPointSize(mFont.pointSize()-1);
    HPFilterFcCombo->setFont(mFont);

    QToolButton *HPFilterButton = new QToolButton(HPFilterFrame); // FIXME : Text doesn't appear when mouse is on the button
    HPFilterButton->setIcon(QIcon(":icons/iconHP.png"));
    HPFilterButton->setIconSize(appToolBar_IsonSize);
    HPFilterButton->setCheckable(true);
    HPFilterButton->setText("High-Pass Filter");

    QLabel * HPFilterLabel =new  QLabel("Fc (Hz)",HPFilterFrame);
    QFont HPfont = HPFilterLabel->font();
    HPfont.setPointSize(HPfont.pointSize()-1);
    HPFilterLabel->setFont(HPfont);
    HPFilterFcCombo->setFont(HPfont);

    HPFilterActionLayout->addWidget(HPFilterButton,0,Qt::AlignCenter);
    HPFilterActionLayout->addWidget(HPFilterLabel,0,Qt::AlignCenter);
    HPFilterActionLayout->addWidget(HPFilterFcCombo,0,Qt::AlignCenter);
    HPFilterActionLayout->setAlignment(Qt::AlignCenter);

    HPFilterFrame->setEnabled(false); //TODO : HP filter
    appToolBar->addWidget(HPFilterFrame);

    connect(HPFilterButton,SIGNAL(toggled(bool)),HPFilterFcCombo,SLOT(setEnabled(bool)));

    // TODO : find more elegant way : e.g. myQBoxLayout with a slot change orientation
    connect(appToolBar,SIGNAL(orientationChanged(Qt::Orientation)),this,SLOT(onAppToolBarOrienationChanged(Qt::Orientation)));
}



void MainWindow::initAudio()
{
    soundBufferSize = 8192;

    // === Open default input Device with preffered Format ===
    //- get default device
    QAudioDeviceInfo m_deviceInfo;
    m_deviceInfo = m_deviceInfo.defaultInputDevice();
    // - get preffered format
    QAudioFormat prefferedFormat = m_deviceInfo.preferredFormat();
    prefferedFormat.setChannelCount(1); // Force to mono
    Fs = prefferedFormat.sampleRate();
    //- Open Input Device
    m_AudioInput = new QAudioInput(m_deviceInfo, prefferedFormat, this);
    m_AudioInput->setBufferSize(soundBufferSize); // To be clarified : just need to be bigger enough according to notfityinterval?
    // -Config IO Device
    m_InputDevice = m_AudioInput->start();
    m_InputDevice->open(QIODevice::ReadOnly);

    // Connection
    connect(m_InputDevice, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    // Init Vectors
    dataSound = new char[soundBufferSize];
    xVecBase.clear();
    for (int i=0;i<(soundBufferSize/2);i++)
        xVecBase.append(i*1.0/Fs);

    // === Print audio configuration ===
    QString tmp;
    tmp = "Device: ";
    tmp += m_deviceInfo.deviceName();
    tmp += " (";
    tmp += QString::number(prefferedFormat.sampleRate());
    tmp += "Hz, ";
    tmp += QString::number(prefferedFormat.sampleSize());
    tmp += "bits, ";
    tmp += "channelCount=";
    tmp += QString::number(prefferedFormat.channelCount());
    tmp += ", ";
    tmp += "sampleType=";
    switch (prefferedFormat.sampleType()) {
    case 0:
        tmp += "Unknown";
        break;
    case 1:
        tmp += "SignedInt";
        break;
    case 2:
        tmp += "UnSignedInt";
        break;
    case 3:
        tmp += "Float";
        break;
    }
    tmp += ", ";
    tmp += "byteOrder=";
    switch (prefferedFormat.byteOrder()) {
    case 0:
        tmp += "BigEndian";
        break;
    case 1:
        tmp += "LittleEndian";
        break;
    }
    tmp += ", ";
    tmp += "codec=";
    tmp += prefferedFormat.codec();
    tmp += ")";

    audioConfig->setText(tmp);

    qDebug() << prefferedFormat;
}

/**
 * @brief MainWindow::closeEvent, ask before quit and do what's necessary before quit application
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "",
                                                                tr("Quit? Are you sure?\n"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        mFileData->close();
        mFileData->remove();
        event->accept();
    } else
        event->ignore();
}

void MainWindow::about()
{
    AboutDialog().exec();
}

void MainWindow::onAudioConfigActionTriggered(bool)
{
    ;
}


void MainWindow::onLiveViewActionToggled(bool checked)
{
    QAction *tmp = dynamic_cast<QAction*>(sender());
    if (checked)
    {
        oscillogramWidget->setAllowRectangleAutoMove(false); // Disable then the rectangle can be move even if the signal have'nt reach the end of axis.
        tmp->setIcon(QIcon(":/icons/liveViewOnColoredOff.png"));
        m_AudioInput->suspend();
    } else
    {
        oscillogramWidget->setAllowRectangleAutoMove(true);
        tmp->setIcon(QIcon(":/icons/liveViewOnColoredOn.png"));
        m_AudioInput->resume();

        // Uncheck ROIAction : i.e. not allowed to move rectangle in live mode
        tmp->parent()->findChild<QAction*>("ROIAction")->setChecked(false);
    }
}

void MainWindow::onAppToolBarOrienationChanged(Qt::Orientation orientation)
{
    QToolBar *tmp = dynamic_cast<QToolBar*>(sender());
    if (orientation == Qt::Vertical)
        tmp->parent()->findChild<QBoxLayout*>("HPFilterActionLayout")->setDirection(QBoxLayout::TopToBottom);
    else
        tmp->parent()->findChild<QBoxLayout*>("HPFilterActionLayout")->setDirection(QBoxLayout::LeftToRight);
}

void MainWindow::onTriggerActionToggled(bool checked)
{
    oscillogramWidget->setTrigger(checked);
}

void MainWindow::onROIActionToggled(bool value)
{
    oscillogramWidget->setSelectRectangleMovable(value);
}

void MainWindow::onReadyRead()
{
    // Get data from audiobuffer
    qint64 count = m_InputDevice->read(dataSound, soundBufferSize);
    qint16 *sample = reinterpret_cast<qint16*>(reinterpret_cast<unsigned char*>(dataSound));

    //Convet to double
    QVector<double> myArray(qRound(count/2.0));
    std::copy(sample,sample + count/2,myArray.begin());
    std::transform (myArray.begin(), myArray.end(), myArray.begin(),
                    bind(std::divides<double>(), std::placeholders::_1, 32768.0 / oscillogramWidget->gain())); // FIXME Not really correct

    //Construct time vector
    QVector<double> x(xVecBase.mid(0,qRound(count/2.0)));
    std::transform (x.begin(), x.end(), x.begin(),
                    bind(std::plus<double>(), std::placeholders::_1, oscillogramWidget->upperTime()));

    // update last time
    oscillogramWidget->setUpperTime(oscillogramWidget->upperTime() + count/2.0 * 1.0/ Fs );

    // Add data to the graph
    oscillogramWidget->mPlot->graph(0)->addData(x, myArray);

    // Remove invisible data
    oscillogramWidget->mPlot->graph(0)->data().data()->removeBefore(oscillogramWidget->mPlot->xAxis->range().lower);

    // Move oscillogram axis
    if(oscillogramWidget->upperTime()>oscillogramWidget->mPlot->xAxis->range().upper)
    {
        double xmin = oscillogramWidget->mPlot->xAxis->range().lower +  count/2.0 * 1.0/ Fs;
        double xmax = oscillogramWidget->upperTime();
        oscillogramWidget->mPlot->xAxis->setRange(xmin,xmax);
    }


    // === Write data ===
    mFileData->write(dataSound,count);

}

void MainWindow::updateLevelMeter()
{
    // Compute the signal level in dBFS
    auto it = std::max_element(oscillogramWidget->selectRectangleData().constBegin(), oscillogramWidget->selectRectangleData().constEnd());
    double tmp = 20*log10(*it); // c++11;

    // Set the widget level
    if(!isnanf(float(tmp)))
        levelMeterWidget->setLevel(tmp);
}
/**
 * @brief MainWindow::onLevelMeterEnableChanged
 * @param checked
 */
void MainWindow::onLevelMeterEnableChanged(bool checked)
{
    if (checked)
        connect(oscillogramWidget,SIGNAL(availableSelectRectangleData()),this,SLOT(updateLevelMeter()));
    else
        disconnect(oscillogramWidget,SIGNAL(availableSelectRectangleData()),this,SLOT(updateLevelMeter()));
}

void MainWindow::onRefreshGraphs()
{
    oscillogramWidget->mPlot->replot(QCustomPlot::rpQueuedReplot);
}



#include "qfoscillogram.h"



QFOscillogram::QFOscillogram(QWidget *parent) : QWidget(parent),
    mPlot(new QCustomPlot()), selectRectangleWidth(0.2),
    triggerLevel(0.5),isTriggered(false),
    // isLiveView(true),
    triggerStillMove(false),
    selectRectangleStillMove(false), _upperTime(0),_allowRectangleAutoMove(false)
{


    // === Create Plot ===
    mPlot->addGraph();
    // mPlot->graph(0)->setData(QVector<double>{0},QVector<double>{0}); // Put one point at (0,0)
    mPlot->graph(0)->setPen(QPen(Qt::cyan));
    /// Add toolsisTriggertemplate<typename T>
    mGraph = new QFGraph(mPlot,mPlot->graph(0));

    // === Configure For real Time
    mPlot->graph(0)->setAdaptiveSampling(true);
    mPlot->graph(0)->setAntialiased(false);
    mPlot->graph(0)->setAntialiasedFill(false);
    mPlot->graph(0)->setAntialiasedScatters(false);
    mPlot->graph(0)->layer()->setMode(QCPLayer::lmBuffered);
    // See : https://www.qcustomplot.com/documentation/performanceimprovement.html
    //TODO : If you often need to call a full QCustomPlot::replot only because a non-complex object (e.g. an item)
    //       has changed while having relatively static but complex graphs in the plot, consider placing the
    //       regularly changing objects onto an own layer and setting its mode (QCPLayer::setMode) to QCPLayer::lmBuffered.
    //       This makes QCustomPlot allocate a dedicated paint buffer for this layer, and allows it to be replotted
    //       individually with QCPLayer::replot, independent of the other layers containing the potentially complex and
    //       slow graphs. See the documentation of the respective methods for details.
    //TODO 2
    //========
    //QSharedPointer< QCPGraphDataContainer > QCPGraph::data 	( 		) 	const
    //	inline
    //
    //Returns a shared pointer to the internal data storage of type QCPGraphDataContainer. You may use it to directly manipulate the data, which may be more convenient and faster than using the regular setData or addData methods.


    // === Cutomize Plot ===
    mPlot->xAxis->setLabel("Time (s)");
    mPlot->yAxis->setLabel("Amplitude (V)");
    mPlot->yAxis->setRange(-1,1);
    mPlot->xAxis->setRange(0,5);
    /*QFont mFont = mPlot->xAxis->labelFont();upper
    mFont.setBold(true);
    mPlot->xAxis->setLabelFont(mFont);
    mPlot->yAxis->setLabelFont(mFont);*/

    // ==== Customize X-Axis ===
    QColor baseColor(QColor(255,68,0,140));
    //QColor textColor(QColor(180,180,180));
    QColor textColor(QColor(255,68,0));
    QColor labelColor(QColor(150,150,150));
    mPlot->xAxis->setLabelColor(labelColor);
    mPlot->xAxis->setTickLabelColor(textColor);
    mPlot->xAxis->setBasePen(QPen(baseColor));
    mPlot->xAxis->setTickPen(QPen(baseColor));
    mPlot->xAxis->setSubTickPen(QPen(baseColor));
    QPen mPenGrid;
    mPenGrid = mPlot->xAxis->grid()->pen();
    mPenGrid.setColor(baseColor);
    mPlot->xAxis->grid()->setPen(mPenGrid);
    QPen mPen;
    mPen = mPlot->xAxis->grid()->zeroLinePen();
    mPen.setColor(baseColor);
    mPen.setWidth(2);
    mPlot->xAxis->grid()->setZeroLinePen(mPen);


    // === Customize Y-Axis ===
    mPlot->yAxis->setLabelColor(labelColor);
    mPlot->yAxis->setTickLabelColor(textColor);
    mPlot->yAxis->setBasePen(QPen(baseColor));
    mPlot->yAxis->setTickPen(QPen(baseColor));
    mPlot->yAxis->setSubTickPen(QPen(baseColor));
    mPenGrid = mPlot->yAxis->grid()->pen();
    mPenGrid.setColor(baseColor);
    mPlot->yAxis->grid()->setPen(mPenGrid);
    mPen = mPlot->yAxis->grid()->zeroLinePen();
    mPen.setColor(baseColor);
    mPen.setWidth(1);
    mPlot->yAxis->grid()->setZeroLinePen(mPen);



    // === Customize Background image ===
    mPlot->setBackground(QBrush(QColor("#323232")));
    background_Pixmap= new QCPItemPixmap(mPlot);
    background_Pixmap->setScaled(true,Qt::IgnoreAspectRatio);
    background_Pixmap->topLeft->setType(QCPItemPosition::ptPlotCoords);
    background_Pixmap->bottomRight->setType(QCPItemPosition::ptPlotCoords);
    background_Pixmap->topLeft->setCoords(mPlot->xAxis->range().lower, mPlot->yAxis->range().upper);
    background_Pixmap->bottomRight->setCoords(mPlot->xAxis->range().upper, mPlot->yAxis->range().lower);
    background_Pixmap->setPixmap(QPixmap(":icons/graphBackGround.png"));
    mPlot->addLayer("image", mPlot->layer(0), QCustomPlot::LayerInsertMode::limAbove);
    background_Pixmap->setLayer("image");
    connect(mPlot->xAxis,SIGNAL(rangeChanged(const QCPRange&)),this,SLOT(onAxisRangeChanged(const QCPRange&)));
    connect(mPlot->yAxis,SIGNAL(rangeChanged(const QCPRange&)),this,SLOT(onAxisRangeChanged(const QCPRange&)));

    // === Trigger Line ===
    triggerLine = new QCPItemLine(mPlot);
    triggerLine->start->setCoords(mPlot->xAxis->range().lower,triggerLevel);
    triggerLine->end->setCoords(mPlot->xAxis->range().upper,triggerLevel);
    triggerLine->setPen(QPen(Qt::gray, 2.0, Qt::DashDotLine));
    triggerLine->setSelectable(true);
    triggerLine->setVisible(false);


    // === Selection Rectangle ===
    selectRectangle = new QCPItemRect(mPlot);
    selectRectangle->setBrush(QBrush(QColor(255,68,0,70)));
    selectRectangle->setPen(QPen(QColor(255,68,0)));
    selectRectangle->topLeft->setCoords(mPlot->xAxis->range().upper - selectRectangleWidth ,mPlot->yAxis->range().upper);
    selectRectangle->bottomRight->setCoords(mPlot->xAxis->range().upper, mPlot->yAxis->range().lower);
    selectRectangle->setSelectable(false);
    connect(mPlot,SIGNAL(afterReplot()),this,SLOT(updateSelectRectangleData()));
    connect(mPlot,SIGNAL(beforeReplot()),this,SLOT(autoMoveRectangle()));

    //Update tool
    connect(this,SIGNAL(availableSelectRectangleData()),this,SLOT(updateMultiMeter()));

    // === Main view ===
    QHBoxLayout *mLayout = new QHBoxLayout(this);
    mLayout->addWidget(mGraph);
    this->setLayout(mLayout);
}

/**
 * @brief QFOscillogram::setYLabel, set Y-axis label
 * @param yLabelText: Text Label
 */
void QFOscillogram::setYLabel(const QString yLabelText)
{
    mPlot->yAxis->setLabel(yLabelText);
}

/**
 * @brief QFOscillogram::setTrigger, active/desactive trigger mode
 * @param value: set to True to active the trigger
 */
void QFOscillogram::setTrigger(bool value)
{
    isTriggered = value;
    // Set visible
    triggerLine->setVisible(value);
    // Connect signal to move Trigger line
    if(value)
        connect(mPlot,SIGNAL(mouseMove(QMouseEvent*)),this, SLOT(onTriggerMove(QMouseEvent*)));
    else
        disconnect(mPlot,SIGNAL(mouseMove(QMouseEvent*)),this, SLOT(onTriggerMove(QMouseEvent*)));

    // Change y-axis if trigger line is not visible
    if(triggerLevel>=0) {
        if(triggerLevel+0.1 > mPlot->yAxis->range().upper)
            mPlot->yAxis->setRangeUpper(((triggerLevel+0.1)>1)?1:triggerLevel+0.1);}
    else {
        if(triggerLevel-0.1 < mPlot->yAxis->range().lower)
            mPlot->yAxis->setRangeLower(((triggerLevel-0.1)<-1)?1:triggerLevel-0.1);}

    // Update plot if not in live view
    // if(!isLiveView)
    mPlot->replot(QCustomPlot::rpQueuedReplot);
}

/**
 * @brief QFOscillogram::setSelectRectangleMovable, active the mode to move the selection rectangle
 * @param value: set to True to move active the move mode
 */
void QFOscillogram::setSelectRectangleMovable(bool value)
{
    if(value)
        connect(mPlot,SIGNAL(mouseMove(QMouseEvent*)),this, SLOT(onSelectRectangleMove(QMouseEvent*)));
    else
        disconnect(mPlot,SIGNAL(mouseMove(QMouseEvent*)),this, SLOT(onSelectRectangleMove(QMouseEvent*)));
}

/**
 * @brief QFOscillogram::setTriggerValue, set the level of the trigger. If data is higher than the "value" the recording stopped
 * @param value: value of trigger level
 */
void QFOscillogram::setTriggerValue(double value)
{
    triggerLevel = value;

    // Update line coords
    triggerLine->start->setCoords(mPlot->xAxis->range().lower,triggerLevel);
    triggerLine->end->setCoords(mPlot->xAxis->range().upper,triggerLevel);

}

double QFOscillogram::upperTime() const
{
    return _upperTime;
}

void QFOscillogram::setUpperTime(const double upperTime)
{
    _upperTime = upperTime;
}

void QFOscillogram::setAllowRectangleAutoMove(bool allowRectangleAutoMove)
{
    _allowRectangleAutoMove = allowRectangleAutoMove;

    if(_allowRectangleAutoMove)
        connect(mPlot,SIGNAL(beforeReplot()),this,SLOT(autoMoveRectangle()));
    else
        disconnect(mPlot,SIGNAL(beforeReplot()),this,SLOT(autoMoveRectangle()));


}

void QFOscillogram::autoMoveRectangle()
{
    auto maxIdx = mPlot->graph(0)->data().data()->end()-1;


    auto timeResolution = mPlot->graph(0)->data().data()->begin();
    timeResolution++;
    auto endOfAxis = mPlot->graph(0)->keyAxis()->range().upper - (timeResolution->key); // FIXME : Need  to remove 1/Fs to work!

    // Test if signal reaches the end of the x-axis
    if( (float)maxIdx->key < (float)endOfAxis)
    {
        double newUpper, newLower;
        newUpper=maxIdx->key;
        newLower=maxIdx->key - selectRectangleWidth;

        // Update rectangle coords
        selectRectangle->topLeft->setCoords(newLower, mPlot->yAxis->range().upper);
        selectRectangle->bottomRight->setCoords(newUpper, mPlot->yAxis->range().lower);
        selectRectangle->topLeft->setCoords(newLower, mPlot->yAxis->range().upper);
        selectRectangle->bottomRight->setCoords(newUpper, mPlot->yAxis->range().lower);
    }
}

QVector<double> QFOscillogram::selectRectangleData() const
{
    return _selectRectangleData;
}



void QFOscillogram::onAxisRangeChanged(const QCPRange&)
{
    // Update background coords
    background_Pixmap->topLeft->setCoords(mPlot->xAxis->range().lower, mPlot->yAxis->range().upper);
    background_Pixmap->bottomRight->setCoords(mPlot->xAxis->range().upper, mPlot->yAxis->range().lower);

    // Update rectangle coords
    selectRectangle->topLeft->setCoords(mPlot->xAxis->range().upper-selectRectangleWidth, mPlot->yAxis->range().upper);
    selectRectangle->bottomRight->setCoords(mPlot->xAxis->range().upper, mPlot->yAxis->range().lower);


    // Update Trigger Line coords
    triggerLine->start->setCoords(mPlot->xAxis->range().lower,triggerLevel);
    triggerLine->end->setCoords(mPlot->xAxis->range().upper, triggerLevel);
}



void QFOscillogram::onTriggerMove(QMouseEvent* event)
{
    // == Change cursor ==
    double sel = triggerLine->selectTest(event->pos(), false, nullptr);
    if (sel <= mPlot->selectionTolerance())
        mPlot->setCursor(Qt::SizeVerCursor);
    else
        mPlot->setCursor(Qt::ArrowCursor);

    // === Move if mouse button pressed ===
    if( event->buttons() == Qt::MouseButton::LeftButton  && (sel <= mPlot->selectionTolerance()||triggerStillMove) ) // if left mouse button pressed
    {
        triggerStillMove= true; // allow to make quick displacement without staying mouse cursor on the line

        // Get mouse cursor position
        double x,y;
        mPlot->graph()->pixelsToCoords(event->pos().x(), event->pos().y(), x, y);

        // Update Trigger Line Coords
        triggerLine->start->setCoords( mPlot->xAxis->range().lower, y);
        triggerLine->end->setCoords( mPlot->xAxis->range().upper, y);
        triggerLevel = y;
    }
    else
    {
        triggerStillMove = false;
    }

    // Update plot if not in live view
    // if(!isLiveView)
    mPlot->replot(QCustomPlot::rpQueuedReplot);
}

void QFOscillogram::onSelectRectangleMove(QMouseEvent *event)
{
    double sel=selectRectangle->selectTest(event->pos(), false, nullptr);

    // === Change cursor ===
    if (sel <= mPlot->selectionTolerance())
        mPlot->setCursor(Qt::ClosedHandCursor); // Not Working
    //mPlot->setCursor(Qt::PointingHandCursor); // Alternative
    else
        mPlot->setCursor(Qt::ArrowCursor);

    // === Move if mouse button pressed ===
    if( event->buttons() == Qt::MouseButton::LeftButton  && (sel <= mPlot->selectionTolerance()||selectRectangleStillMove) )
    {
        mPlot->setCursor(Qt::ClosedHandCursor);
        selectRectangleStillMove = true;

        // Get mouse cursor position
        double x,y;
        mPlot->graph()->pixelsToCoords(event->pos().x(), event->pos().y(), x, y);

        // Move Rectangle
        double newUpper, newLower;
        newLower = x - selectRectangleWidth/2;
        newUpper = x + selectRectangleWidth/2;

        if(newLower<qMax(newLower, mPlot->xAxis->range().lower)) // Limit Left diplacement
        {
            newLower=mPlot->xAxis->range().lower;
            newUpper=mPlot->xAxis->range().lower + selectRectangleWidth;
        }

        if(newUpper>qMax(newLower, mPlot->xAxis->range().upper)) // Limit Right diplacemen
        {
            newUpper=mPlot->xAxis->range().upper;
            newLower=mPlot->xAxis->range().upper - selectRectangleWidth;
        }

        // Update rectangle coords
        selectRectangle->topLeft->setCoords(newLower, mPlot->yAxis->range().upper);
        selectRectangle->bottomRight->setCoords(newUpper, mPlot->yAxis->range().lower);
        selectRectangle->topLeft->setCoords(newLower, mPlot->yAxis->range().upper);
        selectRectangle->bottomRight->setCoords(newUpper, mPlot->yAxis->range().lower);

    }
    else
    {
        selectRectangleStillMove = false;
    }

    // Update plot if not in live view
    // if(!isLiveView)
    mPlot->replot(QCustomPlot::rpQueuedReplot);


}

////////////////////////////////////////////////////////:
///  WARNING !!!!!!!!!!!!!!
/// /////////////////////////////////////////////////////////::


//  TODO: try to use selection class of QCustomPlot!
void QFOscillogram::updateSelectRectangleData()
{
    //QCPDataSelection 	selectTestRect ( bool onlySelectable) const
    // === Get begin and end index of data for selectionr rectangle ===
    double xBegin,y;
    mPlot->graph(0)->pixelsToCoords(selectRectangle->left->pixelPosition().x(),selectRectangle->left->pixelPosition().y(),xBegin,y);
    double xEnd;
    mPlot->graph(0)->pixelsToCoords(selectRectangle->right->pixelPosition().x(),selectRectangle->right->pixelPosition().y(),xEnd,y);
    int idxBegin = mPlot->graph(0)->findBegin(xBegin);
    int idxEnd = mPlot->graph(0)->findEnd(xEnd,true);

    // === If enough data is available fill the selection rectangle buffer ===
    auto maxIdx = mPlot->graph(0)->data().data()->begin() + idxEnd-1;
    if(maxIdx->key > selectRectangleWidth)
    {
        _selectRectangleData.clear();
        for (auto it = mPlot->graph(0)->data().data()->begin()+idxBegin; it < (mPlot->graph(0)->data().data()->begin() + idxEnd); it++)
        {
            _selectRectangleData.append(it->value);
        }
        emit availableSelectRectangleData(); // Emits buffer availablility
    }
}



/**
 * @brief QFOscillogram::updateMultiMeter, Computes statistics in selection rectangle zone, (i.e. Min, Max, Avg and Abs values) and update mutlimeter values to update the view
 */
void QFOscillogram::updateMultiMeter()
{

    // Max value
    auto result = std::max_element(selectRectangleData().constBegin(), selectRectangleData().constEnd());
    //this->setMultiMeterMaxValue(*result);
    mGraph->meter->setMaxValue(*result) ;

    // Min Value
    result = std::min_element(selectRectangleData().constBegin(), selectRectangleData().constEnd());
    //this->setMultiMeterMinValue(*result);
    mGraph->meter->setMinValue(*result) ;

    // Average Value
    auto result2 = std::accumulate(selectRectangleData().constBegin(), selectRectangleData().constEnd(),.0)/selectRectangleData().size();
    //this->setMultiMeterAvgValue(result2);
    mGraph->meter->setAvgValue(result2) ;

    // Absolute Value
    result2 =qSqrt(std::accumulate(selectRectangleData().constBegin(), selectRectangleData().constEnd(),.0,square<double>())/selectRectangleData().size());
    //this->setMultiMeterAbsValue(result2);
    mGraph->meter->setAbsValue(result2) ;
}



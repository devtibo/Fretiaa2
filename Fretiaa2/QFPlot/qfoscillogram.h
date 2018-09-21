#ifndef QFOSCILLOGRAM_H
#define QFOSCILLOGRAM_H

#include <QWidget>
#include "qcustomplot/qcustomplot.h"
#include "qfgraph.h"

template<typename T>
struct square
{
    T operator()(const T& Left, const T& Right) const
    {
        return (Left + Right*Right);
    }
};


/**
 * @class QFOscillogram
 * @brief The QFOscillogram class
 */
class QFOscillogram : public QWidget
{
    Q_OBJECT
public:
    explicit QFOscillogram(QWidget *parent = nullptr);

    // === MainGraph ===
    QCustomPlot *mPlot;
    void setYLabel(const QString);

    // === Trigger ===
    void setTrigger(bool);
    void setTriggerValue(double);

    // === Selection rectangle ===
    QVector<double> selectRectangleData() const;
    void setSelectRectangleMovable(bool);
    double upperTime() const;
    void setUpperTime(const double upperTime);

    void setAllowRectangleAutoMove(bool allowRectangleAutoMove);

private:
    // === Mainplot ===
    QFGraph *mGraph;
    QCPItemPixmap *background_Pixmap;

    // === Selection Rectangle ===
    QCPItemRect *selectRectangle;
    QVector<double> _selectRectangleData;
    double _upperTime;
    double selectRectangleWidth;
    bool selectRectangleStillMove;
    bool _allowRectangleAutoMove;

    // === Trigger ===
    QCPItemLine *triggerLine;
    double triggerLevel;
    bool isTriggered;
    bool triggerStillMove;

public slots:
    // === MainPlot ===
    void onAxisRangeChanged (const QCPRange&);
    // === Trigger===
    void onTriggerMove(QMouseEvent* event);
    // === Selection Rectangle ===
    void onSelectRectangleMove(QMouseEvent* event);
    void updateSelectRectangleData();
    void autoMoveRectangle();

    // === Multimeter ===
    void updateMultiMeter();

signals:
    /**
     * @brief availableSelectRectangleData: Emits when buffer containing data in the selection rectangle zone is readable
     */
    void availableSelectRectangleData();
};

#endif // QFOSCILLOGRAM_H

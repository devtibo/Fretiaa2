#ifndef QFLEVELMETER_H
#define QFLEVELMETER_H

#include <QWidget>
#include "qcustomplot/qcustomplot.h"

class QFLevelMeter : public QWidget
{
    Q_OBJECT
public:
    explicit QFLevelMeter(const Qt::Orientation, QWidget *parent = nullptr);
        void setLevel(const double);

private:
    void setVerticalOrientation(void);
    void setHorizontalOrientation(void);
    QLayout *getOrientedLayout(const Qt::Orientation);
    QCPBars *newbars;
    double currenLevel;
    QCustomPlot *mPlot;
public slots:
    void changeOrientation(const Qt::DockWidgetArea);
};

#endif // QFLEVELMETER_H


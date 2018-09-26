#ifndef QFOSCILLOGRAMROIRECTANGLE_H
#define QFOSCILLOGRAMROIRECTANGLE_H

#include "qcustomplot/qcustomplot.h"

class QFOscillogramROIRectangle : public QCPItemRect
{
    Q_OBJECT
public:
    explicit QFOscillogramROIRectangle(QCustomPlot *parentPlot);

    double width() const;
    void setWidth(double width);

    QVector<double> data() const;

    void setPosition(double middle);
    void setPosition(double lower, double upper);

    void setUpper(double upper);
    void setLower(double upper);

private:
    double _width;
    QVector<double> _data;

public slots:
    void updateData();

signals:
    void dataAvailable();

public slots:

};

#endif // QFOSCILLOGRAMROIRECTANGLE_H

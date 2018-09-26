#include "qfoscillogramroirectangle.h"

QFOscillogramROIRectangle::QFOscillogramROIRectangle(QCustomPlot *parentPlot) : QCPItemRect(parentPlot)
{
    // Aspect
    this->setBrush(QBrush(QColor(255,68,0,70)));
    this->setPen(QPen(QColor(255,68,0)));

    //
    this->setSelectable(false);
}

double QFOscillogramROIRectangle::width() const
{
    return _width;
}

void QFOscillogramROIRectangle::setWidth(double width)
{
    _width = width;
}

QVector<double> QFOscillogramROIRectangle::data() const
{
    return _data;
}

void QFOscillogramROIRectangle::setPosition(double middle)
{
    this->topLeft->setCoords(middle - this->width()/2.0, parentPlot()->yAxis->range().upper);
    this->bottomRight->setCoords(middle + this->width()/2.0, parentPlot()->yAxis->range().lower);
    this->topLeft->setCoords(middle - this->width()/2.0, parentPlot()->yAxis->range().upper);
    this->bottomRight->setCoords(middle + this->width()/2.0, parentPlot()->yAxis->range().lower);
}

void QFOscillogramROIRectangle::setPosition(double lower, double upper)
{
    this->topLeft->setCoords(lower, parentPlot()->yAxis->range().upper);
    this->bottomRight->setCoords(upper, parentPlot()->yAxis->range().lower);
    this->topLeft->setCoords(lower, parentPlot()->yAxis->range().upper);
    this->bottomRight->setCoords(upper, parentPlot()->yAxis->range().lower);

}

void QFOscillogramROIRectangle::setUpper(double upper)
{
    this->topLeft->setCoords(upper-this->width(), parentPlot()->yAxis->range().upper);
    this->bottomRight->setCoords(upper, parentPlot()->yAxis->range().lower);
    this->topLeft->setCoords(upper-this->width(), parentPlot()->yAxis->range().upper);
    this->bottomRight->setCoords(upper, parentPlot()->yAxis->range().lower);
}

void QFOscillogramROIRectangle::setLower(double lower)
{
    this->topLeft->setCoords(lower, parentPlot()->yAxis->range().upper);
    this->bottomRight->setCoords(lower+this->width(), parentPlot()->yAxis->range().lower);
    this->topLeft->setCoords(lower, parentPlot()->yAxis->range().upper);
    this->bottomRight->setCoords(lower+this->width(), parentPlot()->yAxis->range().lower);
}

void QFOscillogramROIRectangle::updateData()
{
    // === Get begin and end index of data for selectionr rectangle ===
    double xBegin,y;
    parentPlot()->graph(0)->pixelsToCoords(this->left->pixelPosition().x(),this->left->pixelPosition().y(),xBegin,y);
    double xEnd;
    parentPlot()->graph(0)->pixelsToCoords(this->right->pixelPosition().x(),this->right->pixelPosition().y(),xEnd,y);
    int idxBegin = parentPlot()->graph(0)->findBegin(xBegin);
    int idxEnd = parentPlot()->graph(0)->findEnd(xEnd,true);

    // === If enough data is available fill the selection rectangle buffer ===
    auto maxIdx = parentPlot()->graph(0)->data().data()->begin() + idxEnd-1;
    if(maxIdx->key > this->width())
    {
        _data.clear();
        for (auto it = parentPlot()->graph(0)->data().data()->begin()+idxBegin; it < (parentPlot()->graph(0)->data().data()->begin() + idxEnd); ++it)
        {
            _data.append(it->value);
        }

        emit dataAvailable(); // Emits buffer availablility
    }
}


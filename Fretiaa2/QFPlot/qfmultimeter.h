#ifndef QFMultiMeter_H
#define QFMultiMeter_H

#include <QLabel>
#include <QBoxLayout>
#include <QDebug>

class QFMultiMeter : public QWidget
{
        Q_OBJECT

public:
    explicit QFMultiMeter( QWidget *parent=nullptr);

    double maxValue() const;
    void setMaxValue(double maxValue);

    double minValue() const;
    void setMinValue(double minValue);

    double avgValue() const;
    void setAvgValue(double avgValue);

    double absValue() const;
    void setAbsValue(double absValue);



    QString unit() const;
    void setUnit(const QString &unit);

public slots:
    void meterChangeOrientation(Qt::Orientation);
    void onEnableToggle(bool);

private :
    QLayout *createLayout(Qt::Orientation);
    void updateLabels();


    QLabel *lMaxValue;
    QLabel *lMinValue;
    QLabel *lAvgValue;
    QLabel *lAbsValue;

    QLabel *lMaxUnit;
    QLabel *lMinUnit;
    QLabel *lAvgUnit;
    QLabel *lAbsUnit;

    QLabel *lMaxText;
    QLabel *lMinText;
    QLabel *lAvgText;
    QLabel *lAbsText;

    QString _unit;


    double _maxValue, _minValue, _avgValue, _absValue ;

signals:
    void enableChanged(bool);
};

#endif // QFMultiMeter_H

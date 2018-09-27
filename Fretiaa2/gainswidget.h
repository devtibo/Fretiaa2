#ifndef GAINSWIDGET_H
#define GAINSWIDGET_H

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDebug>
#include <qmath.h>

class GainsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GainsWidget(QWidget *parent = nullptr);

    double gain() const;

private:
    QLayout* createLayout(Qt::Orientation orientation);
    QLineEdit *micSensitivity_edit;
    QComboBox *micSensitivity_combo;

    QLineEdit *preAmpGain_edit ;
    QComboBox *preAmpGain_combo;
    QLineEdit *adcInGain_edit ;
    QComboBox *adcInGain_combo ;
    double _gain;

signals:
    void gainChanged(double);

public slots:

    void onUpdate();
        void changeOrientation(const Qt::DockWidgetArea area);
};

#endif // GAINSWIDGET_H

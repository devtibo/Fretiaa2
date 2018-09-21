#ifndef XYRANGEDIALOG_H
#define XYRANGEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QGridLayout>
#include <QRegExpValidator>
#include <QDialogButtonBox>
#include <QLabel>

class XYRangeDialog  : public QDialog
{
    Q_OBJECT

public:
    XYRangeDialog(QWidget *parent=nullptr, double x1=0, double x2=0);

    double getLowerLimit() const;
    void setLowerLimit(const double);
    double getHigerLimit() const;
    void getHigerLimit(const double);
private :
    QLineEdit *editLine1;
    QLineEdit *editLine2;
    double val1, val2;

public slots:
    void accept();
};

#endif // XYRANGEDIALOG_H

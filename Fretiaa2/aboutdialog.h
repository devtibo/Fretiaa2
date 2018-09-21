#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

//!  An about window class
/*!
  Class to generate a about window
*/
class AboutDialog: public QDialog
{
    Q_OBJECT

public:
     //! Constructor
    AboutDialog(QWidget *parent = 0);

   // QDialog *dial = new QDialog;

public slots:

};

#endif // ABOUTDIALOG_H

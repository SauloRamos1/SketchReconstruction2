#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "mytask.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_modalButton_clicked();

    void on_modelessButton_clicked();

private:
    Ui::Dialog *ui;
    MyTask *myTask;
};

#endif // DIALOG_H

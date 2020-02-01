#ifndef SOFTINFODIALOG_H
#define SOFTINFODIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class SoftInfoDialog;
}

class SoftInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SoftInfoDialog(QWidget *parent = nullptr);
    ~SoftInfoDialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::SoftInfoDialog *ui;
};

#endif // SOFTINFODIALOG_H

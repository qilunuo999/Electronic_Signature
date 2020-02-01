#ifndef UNSEALPASSWD_H
#define UNSEALPASSWD_H

#include <QDialog>

namespace Ui {
class UnsealPasswd;
}

class UnsealPasswd : public QDialog
{
    Q_OBJECT

public:
    explicit UnsealPasswd(QWidget *parent = nullptr);
    ~UnsealPasswd();

private:
    Ui::UnsealPasswd *ui;
};

#endif // UNSEALPASSWD_H

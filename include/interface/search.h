#ifndef SEARCH_H
#define SEARCH_H

#include<QDialog>
//查找窗口
namespace Ui
{
class Search;
}

class Search : public QDialog
{
    Q_OBJECT
public:
    explicit Search(QWidget *parent = nullptr);
    ~Search();

private slots:

    void on_findButton_clicked();

    void on_cancelButton_2_clicked();

private:
    Ui::Search* ui;
};

#endif // SEARCH_H

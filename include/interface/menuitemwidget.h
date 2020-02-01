#ifndef MENUITEMWIDGET_H
#define MENUITEMWIDGET_H

#include "QWidget"
#include "QLabel"
#include "QHBoxLayout"
#include "QStyleOption"
#include "QPainter"


class MenuItemWidget : public QWidget
{
    Q_OBJECT
public:
    MenuItemWidget(const QPixmap& icon, const QString& text, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;

};

#endif // MENUITEMWIDGET_H

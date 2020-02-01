#include "include/interface/menuitemwidget.h"

MenuItemWidget::MenuItemWidget(const QPixmap& icon, const QString& text, QWidget *parent)
{
    QLabel* label_Icon = new QLabel(this);
    label_Icon->setFixedSize(32, 32);
    label_Icon->setScaledContents(true);
    label_Icon->setPixmap(icon);

    QLabel* label_Text = new QLabel(text,this);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(20, 6, 50, 6);
    layout->setSpacing(10);
    layout->addWidget(label_Icon);
    layout->addWidget(label_Text);
    setLayout(layout);

    setFixedWidth(240);
}
void MenuItemWidget::paintEvent(QPaintEvent* e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter *p = new QPainter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, p, this);
}

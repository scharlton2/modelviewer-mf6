#include "qabstractview.h"

QAbstractView::QAbstractView(QObject *parent)
    : QObject{parent}
{

}

void QAbstractView::onUpdate(QAbstractView* sender, QObject* /*hint*/)
{
    assert(sender != this);
}

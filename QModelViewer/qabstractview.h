#ifndef QABSTRACTVIEW_H
#define QABSTRACTVIEW_H

#include <QObject>

class QAbstractView : public QObject
{
    Q_OBJECT
public:
    explicit QAbstractView(QObject *parent = nullptr);

    virtual void onUpdate(QAbstractView* pSender, QObject* pHint);

signals:

};

#endif // QABSTRACTVIEW_H

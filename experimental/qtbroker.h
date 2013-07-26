#ifndef QTBROKER_H
#define QTBROKER_H

#include "Proxy/proxy.h"

class QtQuickBroker : public QObject
{
    Q_OBJECT


    Proxy   *   m_proxy;

public:
    QtQuickBroker(Proxy *proxy, QObject* parent = 0):
         QObject(parent),
         m_proxy( proxy )
        {

        }

    Q_INVOKABLE int connect()
    {
        m_proxy->start();
        m_proxy->connectAsClient();
        return 1;
    }

};

#endif

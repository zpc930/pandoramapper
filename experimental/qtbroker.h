#ifndef QTBROKER_H
#define QTBROKER_H

#include <QQuickView>
#include <QQmlContext>

#include <iostream>

#include "Proxy/proxy.h"

#include "mudoutput.h"

class QtQuickBroker : public QObject
{
    Q_OBJECT


    Proxy           *   m_proxy;
    QQuickView      *   m_quickView;

public:
    QtQuickBroker(QQuickView *view, QObject* parent = 0):
         QObject(parent),
         m_quickView( view )
        {
        }

    ~QtQuickBroker() {}

    Q_INVOKABLE int connect()
    {
        m_proxy = new Proxy(this);
        m_proxy->start();
        m_proxy->connectAsClient();
        return 1;
    }

    Q_INVOKABLE int appendText()
    {
        std::cout << "Sending signal!" << std::endl;
        emit mudTextChanged("My Text \r\n testing \r\n again \r\n");

//        QQuickItem *context = m_quickView->rootObject();
//        MudOutput *output = context->findChild<MudOutput *>("mud_output");

//        if (output == NULL) {
//            std::cout << "Mud output child not found." << std::endl;
//            return 0;
//        }

        //        output->appendText("My Text \r\n testing \r\n again \r\n");

        return 1;
    }

    void newText(const QString &str)
    {
        std::cout << "Adding new text: " << str.toLocal8Bit().constData() << std::endl;
        emit mudTextChanged(str);
    }

Q_SIGNALS:
    void mudTextChanged(const QString &text);


};

#endif

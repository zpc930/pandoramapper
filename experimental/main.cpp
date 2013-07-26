/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>
#include <QtCore/QThread>

#include <QtQuick/QQuickView>

#include <QtQml>

#include <QQmlApplicationEngine>

#include "squircle.h"
#include "fboinsgrenderer.h"
#include "qtbroker.h"
#include "mudoutput.h"

#include "Proxy/proxy.h"

QQuickView *mainView;

static QObject *qtbroker_qobject_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)


    QtQuickBroker *example = new QtQuickBroker(mainView);
    return example;
}

//! [1]
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

//    if (!QGuiApplicationPrivate::platform_integration->hasCapability(QPlatformIntegration::ThreadedOpenGL)) {
//        QQuickView view;
//        view.setSource(QUrl("qrc:///scenegraph/textureinthread/error.qml"));
//        view.show();
//        return app.exec();
//    }

    qmlRegisterType<FboInSGRenderer>("SceneGraphRendering", 1, 0, "Renderer");
    qmlRegisterType<Squircle>("OpenGLUnderQML", 1, 0, "Squircle");
    qmlRegisterType<MudOutput>("MudOutput", 1, 0, "MudOutput");

    qmlRegisterSingletonType<QtQuickBroker>("Qt.QtBrokerSingleton", 1, 0, "QtBroker", qtbroker_qobject_singletontype_provider);


    // Rendering in a thread introduces a slightly more complicated cleanup
    // so we ensure that no cleanup of graphics resources happen until the
    // application is shutting down.
    //view.setPersistentOpenGLContext(true);
    //view.setPersistentSceneGraph(true);

    mainView = new QQuickView();

    mainView->setResizeMode(QQuickView::SizeRootObjectToView);
    mainView->setSource(QUrl("qrc:///scenegraph/openglunderqml/main.qml"));



    mainView->show();


    return app.exec();
}
//! [1]

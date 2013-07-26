#ifndef MUDOUTPUT_H
#define MUDOUTPUT_H

#include <QQuickItem>
#include <QtQuick/private/qquicktext_p.h>

class MudOutput : public QQuickText
{
    Q_OBJECT

public:
    MudOutput(QQuickItem *parent=0);
    ~MudOutput();

protected:

public slots:
    void appendText(const QString &text);

};

#endif

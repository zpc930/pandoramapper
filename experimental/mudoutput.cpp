#include "mudoutput.h"

#include <iostream>

MudOutput::MudOutput(QQuickItem *parent):
    QQuickText(parent)
{

}

MudOutput::~MudOutput()
{

}


void MudOutput::appendText(const QString &t)
{
    std::cout << "Signal appendText received \r\n";
    QString newText = text().append(t);
    setText(newText);
}

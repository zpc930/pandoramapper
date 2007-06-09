#ifndef FINDDIALOG_H
#define FINDDIALOG_H
#include <QDialog>
#include "ui_finddialog.h"

class FindDialog : public QDialog, public Ui::FindDialog
{
    Q_OBJECT

public:
    FindDialog(QWidget *parent = 0);

private:
    void adjustResultTable();

private slots:
    void on_lineEdit_textChanged();
    void findClicked();
    void enableFindButton(const QString &text);
    void itemDoubleClicked(QTreeWidgetItem *item);
};
#endif

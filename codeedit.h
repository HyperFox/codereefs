#ifndef CODEEDIT_H
#define CODEEDIT_H

#include <QFile>
#include <QtWidgets>

class codeEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit codeEdit(QWidget *parent = 0);

signals:

public slots:
    void closeAll();
    void openFile(QFile*,QIODevice::OpenModeFlag);

};

#endif // CODEEDIT_H

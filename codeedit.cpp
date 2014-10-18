#include "codeedit.h"

codeEdit::codeEdit(QWidget*parent) :
    QTextEdit(parent) {
    ;
}

void codeEdit::closeAll() {
    ;
}

void codeEdit::openFile(QFile *pfile, QIODevice::OpenModeFlag openMode) {
    QMessageBox::warning(this,tr("打开文件"),tr("arrived！"),QMessageBox::Ok,0);
}

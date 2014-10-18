#include "mainwindow.h"

MainWindow::MainWindow(qint32 _argc,char *_argv[],QWidget *parent) :
    QMainWindow(parent) {
    if (_argc > 1) {
        //用户通过双击代码文件或带参数的命令行打开程序
        for (argc = 1;argc < _argc;argc++) {
            argv.push_back(QString(_argv[argc]));
        }
    } else {
        //用户通过点击程序图标或不带参数的命令行打开程序
        argv.push_back(QString("/"));
    }
    setupUi(this);
    windowGeometry = saveGeometry();
    mainMenuBar->installEventFilter(this);
    programName = "code reefs";
    untitledNum = 0;
    connect(codeEditTab,SIGNAL(currentChanged(qint32)),this,SLOT(setTitle(qint32)));
}

/*MainWindow::~MainWindow()
{
    delete ui;
}*/

bool MainWindow::eventFilter(QObject *obj,QEvent *e) {
    //实现功能：按住menuBar拖动窗体
   if (obj == mainMenuBar) {
        switch (e->type()) {
        case QEvent::MouseButtonPress: {
            QMouseEvent *event = static_cast<QMouseEvent*> (e);
            if (event->button() == Qt::LeftButton) {
                pos = event->globalPos() - frameGeometry().topLeft();
            }
            return false;
        }
        case QEvent::MouseMove: {
            QMouseEvent *event = static_cast<QMouseEvent*> (e);
            if (event->buttons() == Qt::LeftButton) {
                move(event->globalPos() - this->pos);
            }
            return true;
        }
        case QEvent::MouseButtonDblClick: {
            if (windowState() != Qt::WindowMaximized) {
                windowGeometry = saveGeometry();
                setWindowState(Qt::WindowMaximized);
            } else {
                restoreGeometry(windowGeometry);
            }
            return true;
        }
        default:
            break;
        }
   }
   return false;
}

qint32 MainWindow::newTab() {
    qint32 ret;
    ptmpFiles.push_back(new QTemporaryFile());//TODO:添加断电恢复现场功能
    if ((ptmpFiles.back())->open()) {
        openedFileNames.push_back("/");
        openedTmpFileNames.push_back(QDir::tempPath() + "/" + (ptmpFiles.back())->fileName());
        ptextEdits.push_back(new QTextEdit());
        ret = codeEditTab->addTab(ptextEdits.back(),tr("未命名%1").arg(++untitledNum));
        switchToTab("/");
    } else {
        delete ptmpFiles.back();
        ptmpFiles.erase(ptmpFiles.end() - 1);
        ret = EAGAIN;
    }
    return ret;
}

QMenu* MainWindow::createPopupMenu() {
    return 0;
}

void MainWindow::closeEvent(QCloseEvent *e) {
    on_actionExit_triggered();
    e->accept();
}

void MainWindow::closeFile(qint32 index) {
    if (index >= 0) {
        (ptmpFiles.at(index))->close();//TODO:该函数会发射abouttoclose()，需要添加提示保存的处理函数
        codeEditTab->removeTab(index);//删除对应标签页
        delete ptmpFiles.at(index);//释放QtmpFile的内存,关闭文件
        ptmpFiles.erase(ptmpFiles.begin() + index);//删除对应指针
        delete ptextEdits.at(index);//释放textEdits的内存
        ptextEdits.erase(ptextEdits.begin() + index);//删除对应指针
        openedFileNames.erase(openedFileNames.begin() + index);//删除对应类
        openedTmpFileNames.erase(openedTmpFileNames.begin() + index);//删除对应类
    }
}

void MainWindow::on_actionAbout_triggered() {
    QMessageBox::about(this,programName,tr("this program is written by yuch.\nemail :yuchno1@163.com"));
}

void MainWindow::on_actionAbout_Qt_triggered() {
    QMessageBox::aboutQt(this,programName);
}

void MainWindow::on_actionClose_triggered() {
    on_codeEditTab_tabCloseRequested(codeEditTab->currentIndex());
}

void MainWindow::on_actionClose_all_triggered() {
    qint32 index;
    for (index = openedFileNames.size();index > 0;index--) {
        closeFile(index - 1);
    }
}

void MainWindow::on_actionCopy_triggered() {
    (ptextEdits.at(codeEditTab->currentIndex()))->copy();
}

void MainWindow::on_actionCut_triggered() {
    (ptextEdits.at(codeEditTab->currentIndex()))->cut();
}

void MainWindow::on_actionExit_triggered() {
    on_actionClose_all_triggered();
    close();
}

void MainWindow::on_actionNew_triggered() {
    newTab();
}

void MainWindow::on_actionOpen_triggered() {
    QString dir;
    //使用最后打开的文件所在目录作为默认目录，如果没有就使用根目录作为默认目录
    if (ptmpFiles.empty()) {
        dir = "/";
    } else {
        QFileInfo tmpFileInfo(openedFileNames.back());
        dir = tmpFileInfo.absolutePath();
    }
    openFiles(QFileDialog::getOpenFileNames(this,
                                                         tr("选择文件"),
                                                        dir,
                                                        "All file(*.*);;C file(*.c *.cpp);;header file(*.h)"));
}

void MainWindow::on_actionPaste_triggered() {
    (ptextEdits.at(codeEditTab->currentIndex()))->paste();
}

void MainWindow::on_actionRedo_triggered() {
    (ptextEdits.at(codeEditTab->currentIndex()))->redo();
}

void MainWindow::on_actionSave_triggered() {
    qint32 index = codeEditTab->currentIndex();
    QFileInfo tmpFileInfo(openedFileNames.at(index));
    if (tmpFileInfo.isFile()) {
        saveFile(index);
    } else {
        on_actionSave_as_triggered();
    }
}

void MainWindow::on_actionSave_all_triggered() {
    qint32 index,num = openedFileNames.size();
    for (index = 0;index < num;index++) {
        saveFile(index);
    }
}

void MainWindow::on_actionSave_as_triggered() {
    QString tmpS = QFileDialog::getSaveFileName(this,
                                                             tr("保存文件"),
                                                            openedFileNames.at(codeEditTab->currentIndex()),
                                                            "All file(*.*);;C file(*.c *.cpp);;header file(*.h)");
    if (!(tmpS.isEmpty())) {
        saveFile(codeEditTab->currentIndex(),tmpS);
    }
}

void MainWindow::on_actionUndo_triggered() {
    (ptextEdits.at(codeEditTab->currentIndex()))->undo();
}

void MainWindow::on_codeEditTab_tabCloseRequested(qint32 index) {
    closeFile(index);
}

//该函数用于打开文件并提交给控件进行显示，sin为文件绝对路径，flag为打开方式，
//如果路径可读写就把临时文件放在原文件同一目录下，否则就把临时文件放在系统临时目录下
void MainWindow::openFile(QString sin,QIODevice::OpenModeFlag flag) {
    QFile tmpFile(sin);
    QFileInfo tmpFileInfo(sin);
    if (flag == QIODevice::ReadOnly) {
        ptmpFiles.push_back(new QTemporaryFile(QDir::tempPath() + "/" + tmpFileInfo.fileName()));
    } else {
        ptmpFiles.push_back(new QTemporaryFile(sin));//TODO:临时文件的使用还需要考虑
    }
    if ((ptmpFiles.back())->open() && tmpFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ptextEdits.push_back(new QTextEdit());
        (ptextEdits.back())->setText(tmpFile.readAll());
        tmpFile.close();
        codeEditTab->addTab(ptextEdits.back(),tmpFileInfo.fileName());
        codeEditTab->setTabToolTip(codeEditTab->count() - 1,tmpFileInfo.absoluteFilePath());
        openedTmpFileNames.push_back(QDir::tempPath() + "/" + (ptmpFiles.back())->fileName());//TODO:以后要改成保存成配置文件的，用于断电恢复现场
        openedFileNames.push_back(sin);
        switchToTab(sin);
    } else {
        tmpFile.close();
        delete ptmpFiles.back();
        ptmpFiles.erase(ptmpFiles.end() - 1);
        QMessageBox::warning(this,programName,tr("无法打开文件%1").arg(sin),QMessageBox::Ok,0);
    }
}

//TODO:处理用户强制打开不支持的文件比如*.exe
void MainWindow::openFiles(QStringList openFiles) {
    QFileInfo tmpFileInfo;
    QString sin;
    foreach  (sin,openFiles) {
        tmpFileInfo.setFile(sin);
        if (openedFileNames.contains(sin,Qt::CaseSensitive)) {
            switchToTab(sin);//如果发现文件已打开，则通知tabwidget切换到该文件
        } else if (tmpFileInfo.isFile()) {
            if (tmpFileInfo.isWritable() && tmpFileInfo.isReadable()) {
                openFile(sin,QIODevice::ReadWrite);
            } else if (tmpFileInfo.isReadable()) {
                openFile(sin,QIODevice::ReadOnly);
            } else {
                QMessageBox::warning(this,programName,tr("尝试打开文件: %1失败").arg(tmpFileInfo.fileName()),QMessageBox::Ok,0);
            }
        } else if (tmpFileInfo.isDir()) {
            on_actionNew_triggered();
        }
    }//foreach
}

void MainWindow::saveFile(int index) {
    saveFile(index,openedFileNames.at(index));
}

void MainWindow::saveFile(int index,QString newFileName) {
    QFile fout(newFileName);
    QFileInfo tmpFileInfo(fout);
    fout.remove();
    QTextStream sout(ptmpFiles.at(index));
    sout << ((ptextEdits.at(index))->toPlainText());
    if ((ptmpFiles.at(index))->copy(newFileName)) {
        codeEditTab->setTabText(index,tmpFileInfo.fileName());
        codeEditTab->setTabToolTip(index,newFileName);
        setTitle(index);
    } else {
        QMessageBox::warning(this,programName,tr("保存文件失败"),QMessageBox::Ok,0);
    }
    (ptmpFiles.at(index))->open();
}

void MainWindow::setTitle(qint32 index) {
    if (index < 0) {
        setWindowTitle(programName);
    } else {
        setWindowTitle(codeEditTab->tabText(index) + " - " + programName);
    }
}

//将标签页切换至指定标签
void MainWindow::switchToTab(QString sin) {
    if (openedFileNames.contains(sin,Qt::CaseSensitive)) {
        codeEditTab->setCurrentIndex(openedFileNames.lastIndexOf(sin,-1));
    }
}

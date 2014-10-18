#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QMouseEvent>
#include <QtWidgets>
#include <ui_mainwindow.h>
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow ,Ui::MainWindow {
    Q_OBJECT

public:
    QStringList argv;

    bool eventFilter(QObject *,QEvent *);
    enum errno{OK,EPERM,ENOENT,EAGAIN = 11,EISDIR = 21};
    explicit MainWindow(qint32 argc,char *argv[],QWidget *parent = 0);
    void openFiles(QStringList);

signals:

private slots:
    void closeEvent(QCloseEvent *);
    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionClose_triggered();
    void on_actionClose_all_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void on_actionExit_triggered();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionPaste_triggered();
    void on_actionRedo_triggered();
    void on_actionSave_triggered();
    void on_actionSave_all_triggered();
    void on_actionSave_as_triggered();
    void on_actionUndo_triggered();
    void on_codeEditTab_tabCloseRequested(int);
    void setTitle(int);

private:
    qint32 argc,untitledNum;
    QByteArray windowGeometry;
    QPoint pos;
    QString programName;
    QStringList openedFileNames,openedTmpFileNames;//实时对应标签页
    std::vector<QTemporaryFile*> ptmpFiles;//实时对应标签页
    std::vector<QTextEdit*> ptextEdits;//实时对应标签页

    qint32 newTab();
    QMenu* createPopupMenu();
    void closeFile(int);
    void openFile(QString,QIODevice::OpenModeFlag);
    void saveFile(int);
    void saveFile(int,QString);
    void switchToTab(QString);
};

#endif // MAINWINDOW_H

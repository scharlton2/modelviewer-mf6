#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MvDoc;
class MvView;

class mvManager;
class mvGUISettings;

class QLabel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    //void open();
    bool save();
    bool saveAs();
    //void about();
    //void documentWasModified();

    void setSize();

protected:
    void closeEvent(QCloseEvent *) override;

private:
    bool            isModified() const;

private:
    void            createActions();
    void            createStatusBar();
    void            readSettings();
    void            writeSettings();
    bool            maybeSave();
    bool            saveFile(const QString &fileName);
    void            setCurrentFile(const QString &fileName);
    QString         strippedName(const QString &fullFileName);

    void            setDataName(const QString &name);
    void            setModelName(const QString &name);

    QLabel *        data;
    QLabel *        model;

    MvDoc *         doc;
    MvView *        view;

    //{{
    mvManager *     manager;
    mvGUISettings * gui;
    //}}


    //QPlainTextEdit *textEdit;
    QString         curFile;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

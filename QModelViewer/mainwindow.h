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
    void onFileOpen();        // application MainWindow::open
    void closeFile();
    //bool save();
    //bool saveAs();

    bool onFileSave();         // save
    bool onFileSaveAs();       // saveAs
    void onFileExportAsBmp();
    void onExportAnimation();

    void updateFileActions();
    void updateShowActions();

    //void about();
    //void documentWasModified();

    void onShowIsosurfaces();
    void onShowNone();
    void onShowSolid();

    void onShowVectors();
    void onShowPathlines();
    void onModelFeatures();

    void onShowAxes();
    void onShowGridLines();
    void onShowGridShell();
    void onShowBoundingBox();
    void onShowOverlay();

    void onShowTime();
    void onShowColorBar();

    bool onSaveDocument(const char *lpszPathName);

    void setSize();

protected:
    void closeEvent(QCloseEvent *) override;

private:
    bool            isModified() const;
    bool            isAnimating() const;

private:
    void            createActions();
    void            createStatusBar();
    void            createMenus();

    void            updateActions();
    void            updateAllViews();
    void            updateStatusBar();

    void            readSettings();
    void            writeSettings();
    bool            maybeSave();
    bool            saveFile(const QString &fileName);           // bool onSaveDocument(const char *lpszPathName);
    void            setCurrentFile(const QString &fileName);
    QString         strippedName(const QString &fullFileName);

    void            setDataName(const QString &name);
    void            setModelName(const QString &name);

    bool            _modifiedFlag;
    bool            modifiedFlag();
    void            setModifiedFlag(bool modifed);

    QLabel *        data;
    QLabel *        model;

    MvDoc *         doc;
    MvView *        view;

    //mvManager *     manager;
    //mvGUISettings * gui;


    //QPlainTextEdit *textEdit;
    QString         curFile;

    Ui::MainWindow *ui;

    // File
    QAction *       newAct;
    QAction *       openAct;
    QAction *       closeAct;
    QAction *       saveAction;
    QAction *       saveAsAction;

    QAction *       exportAsBmpAction;
    QAction *       exportAnimationAction;

    QAction *       printAction;
    QAction *       printPreviewAction;
    QAction *       printSetupAction;

    QAction *       preferencesAction;

    QAction *       exitAct;


    // Show
    QAction *       showSolidAction;
    QAction *       showIsosurfacesAction;
    QAction *       showNoneAction;

    QAction *       showVectorsAction;
    QAction *       showPathlinesAction;
    QAction *       showModelFeaturesAction;

    QAction *       showGridShellAction;
    QAction *       showGridLinesAction;
    QAction *       showAxesAction;
    QAction *       showBoundingBoxAction;
    QAction *       showOverlayAction;

    QAction *       showTimeAction;
    QAction *       showColorBarAction;


    // Action
    QAction *       setSizeAction;
};

#endif // MAINWINDOW_H

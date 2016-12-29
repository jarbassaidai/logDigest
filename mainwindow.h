#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filebrowser.h"
#include "tablayout.h"
#include <boost/container/map.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_pagedown_clicked();

    void on_pageup_clicked();

    void on_top_clicked();

    void on_bottom_clicked();

    void on_search_clicked();

    void on_actionMark_as_signature_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionClose_triggered();

    void on_actionSearch_triggered();

    void on_actionQuit_triggered();

    void on_actionPreferences_triggered();

    void on_actionScroll_down_triggered();

    void on_actionPage_up_triggered();

    void on_actionPage_down_triggered();

    void on_navActionSearch_triggered();

    void on_actionNewtab_triggered();

    void on_action_file_format_triggered();

private:
    Ui::MainWindow *ui;
    QTextBrowser *twtb = nullptr;

    boost::container::map<int,FileBrowser*> m_fmap;
    boost::container::map<int,tabLayout *> m_tlmap;
};

#endif // MAINWINDOW_H

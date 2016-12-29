#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QGroupBox>
#include <QTextBrowser>
#include <QTabWidget>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralWidget->setLayout(ui->vboxLayout);
    ui->vboxLayout->addWidget(ui->mainToolBar);
    ui->vboxLayout->addWidget(ui->tabWidget);


    /*QVBoxLayout *tabVboxLayout = new QVBoxLayout();
    ui->tab->setLayout(tabVboxLayout);
    QTextBrowser * textBrowser = new QTextBrowser();
    twtb = textBrowser;
    tabVboxLayout->addWidget(textBrowser);
    */

    QGroupBox * hgroupBox = new QGroupBox();
    QHBoxLayout * hboxLayout= new QHBoxLayout();

    hboxLayout->addWidget((ui->pagedown));
    hboxLayout->addWidget(ui->pageup);
    hboxLayout->addWidget(ui->top);
    hboxLayout->addWidget(ui->bottom);
    hboxLayout->addWidget(ui->search);
    hgroupBox->setLayout(hboxLayout);

    ui->vboxLayout->addWidget(hgroupBox);

}

MainWindow::~MainWindow()
{
    if (ui->actionClose != nullptr ) { delete ui->actionClose; }
    if (ui->actionSearch != nullptr ) { delete ui->actionSearch; }
    if (ui->actionQuit!= nullptr ) { delete ui->actionQuit ; }
    if (ui->actionPreferences != nullptr ) { delete ui->actionPreferences ; }
    if (ui->actionCut != nullptr ) { delete ui->actionCut; }
    if (ui->actionCopy != nullptr ) { delete ui->actionCopy; }
    if (ui->actionMark_as_signature != nullptr ) { delete ui->actionMark_as_signature ; }
    if (ui->actionOpen != nullptr ) { delete ui->actionOpen ; }
    if (ui->actionScroll_up != nullptr ) { delete ui->actionScroll_up ; }
    if (ui->actionScroll_down != nullptr ) { delete ui->actionScroll_down; }
    if (ui->actionPage_up != nullptr ) { delete ui->actionPage_up ; }
    if (ui->actionPage_down != nullptr ) { delete ui->actionPage_down ; }
    if (ui->navActionSearch != nullptr ) { delete ui->navActionSearch ; }
    if (ui->actionNewtab != nullptr ) { delete ui->actionNewtab ; }
    if (ui->centralWidget != nullptr ) { delete ui->centralWidget ; }
    //if ( ui->vboxLayout != nullptr ) { delete vboxLayout ; }
    //QTabWidget *tabWidget;
    //if (ui->tab != nullptr ) { delete ui->tab ; }
    //if (ui->tab_2 != nullptr ) { delete ui->tab_2 ; }
    if (ui->pagedown != nullptr ) { delete ui->pagedown; }
    if (ui->pageup != nullptr ) { delete ui->pageup ; }
    if (ui->top != nullptr ) { delete ui->top ; }
    if (ui->bottom != nullptr ) { delete ui->bottom ; }
    if (ui->search != nullptr ) { delete ui->search ; }
    //QMenuBar *menuBar;
    //QMenu *file;
    //QMenu *menuEdit;
    //QMenu *menuNavigation;
    //QToolBar *mainToolBar;
    //QStatusBar *statusBar;
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"), QString(),
            tr("Text Files (*.log);;or any other text file *.*"));
    QTabBar *tabBar = nullptr;
    if (!fileName.isEmpty()) {
        tabBar = (QTabBar *) ui->tabWidget->currentWidget();
        if (tabBar == nullptr) {
            tabBar = new QTabBar(ui->tabWidget);
            ui->tabWidget->addTab(tabBar,"new");
        }
        FileBrowser * fb = m_fmap[ui->tabWidget->currentIndex()];

        if (fb == nullptr){
            m_tlmap[ui->tabWidget->currentIndex()]= new tabLayout(tabBar);
            fb = new FileBrowser(m_tlmap[ui->tabWidget->currentIndex()]->getTextBrowser(),fileName);
            m_fmap[ui->tabWidget->currentIndex()]=fb;
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),fileName);
        }else {
            on_actionClose_triggered();
        }


/**
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
        QTextStream in(&file);
        ui->textBrowser->setText(in.read(m_browserReadSize));
        //ui->textEdit->setText(in.readAll());
        file.close();
 **/
    }
}
// page down


void MainWindow::on_pagedown_clicked()
{
    m_fmap[ui->tabWidget->currentIndex()]->pageDown();
}

void MainWindow::on_pageup_clicked()
{
    m_fmap[ui->tabWidget->currentIndex()]->pageUp();
}

void MainWindow::on_top_clicked()
{

}

void MainWindow::on_bottom_clicked()
{

}

void MainWindow::on_search_clicked()
{

}

void MainWindow::on_actionMark_as_signature_triggered()
{

}

void MainWindow::on_actionCut_triggered()
{

}

void MainWindow::on_actionCopy_triggered()
{

}

void MainWindow::on_actionClose_triggered()
{
    FileBrowser *fb = m_fmap[ui->tabWidget->currentIndex()];
    QMessageBox msgBox;
     msgBox.setText("Closing file " + fb->getFileName());
     msgBox.setInformativeText("Do you want close the file ? ");
     msgBox.setStandardButtons(QMessageBox::Close | QMessageBox::Cancel);
     msgBox.setDefaultButton(QMessageBox::Close);
     int ret = msgBox.exec();

   if ( ret  == QMessageBox::Close ) {

       delete fb;
       m_fmap.erase(ui->tabWidget->currentIndex());
       tabLayout *tl = m_tlmap[ui->tabWidget->currentIndex()];
       delete tl;
       m_tlmap.erase(ui->tabWidget->currentIndex());
   }else {
       return;
   }

}

void MainWindow::on_actionSearch_triggered()
{

}

void MainWindow::on_actionQuit_triggered()
{
    QMessageBox msgBox;
     msgBox.setText(tr("Quit"));
     msgBox.setInformativeText("Do you want to quit? ");
     msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
     msgBox.setDefaultButton(QMessageBox::Ok);
     int ret = msgBox.exec();

   if ( ret  == QMessageBox::Ok ) {
       int cnt = m_fmap.size();
       for (int i = 0 ; i < cnt ; i++ ){
           delete m_fmap[i];
           m_fmap.erase(i);
           delete m_tlmap[i];
           m_tlmap.erase(i);

       }
   }
   exit(0);
}

void MainWindow::on_actionPreferences_triggered()
{

}

void MainWindow::on_actionScroll_down_triggered()
{

}

void MainWindow::on_actionPage_up_triggered()
{

}

void MainWindow::on_actionPage_down_triggered()
{

}

void MainWindow::on_navActionSearch_triggered()
{

}
void MainWindow::on_actionNewtab_triggered()
{
    QTabBar *tabBar = new QTabBar(ui->tabWidget);
    ui->tabWidget->addTab(tabBar,"new");
}

void MainWindow::on_action_file_format_triggered()
{
    QTabBar *tabBar = new QTabBar(ui->tabWidget);
    ui->tabWidget->addTab(tabBar,"FileFormat");


}

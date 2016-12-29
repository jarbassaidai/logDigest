#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QMessageBox>




class FileBrowser : public QObject
{
    Q_OBJECT


public:
    explicit FileBrowser(QTextEdit *parent=nullptr,QString filename = "");
    virtual ~FileBrowser() ;
    void pageDown();
    void pageUp();
    void top();
    void bottom();
    inline QString getFileName() { return m_filename; }

signals:

public slots:

protected:
    QFile * m_in = nullptr;
    QTextStream *m_ts = nullptr;
    long m_prevPageTop=0;
    long m_nextPageTop= 0;


    void setFPos();

private:
    const long m_browserReadSize=8192;
    QTextEdit * m_qtb;
    QString m_filename;
};

#endif // FILEBROWSER_H

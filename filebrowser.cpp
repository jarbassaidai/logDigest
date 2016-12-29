#include "filebrowser.h"

FileBrowser::FileBrowser(QTextEdit *parent,QString fileName)
{
    m_in = new QFile(fileName);
    if (!m_in->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(m_qtb, tr("Error"), tr("Could not open file"));
        return;
    }
    m_filename = fileName;
    m_qtb = parent;
    m_ts = new QTextStream(m_in);
    m_qtb->setText(m_ts->read(m_browserReadSize));
    m_nextPageTop=m_ts->pos();
}

FileBrowser::~FileBrowser()
{
 delete m_ts;
 m_in->close();
 delete m_in;

}

void FileBrowser::pageDown()
{
    m_qtb->setText(m_ts->read(m_browserReadSize));
    setFPos();
}

void FileBrowser::setFPos()
{
    m_nextPageTop = m_ts->pos();
    m_prevPageTop = (m_nextPageTop - (2 * m_browserReadSize) > 0  ) ? m_nextPageTop -  ( 2 * m_browserReadSize) : 0 ;
}

void FileBrowser::pageUp()
{
    if (m_ts->seek(m_prevPageTop)) {
        m_qtb->setText(m_ts->read(m_browserReadSize));
        setFPos();
    }else {
        QMessageBox::critical(m_qtb, tr("Error"), tr("failed to page up "));
    }
}

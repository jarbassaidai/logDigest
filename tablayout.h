#ifndef TABFILE_H
#define TABFILE_H

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTabBar>
#include "highlighter.h"

class tabLayout : public QWidget
{
    Q_OBJECT
public:
    explicit tabLayout(QWidget *parent = 0);
    virtual ~tabLayout() ;
    inline QTextEdit * getTextBrowser() { return m_tb; }
signals:

public slots:

protected:
    QTextEdit *m_tb = nullptr;
    QVBoxLayout  *m_vboxLayout = nullptr ;
    QGroupBox    *m_groupBox = nullptr;
    QHBoxLayout  *m_hboxLayout = nullptr;
    QTabBar      *m_parent = nullptr;

    highlighter * m_highlighter = nullptr;
};

#endif // TABFILE_H

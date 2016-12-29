#include "tablayout.h"

tabLayout::tabLayout(QWidget *parent) : QWidget(parent)
{
    m_parent = (QTabBar *)parent;
    m_vboxLayout = new QVBoxLayout();
    parent->setLayout(m_vboxLayout);
    m_tb = new QTextEdit();
    m_highlighter = new highlighter(m_tb->document());

    m_vboxLayout->addWidget(m_tb);
}

tabLayout::~tabLayout()
{
    delete m_vboxLayout;
    delete m_tb;


}

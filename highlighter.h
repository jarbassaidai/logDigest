#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H
#include <QWidget>
#include <QTextDocument>
#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QVector>
#include <QTextCharFormat>
#include <QString>
#include <QTextCharFormat>



class highlighter :  public QSyntaxHighlighter
{
    Q_OBJECT

public:
    highlighter(QTextDocument * parent =0);
    void addRules(QTextCharFormat &, const char *, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

protected:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> m_highlightingRules;
    QTextCharFormat m_datetime;
    QTextCharFormat m_info;
    QTextCharFormat m_warning;
    QTextCharFormat m_error;
    QTextCharFormat m_fatal;
    QTextCharFormat m_stackDump;
    QTextCharFormat m_signature;
    QTextCharFormat m_mark;
    QTextCharFormat m_debug;
    QTextCharFormat m_ipv4;

    void highlightBlock(const QString &text) override;
};

#endif // HIGHLIGHTER_H

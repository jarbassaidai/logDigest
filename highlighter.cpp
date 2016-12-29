#include "highlighter.h"
/*
 * This will be a general highliter for all the variations pulled from the
 * postgres tables
 *  signatures of know bugs
 *  file formats supplied by the user
 *  time stamp
 *  dynamic search strings entered
 *
 */

highlighter::highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    m_debug.setForeground(Qt::blue);
    m_info.setFontItalic(true);
    m_warning.setFontWeight(QFont::Bold);
    m_warning.setForeground(Qt::darkMagenta);
    m_error.setForeground(Qt::darkCyan);
    m_error.setFontWeight(QFont::Bold);
    m_fatal.setFontWeight(QFont::Bold);
    m_fatal.setForeground(Qt::red);
    m_signature.setFontUnderline(true);
    m_signature.setForeground(Qt::green);
    m_ipv4.setForeground((Qt::darkGreen));
    m_datetime.setForeground(Qt::magenta);

    HighlightingRule rule;

    addRules(m_debug,"\\[debug\\]");
    addRules(m_info,"\\[info\\]");
    addRules(m_warning,"\\[warning\\]");
    addRules(m_error,"\\[error\\]");
    addRules(m_fatal,"\\[fatal\\]");
    addRules(m_ipv4,"\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}(|:\\d{1,5})");
    addRules(m_datetime,"\\[\\d{4}-\\d{1,2}-\\d{1,2} \\d{2}:\\d{2}:\\d{2},\\d{3}\\]");



}

void highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, m_highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

}

void highlighter::addRules(QTextCharFormat & tcf, const char * pattern ,Qt::CaseSensitivity cs)
{
    HighlightingRule rule;
    rule.format = tcf;
    rule.pattern = QRegExp(pattern,cs);
    m_highlightingRules.append(rule);

}

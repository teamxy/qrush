#include "../include/Highlighter.h"

Highlighter::Highlighter(QTextDocument* parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;

    keywordPatterns << "\\bvar\\b" << "\\bfunction\\b" << "\\bif\\b"
                    << "\\belse\\b" << "\\;" << "\\breturn\\b"
                    << "\\break\\b" << "\\bswitch\\b" << "\\bfor\\b"
                    << "\\bwhile\\b" << "\\bthis\\b" << "\\bswitch\\b"
                    << "\\bnull\\b" << "\\bundefined\\b" << "\\btrue\\b"
                    << "\\bfalse\\b" << "\\bcase\\b" << "\\bcontinue\\b"
                    << "\\bbreak\\b" << "\\bcase\\b" << "\\bdefault\\b" ;

    singleLineCommentFormat.setForeground(Qt::darkGray);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGray);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

    functionFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+\\s*(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    doubleQuotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = doubleQuotationFormat;
    highlightingRules.append(rule);

    singleQuotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\'.*\'");
    rule.format = singleQuotationFormat;
    highlightingRules.append(rule);

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

}

void Highlighter::highlightBlock(const QString &text)
{
    //
    // highlight keywords: var, if, else, function, ...
    //

    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    //
    // highlight multiline comment sections
    //

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

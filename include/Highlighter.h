#ifndef QRUSH_HIGHLIGHTER_H_
#define QRUSH_HIGHLIGHTER_H_

#include <QSyntaxHighlighter>
#include <QTextCharFormat>


class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument* parent);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat doubleQuotationFormat;
    QTextCharFormat singleQuotationFormat;
    QTextCharFormat functionFormat;
};

#endif // QRUSH_HIGHLIGHTER_H_

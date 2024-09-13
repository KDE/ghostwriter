#ifndef DOCUMENT_STORE_H
#define DOCUMENT_STORE_H

#include "markdowndocument.h"

#include <QObject>
#include <QUrl>

namespace ghostwriter
{

class DocumentStore
{
    Q_OBJECT

public:
    static DocumentStore *instance();
    ~DocumentStore();

    MarkdownDocument *load(const QUrl &url) const;

    void save(MarkdownDocument *document) const;

signals:
    void draftLocationChanged(const QUrl &url);

private:
    DocumentStore();
};

} // namespace ghostwriter

#endif // DOCUMENT_STORE_H

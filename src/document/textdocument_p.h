#include <memory>

#include "textdocument.h"

namespace ghostwriter
{

std::unique_ptr<TextDocument *> createTextDocument(const QUrl &url,
                                                   const QString &contents,
                                                   const std::unique_ptr<TextIO> &textIO,
                                                   const QString &draftName = QString(),
                                                   QObject *parent = nullptr);

} // namespace ghostwriter
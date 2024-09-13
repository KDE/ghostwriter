#ifndef AUTOSAVE_H
#define AUTOSAVE_H

#include <QObject>
#include <QScopedPointer>

#include "../markdowndocument.h"

namespace ghostwriter
{

class AutoSavePrivate;
class AutoSave
{
    Q_OBJECT

public:
    AutoSave();
    ~AutoSave();

    int saveInterval() const;
    void setSaveInterval(int seconds);

    void addDocument(MarkdownDocument *document);

signals:
    void saveError(const QString &description);

public slots:
    void saveAll();

private:
    QScopedPointer<AutoSavePrivate *> d;
};

} // namespace ghostwriter

#endif // AUTOSAVE_H

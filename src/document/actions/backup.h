#ifndef BACKUP_H
#define BACKUP_H

#include <QDateTime>
#include <QList>
#include <QString>

#include "../markdowndocument.h"

namespace ghostwriter
{

class FileVersion
{
public:
    FileVersion(int number, const QString &path);
    ~FileVersion();

    int number() const;
    QString path() const;
    QDateTime timestamp() const;
};

class Backup
{
public:
    Backup(MarkdownDocument *document);
    ~Backup();

    QString backupDir() const;
    bool setBackupDir(const QString &path);

    QString extension() const;
    bool setExtension(const QString &extension);

    int maxVersions() const;
    bool setMaxVersions(int max) const;

    QList<FileVersion> versions() const;

    bool saveBackup();

    bool revert();
    bool revertTo(int versionNumber);

    QString errorString() const;
};

} // namespace ghostwriter

#endif // BACKUP_H
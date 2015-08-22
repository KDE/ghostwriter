#ifndef MARKUPDOCUMENT_H
#define MARKUPDOCUMENT_H

#include <QTextDocument>
#include <QString>
#include <QDateTime>

class TextDocument : public QTextDocument
{
    Q_OBJECT

    public:
        TextDocument(QObject* parent = 0);
        virtual ~TextDocument();

        QString getDisplayName() const;
        QString getFilePath() const;
        void setFilePath(const QString& path);

        /**
         * Returns true if the document is new with no file path.
         */
        bool isNew() const;

        /**
         * Returns true if the document has read only permissions.
         */
        bool isReadOnly() const;

        /**
         * Sets whether the document has read only permissions.
         */
        bool setReadOnly(bool readOnly);

        /**
         * Gets the timestamp of the document, which is useful when comparing
         * the last modified time of the file represented on disk.
         */
        QDateTime getTimestamp() const;

        /**
         * Sets a timestamp to the document, which is useful when comparing
         * the last modified time of the file represented on disk.
         */
        bool setTimestamp(const QDateTime& timestamp);

    signals:
        void filePathChanged();

    private:
        QString displayName;
        QString filePath;
        bool readOnlyFlag;
        QDateTime timestamp;
};

#endif // MARKUPDOCUMENT_H

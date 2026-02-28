/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <QObject>
#include <QString>
#include <QUrl>

#include "io/textio.h"
#include "textdocument.h"
#include "util/kerrorcode.h"
#include "util/kresult.h"

namespace ghostwriter
{
/**
 * @brief Interface for creating and opening TextDocument objects.
 *
 * Implementations of this interface provide methods to create new untitled
 * draft documents as well as open existing documents from file URLs.
 */
class TextDocumentFactory
{
public:
    using Result = KResult<TextDocument *, TextIOError>;

    /**
     * @brief Destroys the Text Document Factory object.
     */
    virtual ~TextDocumentFactory() = default;

    /**
     * Creates a new, untitled (draft) document with a backing file at the
     * draft directory given by draftLocation().
     *
     * If unsuccessful in creating a backing file for the draft, a valid document
     * object will still be returned. However, its error() method will return true,
     * and errorString() will return a description of the error. In other words,
     * the document won't have a backing draft file on disk, and will be at risk of
     * data loss until a call to saveAs() can persist its contents.
     *
     * @param draftName Name to use for the draft document's display name.
     * @param draftURL Untitled draft URL for use as temporary storage and display name.
     * @param parent Parent object, or nullptr for no parent.
     *
     * @return Document object if successful, TextIOError error code otherwise.
     */
    virtual Result create(const QString &draftName, const QUrl &draftURL, QObject *parent = nullptr) = 0;

    /**
     * Opens the file contents at the given file path.
     *
     * If unsuccessful, error() will return true, and errorString() will return a
     * description of the error.
     *
     * @param url File URL to a local path from which to load the document.
     *
     * @return Document object if successful, TextIOError error code otherwise.
     */
    virtual Result open(const QUrl &url, QObject *parent = nullptr) = 0;

    /**
     * Opens an untitled draft backed by the given local file path.
     *
     * If unsuccessful, error() will return true, and errorString() will return a
     * description of the error.
     *
     * @param path Local file path from which to load the draft document.
     *
     * @return Document object if successful, TextIOError error code otherwise.
     */
    virtual Result openDraft(const QUrl &draftName, const QUrl &url, QObject *parent = nullptr) = 0;
};

} // namespace ghostwriter

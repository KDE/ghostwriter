/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "textdocumentfactoryimpl.h"
#include "io/textio.h"
#include "io/textiofactory.h"
#include "textdocument.h"

using namespace ghostwriter;

TextDocumentFactory::Result TextDocumentFactoryImpl::create(const QString &draftName, const QUrl &draftURL, QObject *parent)
{
    std::unique_ptr<TextIO> textIO = TextIOFactory::createTextIO(draftURL);

    TextDocument *document = new TextDocument(draftURL, QString(), textIO, draftName, parent);
    // Implementation code to create a new TextDocument draft
    // ...
    return {}; // Return appropriate Result
}

/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "io/textiofactory.h"
#include "textdocumentfactory.h"

namespace ghostwriter
{
class TextDocumentFactoryImpl : public TextDocumentFactory
{
public:
    TextDocumentFactoryImpl() = default;
    ~TextDocumentFactoryImpl() override = default;
    TextDocumentFactory::Result create(const QString &draftName, const QUrl &draftURL, QObject *parent = nullptr) override;
    TextDocumentFactory::Result open(const QUrl &url, QObject *parent = nullptr) override;
    TextDocumentFactory::Result openDraft(const QUrl &draftURL, const QUrl &url, QObject *parent = nullptr) override;

private:
    TextIOFactory m_textIOFactory;
};
} // namespace ghostwriter

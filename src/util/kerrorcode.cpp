/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "kerrorcode.h"

#include <QTextStream>

KErrorCode::KErrorCode(const int code, const QString &message) noexcept
    : m_code(code)
    , m_message(message)
{
}

KErrorCode::KErrorCode(const int code, QString &&message) noexcept
    : m_code(code)
    , m_message(std::move(message))
{
}

QString KErrorCode::message() const & noexcept
{
    return m_message;
}

QString &&KErrorCode::message() && noexcept
{
    return std::move(m_message);
}

QString KErrorCode::toString() const noexcept
{
    return QString("Error code %1: %2").arg(m_code).arg(m_message);
}

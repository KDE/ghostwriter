/***********************************************************************
 *
 * Copyright (C) 2018-2022 wereturtle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "previewproxy.h"

namespace ghostwriter
{
PreviewProxy::PreviewProxy(QObject *parent)
    : QObject(parent),
      m_htmlContent(""),
      m_styleSheet(""),
      m_mathEnabled(false)
{
    ;
}

PreviewProxy::~PreviewProxy()
{
    ;
}

void PreviewProxy::setHtmlContent(const QString &html)
{
    m_htmlContent = html;
    emit htmlChanged(m_htmlContent);
}

QString PreviewProxy::htmlContent() const
{
    return m_htmlContent;
}

void PreviewProxy::setStyleSheet(const QString &css)
{
    m_styleSheet = css;
    emit styleSheetChanged(m_styleSheet);
}

QString PreviewProxy::styleSheet() const
{
    return m_styleSheet;
}

void PreviewProxy::setMathEnabled(bool enabled)
{
    if (enabled != m_mathEnabled) {
        m_mathEnabled = enabled;
        emit mathToggled(m_mathEnabled);
    }
}

bool PreviewProxy::mathEnabled() const
{
    return m_mathEnabled;
}
} // namespace ghostwriter

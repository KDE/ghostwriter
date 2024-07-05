/**
 * SPDX-FileCopyrightText: 2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <QDebug>
#include <QFileInfo>
#include <QHashIterator>
#include <QIconEngine>
#include <QMultiHash>
#include <QPainter>
#include <QRect>
#include <QtGlobal>

#include "svgicontheme.h"

namespace ghostwriter
{
class SvgIconThemeEngine : public QIconEngine
{
public:
    SvgIconThemeEngine(const QIcon &icon, const SvgIconTheme *theme);
    ~SvgIconThemeEngine();

    void setIcon(const QIcon &icon);

    SvgIconThemeEngine *clone() const override;
    virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    virtual QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;

private:
    // The icon being painted per the theme's color specification (extrinsic value).
    QIcon m_icon;

    // The "flyweight" (intrinsic value)
    const SvgIconTheme *m_theme;
};

class SvgIconThemePrivate
{
public:
    SvgIconThemePrivate()
    {
        normalColor = QColor(Qt::black);
        activeColor = QColor(Qt::black);
        selectedColor = QColor(Qt::black);
        disabledColor = QColor(Qt::black);
    }

    ~SvgIconThemePrivate()
    {
        ;
    }

    QString themePath;

    QColor normalColor;
    QColor activeColor;
    QColor selectedColor;
    QColor disabledColor;

    QIcon loadBaseIcon(const QString &iconName) const;
    QString fullIconPath(const QString &name) const;
};

/////// SvgIconTheme ///////

SvgIconTheme::SvgIconTheme(const QString &themePath)
    : d(new SvgIconThemePrivate())
{
    d->themePath = themePath;

    if (!QFile::exists(themePath)) {
        qWarning() << "SVG icon theme path does not exist:" << themePath;
    }
}

SvgIconTheme::~SvgIconTheme()
{
    ;
}

QString SvgIconTheme::path() const
{
    return d->themePath;
}

QIcon SvgIconTheme::icon(const QString &name) const
{
    SvgIconThemeEngine *engine = new SvgIconThemeEngine(d->loadBaseIcon(name), this);
    return QIcon(engine);
}

QIcon SvgIconTheme::checkableIcon(const QString &uncheckedName, const QString &checkedName) const
{
    QString uncheckedIconPath = d->fullIconPath(uncheckedName);
    QString checkedIconPath = d->fullIconPath(checkedName);
    QPixmap uncheckedPixmap;
    QPixmap checkedPixmap;

    if (!uncheckedIconPath.isEmpty()) {
        uncheckedPixmap = QPixmap(uncheckedIconPath);
    }

    if (!checkedIconPath.isEmpty()) {
        checkedPixmap = QPixmap(checkedIconPath);
    }

    QIcon icon(uncheckedPixmap);
    icon.addPixmap(checkedPixmap, QIcon::Normal, QIcon::On);
    icon.addPixmap(checkedPixmap, QIcon::Active, QIcon::On);
    icon.addPixmap(checkedPixmap, QIcon::Disabled, QIcon::On);
    icon.addPixmap(checkedPixmap, QIcon::Selected, QIcon::On);

    SvgIconThemeEngine *engine = new SvgIconThemeEngine(icon, this);
    return QIcon(engine);
}

void SvgIconTheme::setColor(QIcon::Mode mode, const QColor &color)
{
    switch (mode) {
    case QIcon::Normal:
        d->normalColor = color;
        break;
    case QIcon::Active:
        d->activeColor = color;
        break;
    case QIcon::Disabled:
        d->disabledColor = color;
        break;
    case QIcon::Selected:
        d->selectedColor = color;
        break;
    default:
        qWarning() << "SvgIconTheme::setColor: Invalid QIcon mode specified.";
        break;
    }
}

QColor SvgIconTheme::color(QIcon::Mode mode) const
{
    QColor retval;

    switch (mode) {
    case QIcon::Normal:
        retval = d->normalColor;
        break;
    case QIcon::Active:
        retval = d->activeColor;
        break;
    case QIcon::Disabled:
        retval = d->disabledColor;
        break;
    case QIcon::Selected:
        retval = d->selectedColor;
        break;
    default:
        retval = d->normalColor;
        break;
    }

    if (!retval.isValid()) {
        if (d->normalColor.isValid()) {
            retval = d->normalColor;
        } else {
            retval = Qt::black;
        }
    }

    return retval;
}

/////// SvgIconThemeEngine ///////

SvgIconThemeEngine::SvgIconThemeEngine(const QIcon &icon, const SvgIconTheme *theme)
    : m_icon(icon)
    , m_theme(theme)
{
    ;
}

SvgIconThemeEngine::~SvgIconThemeEngine()
{
    ;
}

SvgIconThemeEngine *SvgIconThemeEngine::clone() const
{
    return new SvgIconThemeEngine(m_icon, m_theme);
}

void SvgIconThemeEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    QColor color = m_theme->color(mode);

    if (QIcon::On == state) {
        color = m_theme->color(QIcon::Active);
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::LosslessImageRendering);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->setPen(color);
    painter->setBrush(color);

    m_icon.paint(painter, rect, Qt::AlignCenter | Qt::AlignVCenter, mode, state);
    painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter->fillRect(rect, color);
    painter->restore();
}

QPixmap SvgIconThemeEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    paint(&painter, QRect(QPoint(0, 0), size), mode, state);

    return pixmap;
}

/////// SvgIconThemePrivate ///////

QIcon SvgIconThemePrivate::loadBaseIcon(const QString &name) const
{
    QString fullPath = fullIconPath(name);

    if (fullPath.isNull() || fullPath.isEmpty()) {
        return QIcon();
    }

    return QIcon(fullPath);
}

QString SvgIconThemePrivate::fullIconPath(const QString &name) const
{
    QString fullPath = themePath + "/" + name + ".svg";

    if (!QFileInfo::exists(fullPath)) {
        if (QFileInfo::exists(name)) {
            fullPath = name;
        } else {
            qWarning() << "icon not found: " << name;
            return QString();
        }
    }

    return fullPath;
}

} // namespace ghostwriter

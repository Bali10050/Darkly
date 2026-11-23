#ifndef DARKLY_BUTTONS_H
#define DARKLY_BUTTONS_H

/*
 * Copyright 2014  Martin Gräßlin <mgraesslin@kde.org>
 * Copyright 2014  Hugo Pereira Da Costa <hugo.pereira@free.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include "darklydecoration.h"
#include <KDecoration3/DecorationButton>

#include <QHash>
#include <QImage>

class QVariantAnimation;

namespace Darkly
{

class Button : public KDecoration3::DecorationButton
{
    Q_OBJECT

public:
    //* constructor
    explicit Button(QObject *parent, const QVariantList &args);

    //* destructor
    virtual ~Button() = default;

    //* button creation
    static Button *create(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent);

    //* render
    virtual void paint(QPainter *painter, const QRectF &repaintRegion) override;

    //* flag
    enum Flag {
        FlagNone,
        FlagStandalone,
        FlagFirstInList,
        FlagLastInList
    };

    //* flag
    void setFlag(Flag value)
    {
        m_flag = value;
    }

    //* standalone buttons
    bool isStandAlone() const
    {
        return m_flag == FlagStandalone;
    }

    //* offset
    void setOffset(const QPointF &value)
    {
        m_offset = value;
    }

    //* horizontal offset, for rendering
    void setHorizontalOffset(qreal value)
    {
        m_offset.setX(value);
    }

    //* vertical offset, for rendering
    void setVerticalOffset(qreal value)
    {
        m_offset.setY(value);
    }

    //* set icon size
    void setIconSize(const QSizeF &size)
    {
        m_iconSize = size;
    }

    //*@name active state change animation
    //@{
    void setOpacity(qreal value)
    {
        if (m_opacity == value)
            return;
        m_opacity = value;
        update();
    }

    qreal opacity() const
    {
        return m_opacity;
    }

    //* padding
    void setPadding(const QMargins &value)
    {
        m_padding = value;
    }

    //* left padding, for rendering
    void setLeftPadding(qreal value)
    {
        m_padding.setLeft(value);
    }

    //* right padding, for rendering
    void setRightPadding(qreal value)
    {
        m_padding.setRight(value);
    }

    void setPreferredSize(const QSizeF &size)
    {
        m_preferredSize = size;
    }

    QSizeF preferredSize() const
    {
        return m_preferredSize;
    }

    //@}

private Q_SLOTS:

    //* apply configuration changes
    void reconfigure();

    //* animation state
    void updateAnimationState(bool);

private:
    //* private constructor
    explicit Button(KDecoration3::DecorationButtonType type, Decoration *decoration, QObject *parent = nullptr);

    //* draw button icon
    void drawIcon(QPainter *) const;
    void drawTrafficLight(QPainter *) const;

    //*@name colors
    //@{
    QColor foregroundColor() const;
    QColor backgroundColor() const;
    //@}

    Flag m_flag = FlagNone;

    //* active state change animation
    QVariantAnimation *m_animation;

    //* vertical offset (for rendering)
    QPointF m_offset;

    //* active state change opacity
    qreal m_opacity = 0;

    //* padding (for rendering)
    QMargins m_padding;

    //* icon size
    QSizeF m_iconSize;

    //* implicit size
    QSizeF m_preferredSize;
};

} // namespace

#endif

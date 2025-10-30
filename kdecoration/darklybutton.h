/*
 * SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
    explicit Button(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent = nullptr);

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

    //*@name active state change animation
    //@{
    void setOpacity(qreal value)
    {
        if (m_opacity == value) {
            return;
        }
        m_opacity = value;
        update();
    }

    qreal opacity() const
    {
        return m_opacity;
    }

    //@}

    void setPreferredSize(const QSizeF &size)
    {
        if (m_preferredSize != size) {
            m_preferredSize = size;
            Q_EMIT preferredSizeChanged();
        }
    }

    QSizeF preferredSize() const
    {
        return m_preferredSize;
    }

Q_SIGNALS:
    void preferredSizeChanged();

protected:
    //* padding (for rendering)
    QMargins m_padding;

    //* implicit size
    QSizeF m_preferredSize;

    //* active state change opacity
    qreal m_opacity = 0;
};

class IconButton : public Button
{
    Q_OBJECT

public:
    //* constructor
    explicit IconButton(QObject *parent, const QVariantList &args);

    //* destructor
    virtual ~IconButton() = default;

    //* button creation
    static IconButton *create(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent);

    //* render
    void paint(QPainter *painter, const QRectF &repaintRegion) override;

private Q_SLOTS:

    //* apply configuration changes
    void reconfigure();

    //* animation state
    void updateAnimationState(bool);

private:
    //* private constructor
    explicit IconButton(KDecoration3::DecorationButtonType type, Decoration *decoration, QObject *parent = nullptr);

    //* draw button icon
    void drawIcon(QPainter *) const;

    //*@name colors
    //@{
    QColor foregroundColor() const;
    QColor backgroundColor() const;
    //@}

    //* active state change animation
    QVariantAnimation *m_animation;
};

} // namespace

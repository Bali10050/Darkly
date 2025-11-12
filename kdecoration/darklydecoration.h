/*
 * SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include "darkly.h"
#include "darklysettings.h"

#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationSettings>

#include <QPalette>
#include <QVariant>
#include <QVariantAnimation>

namespace KDecoration3
{
class DecorationButton;
class DecorationButtonGroup;
}

namespace Darkly
{
class Decoration : public KDecoration3::Decoration
{
    Q_OBJECT

public:
    explicit Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
    ~Decoration() override;

    void paint(QPainter *painter, const QRectF &repaintRegion) override;

    //* internal settings
    InternalSettingsPtr internalSettings() const
    {
        return m_internalSettings;
    }

    qreal animationsDuration() const
    {
        return m_animation->duration();
    }

    //* caption height
    qreal captionHeight() const;

    //* button size
    int buttonSize() const;

    //*@name active state change animation
    //@{
    void setOpacity(qreal);

    qreal opacity() const
    {
        return m_opacity;
    }

    //@}

    //*@name colors
    //@{
    QColor titleBarColor() const;
    QColor fontColor() const;
    //@}

    //*@name maximization modes
    //@{
    inline bool isMaximized() const;
    inline bool isMaximizedHorizontally() const;
    inline bool isMaximizedVertically() const;

    inline bool isLeftEdge() const;
    inline bool isRightEdge() const;
    inline bool isTopEdge() const;
    inline bool isBottomEdge() const;

    inline bool hideTitleBar() const;
    //@}

Q_SIGNALS:
    void tabletModeChanged();

public Q_SLOTS:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool init() override;
#else
    void init() override;
#endif

private Q_SLOTS:
    void reconfigure();
    void recalculateBorders();
    void updateButtonsGeometry();
    void updateButtonsGeometryDelayed();
    void updateTitleBar();
    void updateAnimationState();
    void onTabletModeChanged(bool mode);
    void updateScale();

private:
    //* return the rect in which caption will be drawn
    QPair<QRectF, Qt::Alignment> captionRect() const;

    void createButtons();
    void paintTitleBar(QPainter *painter, const QRectF &repaintRegion);
    void updateShadow();
    std::shared_ptr<KDecoration3::DecorationShadow> createShadowObject(const float strengthScale);
    void setScaledCornerRadius();

    //*@name border size
    //@{
    qreal borderSize(bool bottom, qreal scale) const;
    inline bool hasBorders() const;
    inline bool hasNoBorders() const;
    inline bool hasNoSideBorders() const;
    QMarginsF bordersFor(qreal scale) const;
    //@}

    inline bool outlinesEnabled() const;

    InternalSettingsPtr m_internalSettings;
    KDecoration3::DecorationButtonGroup *m_leftButtons = nullptr;
    KDecoration3::DecorationButtonGroup *m_rightButtons = nullptr;

    //* active state change animation
    QVariantAnimation *m_animation;
    QVariantAnimation *m_shadowAnimation;

    //* active state change opacity
    qreal m_opacity = 0;
    qreal m_shadowOpacity = 0;

    //*frame corner radius, scaled according to DPI
    qreal m_scaledCornerRadius = 3;

    bool m_tabletMode = false;
};

bool Decoration::hasBorders() const
{
    if (m_internalSettings && m_internalSettings->mask() & BorderSize) {
        return m_internalSettings->borderSize() > InternalSettings::BorderNoSides;
    } else {
        return settings()->borderSize() > KDecoration3::BorderSize::NoSides;
    }
}

bool Decoration::hasNoBorders() const
{
    if (m_internalSettings && m_internalSettings->mask() & BorderSize) {
        return m_internalSettings->borderSize() == InternalSettings::BorderNone;
    } else {
        return settings()->borderSize() == KDecoration3::BorderSize::None;
    }
}

bool Decoration::hasNoSideBorders() const
{
    if (m_internalSettings && m_internalSettings->mask() & BorderSize) {
        return m_internalSettings->borderSize() == InternalSettings::BorderNoSides;
    } else {
        return settings()->borderSize() == KDecoration3::BorderSize::NoSides;
    }
}

bool Decoration::isMaximized() const
{
    return window()->isMaximized() && !m_internalSettings->drawBorderOnMaximizedWindows();
}

bool Decoration::isMaximizedHorizontally() const
{
    return window()->isMaximizedHorizontally() && !m_internalSettings->drawBorderOnMaximizedWindows();
}

bool Decoration::isMaximizedVertically() const
{
    return window()->isMaximizedVertically() && !m_internalSettings->drawBorderOnMaximizedWindows();
}

bool Decoration::isLeftEdge() const
{
    return (window()->isMaximizedHorizontally() || window()->adjacentScreenEdges().testFlag(Qt::LeftEdge))
        && !m_internalSettings->drawBorderOnMaximizedWindows();
}

bool Decoration::isRightEdge() const
{
    return (window()->isMaximizedHorizontally() || window()->adjacentScreenEdges().testFlag(Qt::RightEdge))
        && !m_internalSettings->drawBorderOnMaximizedWindows();
}

bool Decoration::isTopEdge() const
{
    return (window()->isMaximizedVertically() || window()->adjacentScreenEdges().testFlag(Qt::TopEdge)) && !m_internalSettings->drawBorderOnMaximizedWindows();
}

bool Decoration::isBottomEdge() const
{
    return (window()->isMaximizedVertically() || window()->adjacentScreenEdges().testFlag(Qt::BottomEdge))
        && !m_internalSettings->drawBorderOnMaximizedWindows();
}

bool Decoration::hideTitleBar() const
{
    return m_internalSettings->hideTitleBar() && !window()->isShaded();
}

bool Decoration::outlinesEnabled() const
{
    return (m_internalSettings->outlineIntensity() != InternalSettings::OutlineOff);
}
}

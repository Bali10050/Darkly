/*
 * SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2018 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 * SPDX-FileCopyrightText: 2021 Paul McAuley <kde@paulmcauley.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "darklydecoration.h"

#include "darklysettingsprovider.h"

#include "darklyapplicationmenu.h"
#include "darklybutton.h"

#include "darklyboxshadowrenderer.h"

#include <KDecoration3/DecorationButtonGroup>
#include <KDecoration3/DecorationShadow>
#include <KDecoration3/ScaleHelpers>

#include <KColorUtils>
#include <KConfigGroup>
#include <KPluginFactory>
#include <KSharedConfig>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QPainter>
#include <QPainterPath>
#include <QTextStream>
#include <QTimer>

K_PLUGIN_FACTORY_WITH_JSON(DarklyDecoFactory, "darkly.json", registerPlugin<Darkly::Decoration>(); registerPlugin<Darkly::IconButton>();)

namespace
{
struct ShadowParams {
    ShadowParams()
        : offset(QPoint(0, 0))
        , radius(0)
        , opacity(0)
    {
    }

    ShadowParams(const QPoint &offset, int radius, qreal opacity)
        : offset(offset)
        , radius(radius)
        , opacity(opacity)
    {
    }

    QPoint offset;
    int radius;
    qreal opacity;
};

struct CompositeShadowParams {
    CompositeShadowParams() = default;

    CompositeShadowParams(const QPoint &offset, const ShadowParams &shadow1, const ShadowParams &shadow2)
        : offset(offset)
        , shadow1(shadow1)
        , shadow2(shadow2)
    {
    }

    bool isNone() const
    {
        return qMax(shadow1.radius, shadow2.radius) == 0;
    }

    QPoint offset;
    ShadowParams shadow1;
    ShadowParams shadow2;
};

const CompositeShadowParams s_shadowParams[] = {
    // None
    CompositeShadowParams(),
    // Small
    CompositeShadowParams(QPoint(0, 4), ShadowParams(QPoint(0, 0), 16, 1), ShadowParams(QPoint(0, -2), 8, 0.4)),
    // Medium
    CompositeShadowParams(QPoint(0, 8), ShadowParams(QPoint(0, 0), 32, 0.9), ShadowParams(QPoint(0, -4), 16, 0.3)),
    // Large
    CompositeShadowParams(QPoint(0, 12), ShadowParams(QPoint(0, 0), 48, 0.8), ShadowParams(QPoint(0, -6), 24, 0.2)),
    // Very large
    CompositeShadowParams(QPoint(0, 16), ShadowParams(QPoint(0, 0), 64, 0.7), ShadowParams(QPoint(0, -8), 32, 0.1)),
};

inline CompositeShadowParams lookupShadowParams(int size)
{
    switch (size) {
    case Darkly::InternalSettings::ShadowNone:
        return s_shadowParams[0];
    case Darkly::InternalSettings::ShadowSmall:
        return s_shadowParams[1];
    case Darkly::InternalSettings::ShadowMedium:
        return s_shadowParams[2];
    case Darkly::InternalSettings::ShadowLarge:
        return s_shadowParams[3];
    case Darkly::InternalSettings::ShadowVeryLarge:
        return s_shadowParams[4];
    default:
        // Fallback to the Large size.
        return s_shadowParams[3];
    }
}

inline qreal lookupOutlineIntensity(int intensity)
{
    switch (intensity) {
    case Darkly::InternalSettings::OutlineOff:
        return 0;
    case Darkly::InternalSettings::OutlineLow:
        return Darkly::Metrics::Bias_Default / 2;
    case Darkly::InternalSettings::OutlineMedium:
        return Darkly::Metrics::Bias_Default;
    case Darkly::InternalSettings::OutlineHigh:
        return Darkly::Metrics::Bias_Default * 2;
    case Darkly::InternalSettings::OutlineMaximum:
        return Darkly::Metrics::Bias_Default * 3;
    default:
        // Fallback to the Medium intensity.
        return Darkly::Metrics::Bias_Default;
    }
}
}

namespace Darkly
{
using KDecoration3::ColorGroup;
using KDecoration3::ColorRole;

//________________________________________________________________
static int g_sDecoCount = 0;
static int g_shadowSizeEnum = InternalSettings::ShadowLarge;
static int g_shadowStrength = 255;
static QColor g_shadowColor = Qt::black;
static std::shared_ptr<KDecoration3::DecorationShadow> g_sShadow;
static std::shared_ptr<KDecoration3::DecorationShadow> g_sShadowInactive;

//________________________________________________________________
Decoration::Decoration(QObject *parent, const QVariantList &args)
    : KDecoration3::Decoration(parent, args)
    , m_animation(new QVariantAnimation(this))
    , m_shadowAnimation(new QVariantAnimation(this))
{
    g_sDecoCount++;
}

//________________________________________________________________
Decoration::~Decoration()
{
    g_sDecoCount--;
    if (g_sDecoCount == 0) {
        // last deco destroyed, clean up shadow
        g_sShadow.reset();
    }
}

//________________________________________________________________
void Decoration::setOpacity(qreal value)
{
    if (m_opacity == value) {
        return;
    }
    m_opacity = value;
    update();
}

//________________________________________________________________
QColor Decoration::titleBarColor() const
{
    if (hideTitleBar()) {
        return window()->color(ColorGroup::Inactive, ColorRole::TitleBar);
    } else if (m_animation->state() == QAbstractAnimation::Running) {
        return KColorUtils::mix(window()->color(ColorGroup::Inactive, ColorRole::TitleBar),
                                window()->color(ColorGroup::Active, ColorRole::TitleBar),
                                m_opacity);
    } else {
        return window()->color(window()->isActive() ? ColorGroup::Active : ColorGroup::Inactive, ColorRole::TitleBar);
    }
}

//________________________________________________________________
QColor Decoration::fontColor() const
{
    if (m_animation->state() == QAbstractAnimation::Running) {
        return KColorUtils::mix(window()->color(ColorGroup::Inactive, ColorRole::Foreground),
                                window()->color(ColorGroup::Active, ColorRole::Foreground),
                                m_opacity);
    } else {
        return window()->color(window()->isActive() ? ColorGroup::Active : ColorGroup::Inactive, ColorRole::Foreground);
    }
}

//________________________________________________________________
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool Decoration::init()
#else
void Decoration::init()
#endif
{
    // active state change animation
    // It is important start and end value are of the same type, hence 0.0 and not just 0
    m_animation->setStartValue(0.0);
    m_animation->setEndValue(1.0);
    // Linear to have the same easing as Darkly animations
    m_animation->setEasingCurve(QEasingCurve::Linear);
    connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        setOpacity(value.toReal());
    });

    m_shadowAnimation->setStartValue(0.0);
    m_shadowAnimation->setEndValue(1.0);
    m_shadowAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_shadowAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        m_shadowOpacity = value.toReal();
        updateShadow();
    });

    // use DBus connection to update on darkly configuration change
    auto dbus = QDBusConnection::sessionBus();
    dbus.connect(QString(),
                 QStringLiteral("/KGlobalSettings"),
                 QStringLiteral("org.kde.KGlobalSettings"),
                 QStringLiteral("notifyChange"),
                 this,
                 SLOT(reconfigure()));

    dbus.connect(QStringLiteral("org.kde.KWin"),
                 QStringLiteral("/org/kde/KWin"),
                 QStringLiteral("org.kde.KWin.TabletModeManager"),
                 QStringLiteral("tabletModeChanged"),
                 QStringLiteral("b"),
                 this,
                 SLOT(onTabletModeChanged(bool)));

    auto message = QDBusMessage::createMethodCall(QStringLiteral("org.kde.KWin"),
                                                  QStringLiteral("/org/kde/KWin"),
                                                  QStringLiteral("org.freedesktop.DBus.Properties"),
                                                  QStringLiteral("Get"));
    message.setArguments({QStringLiteral("org.kde.KWin.TabletModeManager"), QStringLiteral("tabletMode")});
    auto call = new QDBusPendingCallWatcher(dbus.asyncCall(message), this);
    connect(call, &QDBusPendingCallWatcher::finished, this, [this, call]() {
        QDBusPendingReply<QVariant> reply = *call;
        if (!reply.isError()) {
            onTabletModeChanged(reply.value().toBool());
        }

        call->deleteLater();
    });

    reconfigure();
    updateTitleBar();
    auto s = settings();
    connect(s.get(), &KDecoration3::DecorationSettings::borderSizeChanged, this, &Decoration::recalculateBorders);

    // a change in font might cause the borders to change
    connect(s.get(), &KDecoration3::DecorationSettings::fontChanged, this, &Decoration::recalculateBorders);
    connect(s.get(), &KDecoration3::DecorationSettings::spacingChanged, this, &Decoration::recalculateBorders);

    // buttons
    connect(s.get(), &KDecoration3::DecorationSettings::spacingChanged, this, &Decoration::updateButtonsGeometryDelayed);
    connect(s.get(), &KDecoration3::DecorationSettings::decorationButtonsLeftChanged, this, &Decoration::updateButtonsGeometryDelayed);
    connect(s.get(), &KDecoration3::DecorationSettings::decorationButtonsRightChanged, this, &Decoration::updateButtonsGeometryDelayed);

    // full reconfiguration
    connect(s.get(), &KDecoration3::DecorationSettings::reconfigured, this, &Decoration::reconfigure);
    connect(s.get(), &KDecoration3::DecorationSettings::reconfigured, SettingsProvider::self(), &SettingsProvider::reconfigure, Qt::UniqueConnection);
    connect(s.get(), &KDecoration3::DecorationSettings::reconfigured, this, &Decoration::updateButtonsGeometryDelayed);

    connect(window(), &KDecoration3::DecoratedWindow::adjacentScreenEdgesChanged, this, &Decoration::recalculateBorders);
    connect(window(), &KDecoration3::DecoratedWindow::maximizedHorizontallyChanged, this, &Decoration::recalculateBorders);
    connect(window(), &KDecoration3::DecoratedWindow::maximizedVerticallyChanged, this, &Decoration::recalculateBorders);
    connect(window(), &KDecoration3::DecoratedWindow::shadedChanged, this, &Decoration::recalculateBorders);
    connect(window(), &KDecoration3::DecoratedWindow::captionChanged, this, [this]() {
        // update the caption area
        update(titleBar());
    });

    connect(window(), &KDecoration3::DecoratedWindow::activeChanged, this, &Decoration::updateAnimationState);
    connect(this, &KDecoration3::Decoration::bordersChanged, this, &Decoration::updateTitleBar);
    connect(window(), &KDecoration3::DecoratedWindow::adjacentScreenEdgesChanged, this, &Decoration::updateTitleBar);
    connect(window(), &KDecoration3::DecoratedWindow::widthChanged, this, &Decoration::updateTitleBar);
    connect(window(), &KDecoration3::DecoratedWindow::maximizedChanged, this, &Decoration::updateTitleBar);
    connect(window(), &KDecoration3::DecoratedWindow::maximizedChanged, this, &Decoration::setOpaque);

    connect(window(), &KDecoration3::DecoratedWindow::widthChanged, this, &Decoration::updateButtonsGeometry);
    connect(window(), &KDecoration3::DecoratedWindow::maximizedChanged, this, &Decoration::updateButtonsGeometry);
    connect(window(), &KDecoration3::DecoratedWindow::adjacentScreenEdgesChanged, this, &Decoration::updateButtonsGeometry);
    connect(window(), &KDecoration3::DecoratedWindow::shadedChanged, this, &Decoration::updateButtonsGeometry);

    connect(window(), &KDecoration3::DecoratedWindow::nextScaleChanged, this, &Decoration::updateScale);

    createButtons();
    updateShadow();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return true;
#endif
}

//________________________________________________________________
void Decoration::updateTitleBar()
{
    // The titlebar rect has margins around it so the window can be resized by dragging a decoration edge.
    auto s = settings();
    const bool maximized = isMaximized();
    const qreal width = maximized ? window()->width() : window()->width() - 2 * s->smallSpacing() * Metrics::TitleBar_SideMargin;
    const qreal height = (maximized || isTopEdge()) ? borderTop() : borderTop() - s->smallSpacing() * Metrics::TitleBar_TopMargin;
    const qreal x = maximized ? 0 : s->smallSpacing() * Metrics::TitleBar_SideMargin;
    const qreal y = (maximized || isTopEdge()) ? 0 : s->smallSpacing() * Metrics::TitleBar_TopMargin;
    setTitleBar(QRectF(x, y, width, height));
}

//________________________________________________________________
void Decoration::updateAnimationState()
{
    if (m_shadowAnimation->duration() > 0) {
        m_shadowAnimation->setDirection(window()->isActive() ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
        m_shadowAnimation->setEasingCurve(window()->isActive() ? QEasingCurve::OutCubic : QEasingCurve::InCubic);
        if (m_shadowAnimation->state() != QAbstractAnimation::Running) {
            m_shadowAnimation->start();
        }

    } else {
        updateShadow();
    }

    if (m_animation->duration() > 0) {
        m_animation->setDirection(window()->isActive() ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
        if (m_animation->state() != QAbstractAnimation::Running) {
            m_animation->start();
        }

    } else {
        update();
    }
}

//________________________________________________________________
qreal Decoration::borderSize(bool bottom, qreal scale) const
{
    const qreal pixelSize = KDecoration3::pixelSize(scale);
    const qreal baseSize = std::max<qreal>(pixelSize, KDecoration3::snapToPixelGrid(settings()->smallSpacing(), scale));
    if (m_internalSettings && (m_internalSettings->mask() & BorderSize)) {
        switch (m_internalSettings->borderSize()) {
        case InternalSettings::BorderNone:
            return outlinesEnabled() ? std::max<qreal>(pixelSize, KDecoration3::snapToPixelGrid(1, scale)) : 0;
        case InternalSettings::BorderNoSides:
            if (bottom) {
                return KDecoration3::snapToPixelGrid(std::max(4.0, baseSize), scale);
            } else {
                return outlinesEnabled() ? std::max<qreal>(pixelSize, KDecoration3::snapToPixelGrid(1, scale)) : 0;
            }
        default:
        case InternalSettings::BorderTiny:
            if (bottom) {
                return KDecoration3::snapToPixelGrid(std::max(4.0, baseSize), scale);
            } else {
                return baseSize;
            }
        case InternalSettings::BorderNormal:
            return baseSize * 2;
        case InternalSettings::BorderLarge:
            return baseSize * 3;
        case InternalSettings::BorderVeryLarge:
            return baseSize * 4;
        case InternalSettings::BorderHuge:
            return baseSize * 5;
        case InternalSettings::BorderVeryHuge:
            return baseSize * 6;
        case InternalSettings::BorderOversized:
            return baseSize * 10;
        }
    } else {
        switch (settings()->borderSize()) {
        case KDecoration3::BorderSize::None:
            return outlinesEnabled() ? std::max<qreal>(pixelSize, KDecoration3::snapToPixelGrid(1, scale)) : 0;
        case KDecoration3::BorderSize::NoSides:
            if (bottom) {
                return KDecoration3::snapToPixelGrid(std::max(4.0, baseSize), scale);
            } else {
                return outlinesEnabled() ? std::max<qreal>(pixelSize, KDecoration3::snapToPixelGrid(1, scale)) : 0;
            }
        default:
        case KDecoration3::BorderSize::Tiny:
            if (bottom) {
                return KDecoration3::snapToPixelGrid(std::max(4.0, baseSize), scale);
            } else {
                return baseSize;
            }
        case KDecoration3::BorderSize::Normal:
            return baseSize * 2;
        case KDecoration3::BorderSize::Large:
            return baseSize * 3;
        case KDecoration3::BorderSize::VeryLarge:
            return baseSize * 4;
        case KDecoration3::BorderSize::Huge:
            return baseSize * 5;
        case KDecoration3::BorderSize::VeryHuge:
            return baseSize * 6;
        case KDecoration3::BorderSize::Oversized:
            return baseSize * 10;
        }
    }
}

//________________________________________________________________
void Decoration::reconfigure()
{
    m_internalSettings = SettingsProvider::self()->internalSettings(this);

    setScaledCornerRadius();

    // animation

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    const KConfigGroup cg(config, QStringLiteral("KDE"));

    m_animation->setDuration(0);
    // Syncing anis between client and decoration is troublesome, so we're not using
    // any animations right now.
    // m_animation->setDuration( cg.readEntry("AnimationDurationFactor", 1.0f) * 100.0f );

    // But the shadow is fine to animate like this!
    m_shadowAnimation->setDuration(cg.readEntry("AnimationDurationFactor", 1.0f) * 100.0f);

    // borders
    recalculateBorders();

    // shadow
    updateShadow();
}

QMarginsF Decoration::bordersFor(qreal scale) const
{
    const qreal left = isLeftEdge() ? 0 : borderSize(false, scale);
    const qreal right = isRightEdge() ? 0 : borderSize(false, scale);
    const qreal bottom = (window()->isShaded() || isBottomEdge()) ? 0 : borderSize(true, scale);

    qreal top = 0;
    if (hideTitleBar()) {
        top = bottom;
    } else {
        QFontMetrics fm(settings()->font());
        top += KDecoration3::snapToPixelGrid(std::max(fm.height(), buttonSize()), scale);

        // padding below
        const int baseSize = settings()->smallSpacing();
        top += KDecoration3::snapToPixelGrid(baseSize * Metrics::TitleBar_BottomMargin, scale);

        // padding above
        top += KDecoration3::snapToPixelGrid(baseSize * Metrics::TitleBar_TopMargin, scale);
    }
    return QMarginsF(left, top, right, bottom);
}

void Decoration::recalculateBorders()
{
    setBorders(bordersFor(window()->nextScale()));

    // extended sizes
    const qreal extSize = KDecoration3::snapToPixelGrid(settings()->largeSpacing(), window()->nextScale());
    qreal extSides = 0;
    qreal extBottom = 0;
    if (hasNoBorders()) {
        if (!isMaximizedHorizontally()) {
            extSides = extSize;
        }
        if (!isMaximizedVertically()) {
            extBottom = extSize;
        }

    } else if (hasNoSideBorders() && !isMaximizedHorizontally()) {
        extSides = extSize;
    }

    setResizeOnlyBorders(QMarginsF(extSides, 0, extSides, extBottom));
}

//________________________________________________________________
void Decoration::createButtons()
{
    auto createButton = [this](KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *object) -> Button * {
        Button *button;
        switch (type) {
        case KDecoration3::DecorationButtonType::LocallyIntegratedMenu:
            button = new ApplicationMenuButton(decoration);
            break;
        default:
            button = IconButton::create(type, decoration, object);
            break;
        }
        connect(button, &Button::preferredSizeChanged, this, &Decoration::updateButtonsGeometry);
        return button;
    };
    m_leftButtons = new KDecoration3::DecorationButtonGroup(KDecoration3::DecorationButtonGroup::Position::Left, this, createButton);
    m_rightButtons = new KDecoration3::DecorationButtonGroup(KDecoration3::DecorationButtonGroup::Position::Right, this, createButton);
    updateButtonsGeometry();
}

//________________________________________________________________
void Decoration::updateButtonsGeometryDelayed()
{
    QTimer::singleShot(0, this, &Decoration::updateButtonsGeometry);
}

//________________________________________________________________
void Decoration::updateButtonsGeometry()
{
    const auto s = settings();

    // adjust button position
    const auto buttonList = m_leftButtons->buttons() + m_rightButtons->buttons();
    for (KDecoration3::DecorationButton *button : buttonList) {
        auto btn = static_cast<Button *>(button);

        const int verticalOffset = (isTopEdge() ? s->smallSpacing() * Metrics::TitleBar_TopMargin : 0);

        const QSizeF preferredSize = btn->preferredSize();
        const int bHeight = preferredSize.height() + verticalOffset;
        const int bWidth = preferredSize.width();

        btn->setGeometry(QRectF(QPoint(0, 0), QSizeF(bWidth, bHeight + 10)));
        btn->setPadding(QMargins(0, verticalOffset, 0, 0));
    }

    // left buttons
    if (!m_leftButtons->buttons().isEmpty()) {
        // spacing
        m_leftButtons->setSpacing(s->smallSpacing() * Metrics::TitleBar_ButtonSpacing);

        // padding
        const int vPadding = 0;
        const int hPadding = s->smallSpacing() * Metrics::TitleBar_SideMargin;
        if (isLeftEdge()) {
            // add offsets on the side buttons, to preserve padding, but satisfy Fitts law
            auto button = static_cast<Button *>(m_leftButtons->buttons().front());

            QRectF geometry = button->geometry();
            geometry.adjust(-hPadding, -10, 0, -10);
            button->setGeometry(geometry);
            button->setLeftPadding(hPadding);

            m_leftButtons->setPos(QPointF(0, vPadding));

        } else {

            m_leftButtons->setPos(QPointF(hPadding + borderLeft(), 0));
        }
    }

    // right buttons
    if (!m_rightButtons->buttons().isEmpty()) {
        // spacing
        m_rightButtons->setSpacing(s->smallSpacing() * Metrics::TitleBar_ButtonSpacing);

        // padding
        const int vPadding = isTopEdge() ? 0 : s->smallSpacing() * Metrics::TitleBar_TopMargin;
        const int hPadding = s->smallSpacing() * Metrics::TitleBar_SideMargin;
        if (isRightEdge()) {
            auto button = static_cast<Button *>(m_rightButtons->buttons().back());

            QRectF geometry = button->geometry();
            geometry.adjust(0, 0, hPadding, 0);
            button->setGeometry(geometry);
            button->setRightPadding(hPadding);

            m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width(), vPadding));

        } else {
            m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width() - hPadding - borderRight(), vPadding));
        }
    }

    update();
}

//________________________________________________________________
void Decoration::paint(QPainter *painter, const QRectF &repaintRegion)
{
    // TODO: optimize based on repaintRegion
    auto s = settings();
    // paint background
    if (!window()->isShaded()) {
        painter->fillRect(rect(), Qt::transparent);
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(window()->color(window()->isActive() ? ColorGroup::Active : ColorGroup::Inactive, ColorRole::Frame));

        // clip away the top part
        if (!hideTitleBar()) {
            painter->setClipRect(QRectF(0, borderTop(), size().width(), size().height() - borderTop()), Qt::IntersectClip);
        }

        if (s->isAlphaChannelSupported()) {
            if (hasNoBorders()) {
                painter->drawRoundedRect(rect(), 0, 0);
            } else {
                painter->drawRoundedRect(rect(), m_scaledCornerRadius, m_scaledCornerRadius);
            }
        } else {
            painter->drawRect(rect());
        }

        painter->restore();
    }

    if (!hideTitleBar()) {
        paintTitleBar(painter, repaintRegion);
    }

    if (hasBorders() && !s->isAlphaChannelSupported()) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(window()->isActive() ? window()->color(ColorGroup::Active, ColorRole::TitleBar)
                                             : window()->color(ColorGroup::Inactive, ColorRole::Foreground));

        painter->drawRect(rect().adjusted(0, 0, -1, -1));
        painter->restore();
    }
    if (outlinesEnabled() && !isMaximized()) {
        auto outlineColor = KColorUtils::mix(window()->color(window()->isActive() ? ColorGroup::Active : ColorGroup::Inactive, ColorRole::Frame),
                                             window()->palette().text().color(),
                                             lookupOutlineIntensity(m_internalSettings->outlineIntensity()));

        const qreal lineWidth = std::max(KDecoration3::pixelSize(window()->scale()), KDecoration3::snapToPixelGrid(1, window()->scale()));
        QRectF outlineRect = rect();
        // this is necessary to paint exactly in the center of the intended line
        // otherwise anti aliasing kicks in and ruins the day
        outlineRect.adjust(lineWidth / 2, lineWidth / 2, -lineWidth / 2, -lineWidth / 2);

        qreal cornerSize = m_scaledCornerRadius * 2.0;
        QRectF cornerRect(outlineRect.x(), outlineRect.y(), cornerSize, cornerSize);

        QPainterPath outlinePath;
        outlinePath.arcMoveTo(cornerRect, 180);
        outlinePath.arcTo(cornerRect, 180, -90);
        cornerRect.moveTopRight(outlineRect.topRight());
        outlinePath.arcTo(cornerRect, 90, -90);
        // Check if border size is "no borders" or "no side-borders"
        if (hasNoBorders()) {
            outlinePath.lineTo(outlineRect.bottomRight());
            outlinePath.lineTo(outlineRect.bottomLeft());
        } else {
            cornerRect.moveBottomRight(outlineRect.bottomRight());
            outlinePath.arcTo(cornerRect, 0, -90);
            cornerRect.moveBottomLeft(outlineRect.bottomLeft());
            outlinePath.arcTo(cornerRect, 270, -90);
        }
        outlinePath.closeSubpath();

        painter->save();
        painter->setPen(QPen(outlineColor, lineWidth));
        painter->setBrush(Qt::NoBrush);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawPath(outlinePath.simplified());
        painter->restore();
    }
}

//________________________________________________________________
void Decoration::paintTitleBar(QPainter *painter, const QRectF &repaintRegion)
{
    const qreal halfAPixel = 0.5 * KDecoration3::pixelSize(window()->scale());
    QRectF rect(QPointF(0, 0), QSizeF(size().width(), borderTop()));
    const bool noOutlines = isMaximized() || !settings()->isAlphaChannelSupported();
    const bool bottomOutline = window()->isShaded();

    if (!noOutlines && outlinesEnabled()) {
        // this is needed to match the corner radius with the outline
        rect.adjust(halfAPixel, halfAPixel, -halfAPixel, bottomOutline ? -halfAPixel : 0);
    }

    //rect.adjust(0, KDecoration3::pixelSize(window()->scale()), 0, 0);

    QBrush frontBrush;
    QBrush backBrush(this->titleBarColor());

    if (!rect.intersects(repaintRegion)) {
        return;
    }

    painter->save();
    painter->setPen(Qt::NoPen);

    // render a linear gradient on title area
    if (window()->isActive() && m_internalSettings->drawBackgroundGradient()) {
        QLinearGradient gradient(0, 0, 0, rect.height());
        gradient.setColorAt(0.0, titleBarColor().lighter(120));
        gradient.setColorAt(0.8, titleBarColor());

        frontBrush = gradient;

    } else {
        frontBrush = titleBarColor();

        painter->setBrush(titleBarColor());
    }


    if (noOutlines) {
        painter->setBrush(backBrush);
        painter->drawRect(rect);

        painter->setBrush(frontBrush);
        painter->drawRect(rect);
    } else if (bottomOutline) {
        painter->setBrush(backBrush);
        painter->drawRoundedRect(rect, m_scaledCornerRadius, m_scaledCornerRadius);

        painter->setBrush(frontBrush);
        painter->drawRoundedRect(rect, m_scaledCornerRadius, m_scaledCornerRadius);

    } else {
        painter->setClipRect(rect, Qt::IntersectClip);

        auto drawThe = [this, painter](const QRectF &r) {
            painter->drawRoundedRect(r, m_scaledCornerRadius, m_scaledCornerRadius);
            // remove the rounding on the bottom
            painter->drawRect(QRectF(r.bottomLeft() - QPointF(0, m_scaledCornerRadius), r.bottomRight()));
        };

        painter->setBrush(backBrush);
        drawThe(rect);

        painter->setBrush(frontBrush);
        drawThe(rect);
    }


    // top highlight
    if (qGray(this->titleBarColor().rgb()) < 130) {
        if (isMaximized()) {
            painter->setPen(QColor(255, 255, 255, 30));
            painter->drawLine(QRect(QPoint(0, 0), QSize(size().width(), borderTop())).topLeft(), QRect(QPoint(0, 0), QSize(size().width(), borderTop())).topRight());

        } else if (!window()->isShaded()) {
            QRect copy(QRect(QPoint(0, 0), QSize(size().width(), borderTop())).adjusted(isLeftEdge() ? -m_scaledCornerRadius : 0,
                                                                                        isTopEdge() ? -m_scaledCornerRadius : 0,
                                                                                        isRightEdge() ? m_scaledCornerRadius : 0,
                                                                                        m_scaledCornerRadius));

            QPixmap pix = QPixmap(copy.width(), copy.height());
            pix.fill(Qt::transparent);

            QPainter p(&pix);
            p.setRenderHint(QPainter::Antialiasing);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(255, 255, 255, 30));
            p.drawRoundedRect(copy, m_scaledCornerRadius, m_scaledCornerRadius);

            p.setBrush(Qt::black);
            p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
            p.drawRoundedRect(copy.adjusted(0, 1, 0, 0), m_scaledCornerRadius, m_scaledCornerRadius);

            painter->drawPixmap(copy, pix);
        }
    }

    painter->restore();

    // draw caption
    painter->setFont(settings()->font());
    painter->setPen(fontColor());
    const auto [captionRectangle, alignment] = captionRect();
    const QString caption = painter->fontMetrics().elidedText(window()->caption(), Qt::ElideMiddle, captionRectangle.width());
    painter->drawText(captionRectangle, alignment | Qt::TextSingleLine, caption);

    // draw all buttons
    m_leftButtons->paint(painter, repaintRegion);
    m_rightButtons->paint(painter, repaintRegion);
}

//________________________________________________________________
int Decoration::buttonSize() const
{
    const int baseSize = m_tabletMode ? settings()->gridUnit() * 2 : settings()->gridUnit();
    switch (m_internalSettings->buttonSize()) {
    case InternalSettings::ButtonTiny:
        return baseSize;
    case InternalSettings::ButtonSmall:
        return baseSize * 1.5;
    default:
    case InternalSettings::ButtonDefault:
        return baseSize * 2;
    case InternalSettings::ButtonLarge:
        return baseSize * 2.5;
    case InternalSettings::ButtonVeryLarge:
        return baseSize * 3.5;
    }
}

void Decoration::onTabletModeChanged(bool mode)
{
    m_tabletMode = mode;
    Q_EMIT tabletModeChanged();

    recalculateBorders();
    updateButtonsGeometry();
}

//________________________________________________________________
qreal Decoration::captionHeight() const
{
    return hideTitleBar() ? borderTop() : borderTop() - settings()->smallSpacing() * (Metrics::TitleBar_BottomMargin + Metrics::TitleBar_TopMargin);
}

//________________________________________________________________
QPair<QRectF, Qt::Alignment> Decoration::captionRect() const
{
    if (hideTitleBar()) {
        return qMakePair(QRectF(), Qt::AlignCenter);
    } else {
        const qreal leftOffset =
            KDecoration3::snapToPixelGrid(m_leftButtons->buttons().isEmpty() ? Metrics::TitleBar_SideMargin * settings()->smallSpacing()
                                                                             : m_leftButtons->geometry().x() + m_leftButtons->geometry().width()
                                                  + Metrics::TitleBar_SideMargin * settings()->smallSpacing(),
                                          window()->scale());

        const qreal rightOffset = KDecoration3::snapToPixelGrid(m_rightButtons->buttons().isEmpty() ? Metrics::TitleBar_SideMargin * settings()->smallSpacing()
                                                                                                    : size().width() - m_rightButtons->geometry().x()
                                                                        + Metrics::TitleBar_SideMargin * settings()->smallSpacing(),
                                                                window()->scale());

        const qreal yOffset = KDecoration3::snapToPixelGrid(settings()->smallSpacing() * Metrics::TitleBar_TopMargin, window()->scale());
        const QRectF maxRect(leftOffset, yOffset, size().width() - leftOffset - rightOffset, captionHeight());

        switch (m_internalSettings->titleAlignment()) {
        case InternalSettings::AlignLeft:
            return qMakePair(maxRect, Qt::AlignVCenter | Qt::AlignLeft);

        case InternalSettings::AlignRight:
            return qMakePair(maxRect, Qt::AlignVCenter | Qt::AlignRight);

        case InternalSettings::AlignCenter:
            return qMakePair(maxRect, Qt::AlignCenter);

        default:
        case InternalSettings::AlignCenterFullWidth: {
            // full caption rect
            const QRectF fullRect = QRect(0, yOffset, size().width(), captionHeight());
            QRectF boundingRect(settings()->fontMetrics().boundingRect(window()->caption()));

            // text bounding rect
            boundingRect.setTop(yOffset);
            boundingRect.setHeight(captionHeight());
            boundingRect.moveLeft((size().width() - boundingRect.width()) / 2);

            if (boundingRect.left() < leftOffset) {
                return qMakePair(maxRect, Qt::AlignVCenter | Qt::AlignLeft);
            } else if (boundingRect.right() > size().width() - rightOffset) {
                return qMakePair(maxRect, Qt::AlignVCenter | Qt::AlignRight);
            } else {
                return qMakePair(fullRect, Qt::AlignCenter);
            }
        }
        }
    }
}

//________________________________________________________________
void Decoration::updateShadow()
{
    auto s = settings();

    // Animated case, no cached shadow object
    if ((m_shadowAnimation->state() == QAbstractAnimation::Running) && (m_shadowOpacity != 0.0) && (m_shadowOpacity != 1.0)) {
        setShadow(createShadowObject(0.5 + m_shadowOpacity * 0.5));
        return;
    }

    if (g_shadowSizeEnum != m_internalSettings->shadowSize() || g_shadowStrength != m_internalSettings->shadowStrength()
        || g_shadowColor != m_internalSettings->shadowColor()) {
        g_sShadow.reset();
        g_sShadowInactive.reset();
        g_shadowSizeEnum = m_internalSettings->shadowSize();
        g_shadowStrength = m_internalSettings->shadowStrength();
        g_shadowColor = m_internalSettings->shadowColor();
    }

    auto &shadow = (window()->isActive()) ? g_sShadow : g_sShadowInactive;
    if (!shadow) {
        // Update both active and inactive shadows so outline stays consistent between the two
        g_sShadow = createShadowObject(1.0);
        g_sShadowInactive = createShadowObject(0.5);
    }
    setShadow(shadow);
}

//________________________________________________________________
std::shared_ptr<KDecoration3::DecorationShadow> Decoration::createShadowObject(const float strengthScale)
{
    CompositeShadowParams params = lookupShadowParams(m_internalSettings->shadowSize());
    if (params.isNone()) {
        // If shadows are disabled, return nothing
        return nullptr;
    }

    auto withOpacity = [](const QColor &color, qreal opacity) -> QColor {
        QColor c(color);
        c.setAlphaF(opacity);
        return c;
    };

    const QSize boxSize =
        BoxShadowRenderer::calculateMinimumBoxSize(params.shadow1.radius).expandedTo(BoxShadowRenderer::calculateMinimumBoxSize(params.shadow2.radius));

    BoxShadowRenderer shadowRenderer;
    shadowRenderer.setBorderRadius(m_scaledCornerRadius + 0.5);
    shadowRenderer.setBoxSize(boxSize);

    const qreal strength = m_internalSettings->shadowStrength() / 255.0 * strengthScale;
    shadowRenderer.addShadow(params.shadow1.offset, params.shadow1.radius, withOpacity(m_internalSettings->shadowColor(), params.shadow1.opacity * strength));
    shadowRenderer.addShadow(params.shadow2.offset, params.shadow2.radius, withOpacity(m_internalSettings->shadowColor(), params.shadow2.opacity * strength));

    QImage shadowTexture = shadowRenderer.render();

    QPainter painter(&shadowTexture);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRectF outerRect = shadowTexture.rect();

    QRectF boxRect(QPoint(0, 0), boxSize);
    boxRect.moveCenter(outerRect.center());

    // Mask out inner rect.
    const QMarginsF padding = QMarginsF(boxRect.left() - outerRect.left() - Metrics::Shadow_Overlap - params.offset.x(),
                                        boxRect.top() - outerRect.top() - Metrics::Shadow_Overlap - params.offset.y(),
                                        outerRect.right() - boxRect.right() - Metrics::Shadow_Overlap + params.offset.x(),
                                        outerRect.bottom() - boxRect.bottom() - Metrics::Shadow_Overlap + params.offset.y());
    QRectF innerRect = outerRect - padding;
    // Push the shadow slightly under the window, which helps avoiding glitches with fractional scaling
    // TODO fix this more properly
    innerRect.adjust(2, 2, -2, -2);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.drawRoundedRect(innerRect, m_scaledCornerRadius + 0.5, m_scaledCornerRadius + 0.5);

    painter.end();

    auto ret = std::make_shared<KDecoration3::DecorationShadow>();
    ret->setPadding(padding);
    ret->setInnerShadowRect(QRectF(outerRect.center(), QSizeF(1, 1)));
    ret->setShadow(shadowTexture);
    return ret;
}

void Decoration::setScaledCornerRadius()
{
    // On X11, the smallSpacing value is used for scaling.
    // On Wayland, this value has constant factor of 2.
    // Removing it will break radius scaling on X11.
    m_scaledCornerRadius = KDecoration3::snapToPixelGrid(Metrics::Frame_FrameRadius * settings()->smallSpacing(), window()->nextScale());
}

void Decoration::updateScale()
{
    setScaledCornerRadius();
    recalculateBorders();
}
} // namespace

#include "darklydecoration.moc"

/*
 * SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "darklyapplicationmenu.h"
#include "darklydecoration.h"

#include "libdbusmenuqt/dbusmenuimporter.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDebug>
#include <QGuiApplication>
#include <QHoverEvent>
#include <QMenu>
#include <QPainter>

namespace Darkly
{

static int s_refs = 0;

ApplicationMenu::ApplicationMenu(QWidget *parent)
    : QMenu(parent)
{
}

void ApplicationMenu::keyPressEvent(QKeyEvent *event)
{
    QMenu::keyPressEvent(event);

    if (!event->isAccepted()) {
        switch (event->key()) {
        case Qt::Key_Left:
            Q_EMIT hitLeft();
            break;
        case Qt::Key_Right:
            Q_EMIT hitRight();
            break;
        }
    }
}

ApplicationMenuImporter::ApplicationMenuImporter(const QString &service, const QString &path, QObject *parent)
    : DBusMenuImporter(service, path, parent)
{
}

QMenu *ApplicationMenuImporter::createMenu(QWidget *parent)
{
    return new ApplicationMenu(parent);
}

ApplicationMenuButton::ApplicationMenuButton(KDecoration3::Decoration *decoration)
    : Button(KDecoration3::DecorationButtonType::LocallyIntegratedMenu, decoration)
{
    if (++s_refs == 1) {
        QDBusConnection::sessionBus().interface()->registerService(QStringLiteral("org.kde.kappmenuview"),
                                                                   QDBusConnectionInterface::QueueService,
                                                                   QDBusConnectionInterface::DontAllowReplacement);
    }

    const auto window = decoration->window();
    track(window->applicationMenuServiceName(), window->applicationMenuObjectPath());
    connect(decoration->window(), &KDecoration3::DecoratedWindow::applicationMenuChanged, this, [this, window]() {
        track(window->applicationMenuServiceName(), window->applicationMenuObjectPath());
    });

    connect(decoration, &KDecoration3::Decoration::bordersChanged, this, &ApplicationMenuButton::rebuild);
    connect(decoration->settings().get(), &KDecoration3::DecorationSettings::reconfigured, this, &ApplicationMenuButton::rebuild);
}

ApplicationMenuButton::~ApplicationMenuButton()
{
    if (--s_refs == 0) {
        QDBusConnection::sessionBus().interface()->unregisterService(QStringLiteral("org.kde.kappmenuview"));
    }
}

void ApplicationMenuButton::paint(QPainter *painter, const QRectF &repaintRegion)
{
    auto decoration = qobject_cast<Decoration *>(this->decoration());
    if (!decoration) {
        return;
    }

    const QRectF rect = geometry().marginsRemoved(QMargins(0, -10, 0, 0));

    painter->save();
    painter->translate(rect.topLeft());
    painter->translate(0, 14);

    for (const ApplicationMenuEntry &entry : std::as_const(m_entries)) {
        if (entry.active || entry.hovered) {
            painter->setBrush(decoration->fontColor());
            painter->drawRoundedRect(entry.rect.adjusted(2, -2, -2, 2), 5, 5);
            painter->setBrush(Qt::NoBrush);
            painter->setPen(decoration->titleBarColor());
        } else {
            painter->setPen(decoration->fontColor());
        }

        painter->drawText(entry.rect, Qt::AlignCenter | Qt::TextSingleLine, entry.text);
    }

    painter->restore();
}

void ApplicationMenuButton::hoverEnterEvent(QHoverEvent *event)
{
    Button::hoverEnterEvent(event);

    QAction *hovered = actionAt(event->position() - geometry().topLeft());
    if (m_hoveredAction != hovered) {
        m_hoveredAction = hovered;

        if (m_activeAction && m_hoveredAction && m_activeAction != m_hoveredAction) {
            showMenu(m_hoveredAction);
        }

        rebuild();
    }
}

void ApplicationMenuButton::hoverLeaveEvent(QHoverEvent *event)
{
    Button::hoverLeaveEvent(event);

    if (m_hoveredAction) {
        m_hoveredAction = nullptr;
        rebuild();
    }
}

void ApplicationMenuButton::hoverMoveEvent(QHoverEvent *event)
{
    Button::hoverMoveEvent(event);

    QAction *hovered = actionAt(event->position() - geometry().topLeft());
    if (m_hoveredAction != hovered) {
        m_hoveredAction = hovered;

        if (m_activeAction && m_hoveredAction && m_activeAction != m_hoveredAction) {
            showMenu(m_hoveredAction);
        }

        rebuild();
    }
}

void ApplicationMenuButton::mousePressEvent(QMouseEvent *event)
{
    Button::mousePressEvent(event);

    if (m_activeAction) {
        rebuild();
    }
}

void ApplicationMenuButton::mouseReleaseEvent(QMouseEvent *event)
{
    Button::mouseReleaseEvent(event);

    QAction *releasedAction = actionAt(event->position() - geometry().topLeft());
    if (releasedAction) {
        triggerAction(releasedAction);
    }
}

QAction *ApplicationMenuButton::actionAt(const QPointF &point) const
{
    for (int i = 0; i < m_entries.size(); ++i) {
        const QRectF rect = m_entries[i].rect;
        if (rect.left() <= point.x() && point.x() < rect.right() && rect.top() <= point.y() && point.y() < rect.bottom()) {
            return m_entries[i].action;
        }
    }

    return nullptr;
}

int ApplicationMenuButton::findAction(QAction *action) const
{
    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].action == action) {
            return i;
        }
    }
    return -1;
}

QAction *ApplicationMenuButton::previousAction(QAction *reference) const
{
    if (int index = findAction(reference); index != -1) {
        if (QGuiApplication::layoutDirection() == Qt::RightToLeft) {
            if (index + 1 < m_entries.size()) {
                return m_entries[index + 1].action;
            }
        } else {
            if (index > 0) {
                return m_entries[index - 1].action;
            }
        }
    }
    return nullptr;
}

QAction *ApplicationMenuButton::nextAction(QAction *reference) const
{
    if (int index = findAction(reference); index != -1) {
        if (QGuiApplication::layoutDirection() == Qt::RightToLeft) {
            if (index > 0) {
                return m_entries[index - 1].action;
            }
        } else {
            if (index + 1 < m_entries.size()) {
                return m_entries[index + 1].action;
            }
        }
    }
    return nullptr;
}

void ApplicationMenuButton::triggerAction(QAction *action)
{
    if (action->menu()) {
        showMenu(action);
    } else {
        action->trigger();
    }

    rebuild();
}

QRectF ApplicationMenuButton::mapToDecoration(const QRectF &rect) const
{
    return rect.translated(geometry().topLeft());
}

void ApplicationMenuButton::showMenu(QAction *action)
{
    if (m_activeAction) {
        if (auto menu = qobject_cast<ApplicationMenu *>(m_activeAction->menu())) {
            disconnect(menu, &QMenu::aboutToHide, this, &ApplicationMenuButton::onMenuHidden);
            disconnect(menu, &ApplicationMenu::hitLeft, this, &ApplicationMenuButton::showPreviousMenu);
            disconnect(menu, &ApplicationMenu::hitRight, this, &ApplicationMenuButton::showNextMenu);
            menu->hide();
        }
        m_activeAction = nullptr;
    }

    if (auto menu = qobject_cast<ApplicationMenu *>(action->menu())) {
        for (const auto &entry : std::as_const(m_entries)) {
            if (entry.action == action) {
                connect(menu, &ApplicationMenu::aboutToHide, this, &ApplicationMenuButton::onMenuHidden);
                connect(menu, &ApplicationMenu::hitLeft, this, &ApplicationMenuButton::showPreviousMenu);
                connect(menu, &ApplicationMenu::hitRight, this, &ApplicationMenuButton::showNextMenu);

                m_activeAction = action;

                KDecoration3::Positioner positioner;
                positioner.setAnchorRect(mapToDecoration(entry.rect.adjusted(1, 0, 0, 7)));
                decoration()->popup(positioner, menu);
            }
        }
    }
}

void ApplicationMenuButton::showPreviousMenu()
{
    if (QAction *action = previousAction(m_activeAction)) {
        showMenu(action);
    }
}

void ApplicationMenuButton::showNextMenu()
{
    if (QAction *action = nextAction(m_activeAction)) {
        showMenu(action);
    }
}

void ApplicationMenuButton::onMenuHidden()
{
    if (auto menu = qobject_cast<ApplicationMenu *>(m_activeAction->menu())) {
        disconnect(menu, &QMenu::aboutToHide, this, &ApplicationMenuButton::onMenuHidden);
        disconnect(menu, &ApplicationMenu::hitLeft, this, &ApplicationMenuButton::showPreviousMenu);
        disconnect(menu, &ApplicationMenu::hitRight, this, &ApplicationMenuButton::showNextMenu);
    }

    m_activeAction = nullptr;
    rebuild();
}

void ApplicationMenuButton::track(const QString &serviceName, const QString &objectPath)
{
    if (serviceName.isEmpty() || objectPath.isEmpty()) {
        m_menuImporter.reset();
        setEnabled(false);
    } else {
        m_menuImporter = std::make_unique<ApplicationMenuImporter>(serviceName, objectPath);
        connect(m_menuImporter.get(), &ApplicationMenuImporter::menuUpdated, this, &ApplicationMenuButton::rebuild);
        connect(m_menuImporter.get(), &ApplicationMenuImporter::actionActivationRequested, this, &ApplicationMenuButton::triggerAction);
        setEnabled(true);
    }

    rebuild();
}

static QString stripMnemonics(const QString &text)
{
    QString stripped = text;
    stripped.remove(QLatin1Char('&'));
    return stripped;
}

QList<ApplicationMenuEntry> ApplicationMenuButton::build() const
{
    QList<ApplicationMenuEntry> entries;
    if (!m_menuImporter) {
        return entries;
    }

    QMenu *rootMenu = m_menuImporter->menu();
    if (!rootMenu) {
        return entries;
    }

    auto decoration = qobject_cast<Decoration *>(this->decoration());
    if (!decoration) {
        return entries;
    }

    const auto settings = decoration->settings();
    const QFontMetricsF fontMetrics(settings->font());
    qreal lastPosition = 0;

    const QList<QAction *> actions = rootMenu->actions();
    for (QAction *action : actions) {
        const QString text = stripMnemonics(action->text());
        const qreal width = fontMetrics.boundingRect(text).width() + 2 * settings->gridUnit();
        const qreal height = decoration->captionHeight();
        const QRectF bounds = QRectF(lastPosition, 0, width, height);

        entries.append(ApplicationMenuEntry{
            .action = action,
            .text = text,
            .rect = bounds,
            .active = m_activeAction == action,
            .hovered = m_hoveredAction == action,
        });

        lastPosition += bounds.width();
    }

    return entries;
}

static bool diff(const QList<ApplicationMenuEntry> &left, const QList<ApplicationMenuEntry> &right)
{
    if (left.size() != right.size()) {
        return true;
    }

    for (int i = 0; i < left.size(); ++i) {
        const bool dirty = left[i].action != right[i].action || left[i].text != right[i].text || left[i].rect != right[i].rect
            || left[i].hovered != right[i].hovered || left[i].active != right[i].active;
        if (dirty) {
            return true;
        }
    }

    return false;
}

void ApplicationMenuButton::rebuild()
{
    const auto newEntries = build();
    if (!diff(m_entries, newEntries)) {
        return;
    }

    QRectF bounds;
    for (const auto &entry : newEntries) {
        bounds |= entry.rect;
    }

    m_entries = newEntries;
    update();

    setPreferredSize(bounds.size());
}

} // namespace Darkly

/*
 * SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include "darklybutton.h"
#include "libdbusmenuqt/dbusmenuimporter.h"

#include <QMenu>

namespace Darkly
{

struct ApplicationMenuEntry {
    QAction *action;
    QString text;
    QRectF rect;
    bool active = false;
    bool hovered = false;

    bool diff(const ApplicationMenuEntry &other) const;
};

class ApplicationMenu : public QMenu
{
    Q_OBJECT

public:
    explicit ApplicationMenu(QWidget *parent = nullptr);

Q_SIGNALS:
    void hitLeft();
    void hitRight();

protected:
    void keyPressEvent(QKeyEvent *) override;
};

class ApplicationMenuImporter : public DBusMenuImporter
{
    Q_OBJECT

public:
    ApplicationMenuImporter(const QString &service, const QString &path, QObject *parent = nullptr);

protected:
    QMenu *createMenu(QWidget *parent) override;
};

class ApplicationMenuButton : public Button
{
    Q_OBJECT

public:
    ApplicationMenuButton(KDecoration3::Decoration *decoration);
    ~ApplicationMenuButton() override;

    void paint(QPainter *painter, const QRectF &repaintRegion) override;

protected:
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void track(const QString &serviceName, const QString &objectPath);
    void rebuild();
    QList<ApplicationMenuEntry> build() const;

    int findAction(QAction *action) const;
    QAction *actionAt(const QPointF &point) const;
    QAction *previousAction(QAction *reference) const;
    QAction *nextAction(QAction *reference) const;
    void triggerAction(QAction *action);
    QRectF mapToDecoration(const QRectF &rect) const;

    void showMenu(QAction *action);
    void showPreviousMenu();
    void showNextMenu();
    void onMenuHidden();

    std::unique_ptr<ApplicationMenuImporter> m_menuImporter;
    QList<ApplicationMenuEntry> m_entries;
    QPointer<QAction> m_hoveredAction;
    QPointer<QAction> m_activeAction;
};

} // namespace Darkly

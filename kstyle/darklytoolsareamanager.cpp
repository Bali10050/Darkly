#include "darklytoolsareamanager.h"
#include "darklypropertynames.h"

#include <QMainWindow>
#include <QMdiArea>
#include <QMenuBar>
#include <QObject>
#include <QToolBar>
#include <QWidget>
#include <QWindow>

#include <KColorUtils>

const char *colorProperty = "KDE_COLOR_SCHEME_PATH";

namespace Darkly
{
ToolsAreaManager::ToolsAreaManager(Helper *helper, QObject *parent)
    : QObject(parent)
    , _helper(helper)
{
    if (qApp && qApp->property(colorProperty).isValid()) {
        auto path = qApp->property(colorProperty).toString();
        _config = KSharedConfig::openConfig(path);
    } else {
        _config = KSharedConfig::openConfig();
    }
    _watcher = KConfigWatcher::create(_config);
    connect(_watcher.data(), &KConfigWatcher::configChanged, this, &ToolsAreaManager::configUpdated);
    configUpdated();
}

ToolsAreaManager::~ToolsAreaManager()
{
}

void ToolsAreaManager::recreateConfigWatcher(const QString &path)
{
    const auto openFlags = path.isEmpty() ? KConfig::OpenFlag::FullConfig : KConfig::OpenFlag::NoGlobals;
    _config = KSharedConfig::openConfig(path, openFlags);

    if (!path.startsWith(QLatin1Char('/'))) {
        _watcher = KConfigWatcher::create(_config);
        connect(_watcher.data(), &KConfigWatcher::configChanged, this, &ToolsAreaManager::configUpdated);
    } else {
        _watcher.reset();
    }
}

void ToolsAreaManager::appendIfNotAlreadyExists(const QMainWindow *window, const QPointer<QToolBar> &toolBar)
{
    const auto windowIt = std::find_if(_windows.begin(), _windows.end(), [window](const WindowToolBars &windowToolBars) {
        return window == windowToolBars.window;
    });
    if (windowIt != _windows.end()) {
        if (!windowIt->toolBars.contains(toolBar)) {
            windowIt->toolBars.append(toolBar);
        }
    } else {
        _windows.emplace_back(WindowToolBars{window, {toolBar}});
        connect(window, &QObject::destroyed, this, [this, window] {
            removeWindow(window);
        });
    }
}

void ToolsAreaManager::removeWindowToolBar(const QMainWindow *window, const QPointer<QToolBar> &toolBar)
{
    const auto windowIt = std::find_if(_windows.begin(), _windows.end(), [window](const WindowToolBars &windowToolBars) {
        return window == windowToolBars.window;
    });
    if (windowIt != _windows.end()) {
        windowIt->toolBars.removeAll(toolBar);
    }
}

void ToolsAreaManager::removeWindow(const QMainWindow *window)
{
    std::erase_if(_windows, [window](const WindowToolBars &windowToolBars) {
        return window == windowToolBars.window;
    });
}

void ToolsAreaManager::registerApplication(QApplication *application)
{
    _listener = new AppListener(this);
    _listener->manager = this;
    if (application->property(colorProperty).isValid()) {
        auto path = application->property(colorProperty).toString();
        recreateConfigWatcher(path);
    }
    application->installEventFilter(_listener);
    configUpdated();
}

QRect ToolsAreaManager::toolsAreaRect(const QMainWindow &window) const
{
    int itemHeight = window.menuWidget() ? window.menuWidget()->height() : 0;
    const auto windowIt = std::find_if(_windows.begin(), _windows.end(), [&window](const WindowToolBars &windowToolBars) {
        return &window == windowToolBars.window;
    });
    if (windowIt != _windows.end()) {
        for (auto item : windowIt->toolBars) {
            if (!item.isNull() && item->isVisible() && window.toolBarArea(item) == Qt::TopToolBarArea) {
                itemHeight = qMax(item->mapTo(&window, item->rect().bottomLeft()).y(), itemHeight);
            }
        }
    }
    if (itemHeight > 0) {
        itemHeight += 1;
    }

    return QRect(0, 0, window.width(), itemHeight);
}

bool ToolsAreaManager::tryRegisterToolBar(QPointer<const QMainWindow> window, QPointer<QWidget> widget)
{
    Q_ASSERT(!widget.isNull());

    QPointer<QToolBar> toolbar;
    if (!(toolbar = qobject_cast<QToolBar *>(widget))) {
        return false;
    }

    if (window->toolBarArea(toolbar) == Qt::TopToolBarArea) {
        widget->setPalette(palette());
        appendIfNotAlreadyExists(window, toolbar);
        return true;
    }

    return false;
}

void ToolsAreaManager::tryUnregisterToolBar(QPointer<const QMainWindow> window, QPointer<QWidget> widget)
{
    Q_ASSERT(!widget.isNull());

    QPointer<QToolBar> toolbar;
    if (!(toolbar = qobject_cast<QToolBar *>(widget))) {
        return;
    }

    if (window->toolBarArea(toolbar) != Qt::TopToolBarArea) {
        widget->setPalette(window->palette());
        removeWindowToolBar(window, toolbar);
    }
}

void ToolsAreaManager::configUpdated()
{
    auto active = KColorScheme(QPalette::Active, KColorScheme::Header, _config);
    auto inactive = KColorScheme(QPalette::Inactive, KColorScheme::Header, _config);
    auto disabled = KColorScheme(QPalette::Disabled, KColorScheme::Header, _config);

    _palette = KColorScheme::createApplicationPalette(_config);

    _palette.setBrush(QPalette::Active, QPalette::Window, active.background());
    _palette.setBrush(QPalette::Active, QPalette::WindowText, active.foreground());
    _palette.setBrush(QPalette::Disabled, QPalette::Window, disabled.background());
    _palette.setBrush(QPalette::Disabled, QPalette::WindowText, disabled.foreground());
    _palette.setBrush(QPalette::Inactive, QPalette::Window, inactive.background());
    _palette.setBrush(QPalette::Inactive, QPalette::WindowText, inactive.foreground());

    for (const WindowToolBars &windowToolBars : _windows) {
        for (const auto &toolbar : windowToolBars.toolBars) {
            if (!toolbar.isNull()) {
                toolbar->setPalette(_palette);
            }
        }

        if (QMenuBar *menuBar = windowToolBars.window->menuBar()) {
            menuBar->setPalette(_palette);
        }
    }

    _colorSchemeHasHeaderColor = KColorScheme::isColorSetSupported(_config, KColorScheme::Header);
}

bool AppListener::eventFilter(QObject *watched, QEvent *event)
{
    Q_ASSERT(watched);
    Q_ASSERT(event);

    if (watched != qApp) {
        return false;
    }

    if (event->type() == QEvent::DynamicPropertyChange) {
        if (watched != qApp) {
            return false;
        }
        auto ev = static_cast<QDynamicPropertyChangeEvent *>(event);
        if (ev->propertyName() == colorProperty) {
            QString path;
            if (qApp && qApp->property(colorProperty).isValid()) {
                path = qApp->property(colorProperty).toString();
            }
            manager->recreateConfigWatcher(path);
            manager->configUpdated();
        }
    }

    return false;
}

bool ToolsAreaManager::eventFilter(QObject *watched, QEvent *event)
{
    Q_ASSERT(watched);
    Q_ASSERT(event);

    QPointer<QObject> parent = watched;
    QPointer<const QMainWindow> mainWindow = nullptr;
    while (parent != nullptr) {
        if (qobject_cast<QMainWindow *>(parent)) {
            mainWindow = qobject_cast<QMainWindow *>(parent);
            break;
        }
        parent = parent->parent();
    }

    if (const QMainWindow *mw = qobject_cast<QMainWindow *>(watched)) {
        QChildEvent *ev = nullptr;
        if (event->type() == QEvent::ChildAdded || event->type() == QEvent::ChildRemoved) {
            ev = static_cast<QChildEvent *>(event);

            if (event->type() == QEvent::ChildAdded) {
                QChildEvent *childEvent = static_cast<QChildEvent *>(event);
                if (QMenuBar *menuBar = qobject_cast<QMenuBar *>(childEvent->child())) {
                    menuBar->setPalette(_palette);
                }
            }
        } else {
            return false;
        }

        QPointer<QToolBar> tb = qobject_cast<QToolBar *>(ev->child());
        if (tb.isNull()) {
            return false;
        }

        if (ev->added()) {
            if (mw->toolBarArea(tb) == Qt::TopToolBarArea) {
                appendIfNotAlreadyExists(mw, tb);
            }
        } else if (ev->removed()) {
            removeWindowToolBar(mw, tb);
        }
    } else if (qobject_cast<QToolBar *>(watched)) {
        if (!mainWindow.isNull()) {
            tryUnregisterToolBar(mainWindow, qobject_cast<QWidget *>(watched));
        }
    }

    return false;
}

void ToolsAreaManager::registerWidget(QWidget *widget)
{
    Q_ASSERT(widget);
    auto ptr = QPointer<QWidget>(widget);

    QPointer<const QMainWindow> mainWindow = qobject_cast<QMainWindow *>(ptr);

    if (mainWindow && mainWindow.data() == mainWindow->window()) {
        const auto toolBars = mainWindow->findChildren<QToolBar *>(QString(), Qt::FindDirectChildrenOnly);
        for (auto *toolBar : toolBars) {
            tryRegisterToolBar(mainWindow, toolBar);
        }

        if (QMenuBar *menuBar = mainWindow->menuBar()) {
            menuBar->setPalette(_palette);
        }

        return;
    }

    auto parent = ptr;

    while (parent != nullptr) {
        if (qobject_cast<QMdiArea *>(parent) || qobject_cast<QDockWidget *>(parent)) {
            break;
        }
        if (auto window = qobject_cast<QMainWindow *>(parent)) {
            mainWindow = window;
        }
        parent = parent->parentWidget();
    }
    if (mainWindow == nullptr) {
        return;
    }
    if (mainWindow.data() != mainWindow->window()) {
        return;
    }
    tryRegisterToolBar(mainWindow, widget);
}

void ToolsAreaManager::unregisterWidget(QWidget *widget)
{
    Q_ASSERT(widget);
    auto ptr = QPointer<QWidget>(widget);

    if (QPointer<const QMainWindow> window = qobject_cast<QMainWindow *>(ptr)) {
        removeWindow(window);
        return;
    } else if (QPointer<QToolBar> toolbar = qobject_cast<QToolBar *>(ptr)) {
        auto parent = ptr;
        QPointer<const QMainWindow> mainWindow = nullptr;
        while (parent != nullptr) {
            if (qobject_cast<QMainWindow *>(parent)) {
                mainWindow = qobject_cast<QMainWindow *>(parent);
                break;
            }
            parent = parent->parentWidget();
        }
        if (mainWindow == nullptr) {
            return;
        }
        removeWindowToolBar(mainWindow, toolbar);
    }
}
bool Darkly::ToolsAreaManager::hasHeaderColors()
{
    return _colorSchemeHasHeaderColor;
}
}
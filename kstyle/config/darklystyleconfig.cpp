
/*************************************************************************
 * Copyright (C) 2014 by Hugo Pereira Da Costa <hugo.pereira@free.fr>    *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *************************************************************************/

#include "darklystyleconfig.h"

#include "../config-darkly.h"
#include "../darkly.h"
#include "darklystyleconfigdata.h"
#include "darklystyleversion.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <KLocalizedString>

extern "C" {
Q_DECL_EXPORT QWidget *allocate_kstyle_config(QWidget *parent)
{
    return new Darkly::StyleConfig(parent);
}
}

namespace Darkly
{

//__________________________________________________________________
StyleConfig::StyleConfig(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    // load setup from configData
    load();

    connect(_tabDrawHighlight, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_unifiedTabBarKonsole, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_renderThinSeperatorBetweenTheScrollBar, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_toolBarDrawItemSeparator, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_viewDrawFocusIndicator, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_dockWidgetDrawFrame, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_titleWidgetDrawFrame, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_sidePanelDrawFrame, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_menuItemDrawThinFocus, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_roundedRubberBandFrame, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_mnemonicsMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(_animationsEnabled, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_animationsDuration, SIGNAL(valueChanged(int)), SLOT(updateChanged()));
    connect(_scrollBarAddLineButtons, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(_scrollBarSubLineButtons, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(_windowDragMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(_menuOpacity, &QAbstractSlider::valueChanged, this, &StyleConfig::updateChanged);
    connect(_menuOpacity, SIGNAL(valueChanged(int)), _menuOpacitySpinBox, SLOT(setValue(int)));
    connect(_menuOpacitySpinBox, SIGNAL(valueChanged(int)), _menuOpacity, SLOT(setValue(int)));
    connect(_buttonSize, &QAbstractSlider::valueChanged, this, &StyleConfig::updateChanged);
    connect(_sidebarOpacity, &QAbstractSlider::valueChanged, this, &StyleConfig::updateChanged);
    connect(_sidebarOpacity, SIGNAL(valueChanged(int)), _sidebarOpacitySpinBox, SLOT(setValue(int)));
    connect(_sidebarOpacitySpinBox, SIGNAL(valueChanged(int)), _sidebarOpacity, SLOT(setValue(int)));

    connect(_tabBarDrawCenteredTabs, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_menuBarOpacity, &QAbstractSlider::valueChanged, this, &StyleConfig::updateChanged);
    connect(_menuBarOpacity, SIGNAL(valueChanged(int)), _menuBarOpacitySpinBox, SLOT(setValue(int)));
    connect(_menuBarOpacitySpinBox, SIGNAL(valueChanged(int)), _menuBarOpacity, SLOT(setValue(int)));

    connect(_toolBarOpacity, &QAbstractSlider::valueChanged, this, &StyleConfig::updateChanged);
    connect(_toolBarOpacity, SIGNAL(valueChanged(int)), _toolBarOpacitySpinBox, SLOT(setValue(int)));
    connect(_toolBarOpacitySpinBox, SIGNAL(valueChanged(int)), _toolBarOpacity, SLOT(setValue(int)));

    connect(_kTextEditDrawFrame, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);

    connect(_widgetDrawShadow, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_widgetToolBarShadow, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_shadowSize, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(_shadowColor, &KColorCombo::activated, this, &StyleConfig::updateChanged);
    connect(_shadowStrength, SIGNAL(valueChanged(int)), _shadowStrength, SLOT(setValue(int)));
    connect(_shadowIntensity, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));

    connect(_scrollableMenu, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_oldTabbar, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_adjustToDarkThemes, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_tabBGColor, &KColorButton::changed, this, &StyleConfig::updateChanged);

    connect(_tabBarAltStyle, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_transparentDolphinView, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_cornerRadius, SIGNAL(valueChanged(int)), SLOT(updateChanged()));
    connect(_tabUseHighlightColor, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_tabUseBrighterCloseIcon, &QAbstractButton::toggled, this, &StyleConfig::updateChanged);
    connect(_tabsHeight, &QAbstractSlider::valueChanged, this, &StyleConfig::updateChanged);
}

//__________________________________________________________________
void StyleConfig::save()
{
    StyleConfigData::setTabDrawHighlight(_tabDrawHighlight->isChecked());
    StyleConfigData::setUnifiedTabBarKonsole(_unifiedTabBarKonsole->isChecked());
    StyleConfigData::setRenderThinSeperatorBetweenTheScrollBar(_renderThinSeperatorBetweenTheScrollBar->isChecked());
    StyleConfigData::setToolBarDrawItemSeparator(_toolBarDrawItemSeparator->isChecked());
    StyleConfigData::setViewDrawFocusIndicator(_viewDrawFocusIndicator->isChecked());
    StyleConfigData::setDockWidgetDrawFrame(_dockWidgetDrawFrame->isChecked());
    StyleConfigData::setTitleWidgetDrawFrame(_titleWidgetDrawFrame->isChecked());
    StyleConfigData::setSidePanelDrawFrame(_sidePanelDrawFrame->isChecked());
    StyleConfigData::setMenuItemDrawStrongFocus(!_menuItemDrawThinFocus->isChecked());
    StyleConfigData::setRoundedRubberBandFrame(_roundedRubberBandFrame->isChecked());
    StyleConfigData::setMnemonicsMode(_mnemonicsMode->currentIndex());
    StyleConfigData::setScrollBarAddLineButtons(_scrollBarAddLineButtons->currentIndex());
    StyleConfigData::setScrollBarSubLineButtons(_scrollBarSubLineButtons->currentIndex());
    StyleConfigData::setAnimationsEnabled(_animationsEnabled->isChecked());
    StyleConfigData::setTabBarDrawCenteredTabs(_tabBarDrawCenteredTabs->isChecked());
    StyleConfigData::setAnimationsDuration(_animationsDuration->value());
    StyleConfigData::setWindowDragMode(_windowDragMode->currentIndex());
    StyleConfigData::setMenuOpacity(_menuOpacity->value());
    StyleConfigData::setDolphinSidebarOpacity(_sidebarOpacity->value());
    StyleConfigData::setMenuBarOpacity(_menuBarOpacity->value());
    StyleConfigData::setToolBarOpacity(_toolBarOpacity->value());
    StyleConfigData::setButtonSize(_buttonSize->value());
    StyleConfigData::setKTextEditDrawFrame(_kTextEditDrawFrame->isChecked());
    StyleConfigData::setWidgetDrawShadow(_widgetDrawShadow->isChecked());
    StyleConfigData::setWidgetToolBarShadow(_widgetToolBarShadow->isChecked());
    StyleConfigData::setShadowSize(_shadowSize->currentIndex());
    StyleConfigData::setShadowColor(_shadowColor->color());
    StyleConfigData::setShadowStrength(_shadowStrength->value());
    StyleConfigData::setShadowIntensity(_shadowIntensity->currentIndex());
    StyleConfigData::setScrollableMenu(_scrollableMenu->isChecked());
    StyleConfigData::setOldTabbar(_oldTabbar->isChecked());
    StyleConfigData::setAdjustToDarkThemes(_adjustToDarkThemes->isChecked());
    StyleConfigData::setTabBGColor(_tabBGColor->color());
    StyleConfigData::setTabBarAltStyle(_tabBarAltStyle->isChecked());
    StyleConfigData::setTransparentDolphinView(_transparentDolphinView->isChecked());
    StyleConfigData::setCornerRadius(_cornerRadius->value());
    StyleConfigData::setTabUseHighlightColor(_tabUseHighlightColor->isChecked());
    StyleConfigData::setTabUseBrighterCloseIcon(_tabUseBrighterCloseIcon->isChecked());
    StyleConfigData::setTabsHeight(_tabsHeight->value());

    StyleConfigData::self()->save();

    // emit dbus signal
    QDBusMessage message(
        QDBusMessage::createSignal(QStringLiteral("/DarklyStyle"), QStringLiteral("org.kde.Darkly.Style"), QStringLiteral("reparseConfiguration")));
    QDBusConnection::sessionBus().send(message);
}

//__________________________________________________________________
void StyleConfig::defaults()
{
    StyleConfigData::self()->setDefaults();
    load();
}

//__________________________________________________________________
void StyleConfig::reset()
{
    // reparse configuration
    StyleConfigData::self()->load();

    load();
}

//__________________________________________________________________
void StyleConfig::updateChanged()
{
    bool modified(false);

    // check if any value was modified
    if (_tabDrawHighlight->isChecked() != StyleConfigData::tabDrawHighlight())
        modified = true;
    else if (_unifiedTabBarKonsole->isChecked() != StyleConfigData::unifiedTabBarKonsole())
        modified = true;
    else if (_renderThinSeperatorBetweenTheScrollBar->isChecked() != StyleConfigData::renderThinSeperatorBetweenTheScrollBar())
        modified = true;
    else if (_toolBarDrawItemSeparator->isChecked() != StyleConfigData::toolBarDrawItemSeparator())
        modified = true;
    else if (_viewDrawFocusIndicator->isChecked() != StyleConfigData::viewDrawFocusIndicator())
        modified = true;
    else if (_dockWidgetDrawFrame->isChecked() != StyleConfigData::dockWidgetDrawFrame())
        modified = true;
    else if (_titleWidgetDrawFrame->isChecked() != StyleConfigData::titleWidgetDrawFrame())
        modified = true;
    else if (_sidePanelDrawFrame->isChecked() != StyleConfigData::sidePanelDrawFrame())
        modified = true;
    else if (_menuItemDrawThinFocus->isChecked() == StyleConfigData::menuItemDrawStrongFocus())
        modified = true;
    else if (_roundedRubberBandFrame->isChecked() == StyleConfigData::roundedRubberBandFrame())
        modified = true;
    else if (_mnemonicsMode->currentIndex() != StyleConfigData::mnemonicsMode())
        modified = true;
    else if (_scrollBarAddLineButtons->currentIndex() != StyleConfigData::scrollBarAddLineButtons())
        modified = true;
    else if (_scrollBarSubLineButtons->currentIndex() != StyleConfigData::scrollBarSubLineButtons())
        modified = true;
    else if (_animationsEnabled->isChecked() != StyleConfigData::animationsEnabled())
        modified = true;
    else if (_animationsDuration->value() != StyleConfigData::animationsDuration())
        modified = true;
    else if (_windowDragMode->currentIndex() != StyleConfigData::windowDragMode())
        modified = true;
    else if (_menuOpacity->value() != StyleConfigData::menuOpacity()) {
        modified = true;
        _menuOpacitySpinBox->setValue(_menuOpacity->value());
    } else if (_buttonSize->value() != StyleConfigData::buttonSize())
        modified = true;
    else if (_sidebarOpacity->value() != StyleConfigData::dolphinSidebarOpacity()) {
        modified = true;
        _sidebarOpacitySpinBox->setValue(_sidebarOpacity->value());
    } else if (_menuBarOpacity->value() != StyleConfigData::menuBarOpacity()) {
        modified = true;
        _menuBarOpacitySpinBox->setValue(_menuBarOpacity->value());
    } else if (_toolBarOpacity->value() != StyleConfigData::toolBarOpacity()) {
        modified = true;
        _toolBarOpacitySpinBox->setValue(_toolBarOpacity->value());
    } else if (_kTextEditDrawFrame->isChecked() != StyleConfigData::kTextEditDrawFrame())
        modified = true;
    else if (_tabBarDrawCenteredTabs->isChecked() != StyleConfigData::tabBarDrawCenteredTabs())
        modified = true;
    else if (_widgetDrawShadow->isChecked() != StyleConfigData::widgetDrawShadow())
        modified = true;
    else if (_widgetToolBarShadow->isChecked() != StyleConfigData::widgetToolBarShadow())
        modified = true;
    else if (_shadowSize->currentIndex() != StyleConfigData::shadowSize()) {
        modified = true;
    } else if (_shadowColor->color() != StyleConfigData::shadowColor())
        modified = true;
    else if (_shadowStrength->value() != StyleConfigData::shadowStrength())
        modified = true;
    else if (_shadowIntensity->currentIndex() != StyleConfigData::shadowIntensity())
        modified = true;
    else if (_scrollableMenu->isChecked() != StyleConfigData::scrollableMenu())
        modified = true;
    else if (_oldTabbar->isChecked() != StyleConfigData::oldTabbar())
        modified = true;
    else if (_adjustToDarkThemes->isChecked() != StyleConfigData::adjustToDarkThemes())
        modified = true;
    else if (_tabBarAltStyle->isChecked() != StyleConfigData::tabBarAltStyle())
        modified = true;
    else if (_transparentDolphinView->isChecked() != StyleConfigData::transparentDolphinView())
        modified = true;
    else if (_cornerRadius->value() != StyleConfigData::cornerRadius())
        modified = true;
    else if (_tabBGColor->color() != StyleConfigData::tabBGColor())
        modified = true;
    else if (_tabUseHighlightColor->isChecked() != StyleConfigData::tabUseHighlightColor())
        modified = true;
    else if (_tabUseBrighterCloseIcon->isChecked() != StyleConfigData::tabUseBrighterCloseIcon())
        modified = true;
    else if (_tabsHeight->value() != StyleConfigData::tabsHeight())
        modified = true;

    if (_shadowSize->currentIndex() == 0) {
        _shadowColor->setEnabled(false);
        _shadowIntensity->setEnabled(false);
        _shadowStrength->setEnabled(false);
    } else {
        _shadowColor->setEnabled(true);
        _shadowIntensity->setEnabled(true);
        _shadowStrength->setEnabled(true);
    }

    if (!_widgetDrawShadow->isChecked()) {
        _widgetToolBarShadow->setEnabled(false);
    } else {
        _widgetToolBarShadow->setEnabled(true);
    }

    emit changed(modified);
}

//__________________________________________________________________
void StyleConfig::load()
{
    _tabDrawHighlight->setChecked(StyleConfigData::tabDrawHighlight());
    _unifiedTabBarKonsole->setChecked(StyleConfigData::unifiedTabBarKonsole());
    _renderThinSeperatorBetweenTheScrollBar->setChecked(StyleConfigData::renderThinSeperatorBetweenTheScrollBar());
    _toolBarDrawItemSeparator->setChecked(StyleConfigData::toolBarDrawItemSeparator());
    _viewDrawFocusIndicator->setChecked(StyleConfigData::viewDrawFocusIndicator());
    _dockWidgetDrawFrame->setChecked(StyleConfigData::dockWidgetDrawFrame());
    _titleWidgetDrawFrame->setChecked(StyleConfigData::titleWidgetDrawFrame());
    _sidePanelDrawFrame->setChecked(StyleConfigData::sidePanelDrawFrame());
    _menuItemDrawThinFocus->setChecked(!StyleConfigData::menuItemDrawStrongFocus());
    _roundedRubberBandFrame->setChecked(StyleConfigData::roundedRubberBandFrame());
    _mnemonicsMode->setCurrentIndex(StyleConfigData::mnemonicsMode());
    _scrollBarAddLineButtons->setCurrentIndex(StyleConfigData::scrollBarAddLineButtons());
    _scrollBarSubLineButtons->setCurrentIndex(StyleConfigData::scrollBarSubLineButtons());
    _animationsEnabled->setChecked(StyleConfigData::animationsEnabled());
    _animationsDuration->setValue(StyleConfigData::animationsDuration());
    _windowDragMode->setCurrentIndex(StyleConfigData::windowDragMode());
    _menuOpacity->setValue(StyleConfigData::menuOpacity());
    _menuOpacitySpinBox->setValue(StyleConfigData::menuOpacity());
    _sidebarOpacity->setValue(StyleConfigData::dolphinSidebarOpacity());
    _sidebarOpacitySpinBox->setValue(StyleConfigData::dolphinSidebarOpacity());
    _tabBarDrawCenteredTabs->setChecked(StyleConfigData::tabBarDrawCenteredTabs());
    _menuBarOpacity->setValue(StyleConfigData::menuBarOpacity());
    _menuBarOpacitySpinBox->setValue(StyleConfigData::menuBarOpacity());
    _toolBarOpacity->setValue(StyleConfigData::toolBarOpacity());
    _toolBarOpacitySpinBox->setValue(StyleConfigData::toolBarOpacity());

    _buttonSize->setValue(StyleConfigData::buttonSize());
    _kTextEditDrawFrame->setChecked(StyleConfigData::kTextEditDrawFrame());
    _widgetDrawShadow->setChecked(StyleConfigData::widgetDrawShadow());
    _widgetToolBarShadow->setChecked(StyleConfigData::widgetToolBarShadow());

    if (!_widgetDrawShadow->isChecked()) {
        _widgetToolBarShadow->setEnabled(false);
    }

    for (QString &item : _shadowSizes) {
        if (item == "None") {
            _shadowSize->addItem(item, "ShadowNone");
        } else if (item == "Small") {
            _shadowSize->addItem(item, "ShadowSmall");
        } else if (item == "Medium") {
            _shadowSize->addItem(item, "ShadowMedium");
        } else if (item == "Large") {
            _shadowSize->addItem(item, "ShadowLarge");
        } else if (item == "VeryLarge") {
            _shadowSize->addItem(item, "ShadowVeryLarge");
        }
    }
    _shadowSize->setCurrentIndex(StyleConfigData::shadowSize());
    if (_shadowSize->currentIndex() == 0) {
        _shadowColor->setEnabled(false);
        _shadowIntensity->setEnabled(false);
        _shadowStrength->setEnabled(false);
    }
    _shadowColor->setColor(StyleConfigData::shadowColor());
    _shadowStrength->setValue(StyleConfigData::shadowStrength());
    _shadowIntensity->setCurrentIndex(StyleConfigData::shadowIntensity());
    _scrollableMenu->setChecked(StyleConfigData::scrollableMenu());
    _oldTabbar->setChecked(StyleConfigData::oldTabbar());
    _adjustToDarkThemes->setChecked(StyleConfigData::adjustToDarkThemes());
    _tabBarAltStyle->setChecked(StyleConfigData::tabBarAltStyle());
    _tabBGColor->setColor(StyleConfigData::tabBGColor());
    _transparentDolphinView->setChecked(StyleConfigData::transparentDolphinView());
    _cornerRadius->setValue(StyleConfigData::cornerRadius());
    _tabUseHighlightColor->setChecked(StyleConfigData::tabUseHighlightColor());
    _tabUseBrighterCloseIcon->setChecked(StyleConfigData::tabUseBrighterCloseIcon());
    _versionNumber->setText(DARKLY_VERSION_STRING);
    _tabsHeight->setValue(StyleConfigData::tabsHeight());
}

}

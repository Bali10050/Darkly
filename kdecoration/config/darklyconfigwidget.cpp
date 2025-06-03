//////////////////////////////////////////////////////////////////////////////
// darklyconfigurationui.cpp
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "darklyconfigwidget.h"
#include "darklyexceptionlist.h"

#include <KLocalizedString>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QFontDatabase>

namespace Darkly
{
//_________________________________________________________
ConfigWidget::ConfigWidget(QObject *parent, const KPluginMetaData &data, const QVariantList & /*args*/)
    : KCModule(parent, data)
    , m_configuration(KSharedConfig::openConfig(QStringLiteral("darklyrc")))
    , m_changed(false)
{
    // configuration
    m_ui.setupUi(widget());

    m_ui.tabWidget->tabBar()->setExpanding(true);

    // track ui changes
    connect(m_ui.titleAlignment, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(m_ui.buttonSize, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(m_ui.outlineCloseButton, &QAbstractButton::clicked, this, &ConfigWidget::updateChanged);
    connect(m_ui.drawBorderOnMaximizedWindows, &QAbstractButton::clicked, this, &ConfigWidget::updateChanged);
    connect(m_ui.drawBackgroundGradient, &QAbstractButton::clicked, this, &ConfigWidget::updateChanged);

    // track shadows changes
    connect(m_ui.shadowSize, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(m_ui.shadowStrength, SIGNAL(valueChanged(int)), SLOT(updateChanged()));
    connect(m_ui.shadowColor, &KColorButton::changed, this, &ConfigWidget::updateChanged);
    connect(m_ui.outlineIntensity, SIGNAL(activated(int)), SLOT(updateChanged()));

    // track exception changes
    connect(m_ui.exceptions, &ExceptionListWidget::changed, this, &ConfigWidget::updateChanged);

    // set formatting
    m_ui.drawBorderOnMaximizedWindowsHelpLabel->setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));
}

//_________________________________________________________
void ConfigWidget::load()
{
    // create internal settings and load from rc files
    m_internalSettings = InternalSettingsPtr(new InternalSettings());
    m_internalSettings->load();

    // assign to ui
    m_ui.titleAlignment->setCurrentIndex(m_internalSettings->titleAlignment());
    m_ui.buttonSize->setCurrentIndex(m_internalSettings->buttonSize());
    m_ui.drawBorderOnMaximizedWindows->setChecked(m_internalSettings->drawBorderOnMaximizedWindows());
    m_ui.outlineCloseButton->setChecked(m_internalSettings->outlineCloseButton());
    m_ui.drawBackgroundGradient->setChecked(m_internalSettings->drawBackgroundGradient());

    // load shadows
    if (m_internalSettings->shadowSize() <= InternalSettings::ShadowVeryLarge) {
        m_ui.shadowSize->setCurrentIndex(m_internalSettings->shadowSize());
    } else {
        m_ui.shadowSize->setCurrentIndex(InternalSettings::ShadowLarge);
    }

    m_ui.shadowStrength->setValue(qRound(qreal(m_internalSettings->shadowStrength() * 100) / 255));
    m_ui.shadowColor->setColor(m_internalSettings->shadowColor());

    // load outline intensity
    if (m_internalSettings->outlineIntensity() <= InternalSettings::OutlineMaximum) {
        m_ui.outlineIntensity->setCurrentIndex(m_internalSettings->outlineIntensity());
    } else {
        m_ui.outlineIntensity->setCurrentIndex(InternalSettings::OutlineMedium);
    }

    // load exceptions
    ExceptionList exceptions;
    exceptions.readConfig(m_configuration);
    m_ui.exceptions->setExceptions(exceptions.get());
    setNeedsSave(false);
}

//_________________________________________________________
void ConfigWidget::save()
{
    // create internal settings and load from rc files
    m_internalSettings = InternalSettingsPtr(new InternalSettings());
    m_internalSettings->load();

    // apply modifications from ui
    m_internalSettings->setTitleAlignment(m_ui.titleAlignment->currentIndex());
    m_internalSettings->setButtonSize(m_ui.buttonSize->currentIndex());
    m_internalSettings->setOutlineCloseButton(m_ui.outlineCloseButton->isChecked());
    m_internalSettings->setDrawBorderOnMaximizedWindows(m_ui.drawBorderOnMaximizedWindows->isChecked());
    m_internalSettings->setDrawBackgroundGradient(m_ui.drawBackgroundGradient->isChecked());

    m_internalSettings->setShadowSize(m_ui.shadowSize->currentIndex());
    m_internalSettings->setShadowStrength(qRound(qreal(m_ui.shadowStrength->value() * 255) / 100));
    m_internalSettings->setShadowColor(m_ui.shadowColor->color());
    m_internalSettings->setOutlineIntensity(m_ui.outlineIntensity->currentIndex());

    // save configuration
    m_internalSettings->save();

    // get list of exceptions and write
    InternalSettingsList exceptions(m_ui.exceptions->exceptions());
    ExceptionList(exceptions).writeConfig(m_configuration);

    // sync configuration
    m_configuration->sync();
    setNeedsSave(false);

    // needed to tell kwin to reload when running from external kcmshell
    {
        QDBusMessage message = QDBusMessage::createSignal("/KWin", "org.kde.KWin", "reloadConfig");
        QDBusConnection::sessionBus().send(message);
    }

    // needed for darkly style to reload shadows
    {
        QDBusMessage message(QDBusMessage::createSignal("/DarklyDecoration", "org.kde.Darkly.Style", "reparseConfiguration"));
        QDBusConnection::sessionBus().send(message);
    }
}

//_________________________________________________________
void ConfigWidget::defaults()
{
    // create internal settings and load from rc files
    m_internalSettings = InternalSettingsPtr(new InternalSettings());
    m_internalSettings->setDefaults();

    // assign to ui
    m_ui.titleAlignment->setCurrentIndex(m_internalSettings->titleAlignment());
    m_ui.buttonSize->setCurrentIndex(m_internalSettings->buttonSize());
    m_ui.outlineCloseButton->setChecked(m_internalSettings->outlineCloseButton());
    m_ui.drawBorderOnMaximizedWindows->setChecked(m_internalSettings->drawBorderOnMaximizedWindows());
    m_ui.drawBackgroundGradient->setChecked(m_internalSettings->drawBackgroundGradient());

    m_ui.shadowSize->setCurrentIndex(m_internalSettings->shadowSize());
    m_ui.shadowStrength->setValue(qRound(qreal(m_internalSettings->shadowStrength() * 100) / 255));
    m_ui.shadowColor->setColor(m_internalSettings->shadowColor());
    m_ui.outlineIntensity->setCurrentIndex(m_internalSettings->outlineIntensity());
}

//_______________________________________________
void ConfigWidget::updateChanged()
{
    // check configuration
    if (!m_internalSettings) {
        return;
    }

    // track modifications
    bool modified(false);

    if (m_ui.titleAlignment->currentIndex() != m_internalSettings->titleAlignment()) {
        modified = true;
    } else if (m_ui.buttonSize->currentIndex() != m_internalSettings->buttonSize()) {
        modified = true;
    } else if (m_ui.outlineCloseButton->isChecked() != m_internalSettings->outlineCloseButton()) {
        modified = true;
    } else if (m_ui.drawBorderOnMaximizedWindows->isChecked() != m_internalSettings->drawBorderOnMaximizedWindows()) {
        modified = true;
    } else if (m_ui.drawBackgroundGradient->isChecked() != m_internalSettings->drawBackgroundGradient()) {
        modified = true;

        // shadows
    } else if (m_ui.shadowSize->currentIndex() != m_internalSettings->shadowSize()) {
        modified = true;
    } else if (qRound(qreal(m_ui.shadowStrength->value() * 255) / 100) != m_internalSettings->shadowStrength()) {
        modified = true;
    } else if (m_ui.shadowColor->color() != m_internalSettings->shadowColor()) {
        modified = true;
    } else if (m_ui.outlineIntensity->currentIndex() != m_internalSettings->outlineIntensity()) {
        modified = true;

        // exceptions
    } else if (m_ui.exceptions->isChanged()) {
        modified = true;
    }

    setNeedsSave(modified);
}

}

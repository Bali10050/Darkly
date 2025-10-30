//////////////////////////////////////////////////////////////////////////////
// darklyconfigurationui.h
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "darkly.h"
#include "darklyexceptionlistwidget.h"
#include "darklysettings.h"
#include "ui_darklyconfigurationui.h"

#include <KCModule>
#include <KSharedConfig>

#include <QSharedPointer>
#include <QWidget>

namespace Darkly
{
//_____________________________________________
class ConfigWidget : public KCModule
{
    Q_OBJECT

public:
    //* constructor
    explicit ConfigWidget(QObject *parent, const KPluginMetaData &data, const QVariantList &args);

    //* destructor
    virtual ~ConfigWidget() = default;

    //* default
    void defaults() override;

    //* load configuration
    void load() override;

    //* save configuration
    void save() override;

protected Q_SLOTS:

    //* update changed state
    virtual void updateChanged();

private:
    //* ui
    Ui_DarklyConfigurationUI m_ui;

    //* kconfiguration object
    KSharedConfig::Ptr m_configuration;

    //* internal exception
    InternalSettingsPtr m_internalSettings;

    //* changed state
    bool m_changed;
};

}

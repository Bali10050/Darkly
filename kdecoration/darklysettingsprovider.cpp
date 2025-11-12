/*
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "darklysettingsprovider.h"

#include "darklyexceptionlist.h"

#include <QRegularExpression>
#include <QTextStream>


namespace Darkly
{

    SettingsProvider *SettingsProvider::s_self = nullptr;

    //__________________________________________________________________
    SettingsProvider::SettingsProvider()
    : m_config(KSharedConfig::openConfig(QStringLiteral("darklyrc")))
    {
        reconfigure();
    }

    //__________________________________________________________________
    SettingsProvider::~SettingsProvider()
    {
        s_self = nullptr;
    }

    //__________________________________________________________________
    SettingsProvider *SettingsProvider::self()
    {
        // TODO: this is not thread safe!
        if (!s_self) {
            s_self = new SettingsProvider();
        }

        return s_self;
    }

    //__________________________________________________________________
    void SettingsProvider::reconfigure()
    {
        if (!m_defaultSettings) {
            m_defaultSettings = InternalSettingsPtr(new InternalSettings());
            m_defaultSettings->setCurrentGroup(QStringLiteral("Windeco"));
        }

        m_defaultSettings->load();

        ExceptionList exceptions;
        exceptions.readConfig(m_config);
        m_exceptions = exceptions.get();
    }

    //__________________________________________________________________
    InternalSettingsPtr SettingsProvider::internalSettings(Decoration *decoration) const
    {
        QString windowTitle;
        QString windowClass;

        // get the client
        // auto client = decoration->client();

        foreach (auto internalSettings, m_exceptions) {
            // discard disabled exceptions
            if (!internalSettings->enabled())
                continue;

            // discard exceptions with empty exception pattern
            if (internalSettings->exceptionPattern().isEmpty())
                continue;

            /*
             *        decide which value is to be compared
             *        to the regular expression, based on exception type
             */
            QString value;
            switch (internalSettings->exceptionType()) {
                case InternalSettings::ExceptionWindowTitle: {
                    value = windowTitle.isEmpty() ? (windowTitle = decoration->window()->caption()) : windowTitle;
                    break;
                }

                default:
                case InternalSettings::ExceptionWindowClassName: {
                    value = windowClass.isEmpty() ? (windowClass = decoration->window()->windowClass()) : windowClass;
                    break;
                }
            }

            // check matching
            QRegularExpression rx(internalSettings->exceptionPattern());
            if (rx.match(value).hasMatch()) {
                return internalSettings;
            }
        }

        return m_defaultSettings;
    }

}

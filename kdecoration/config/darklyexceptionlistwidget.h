//////////////////////////////////////////////////////////////////////////////
// darklyexceptionlistwidget.h
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "darklyexceptionmodel.h"
#include "ui_darklyexceptionlistwidget.h"

//* QDialog used to commit selected files
namespace Darkly
{
class ExceptionListWidget : public QWidget
{
    //* Qt meta object
    Q_OBJECT

public:
    //* constructor
    explicit ExceptionListWidget(QWidget * = nullptr);

    //* set exceptions
    void setExceptions(const InternalSettingsList &);

    //* get exceptions
    InternalSettingsList exceptions();

    //* true if changed
    virtual bool isChanged() const
    {
        return m_changed;
    }

Q_SIGNALS:

    //* emitted when changed
    void changed(bool);

protected:
    //* model
    const ExceptionModel &model() const
    {
        return m_model;
    }

    //* model
    ExceptionModel &model()
    {
        return m_model;
    }

protected Q_SLOTS:

    //* update button states
    virtual void updateButtons();

    //* add
    virtual void add();

    //* edit
    virtual void edit();

    //* remove
    virtual void remove();

    //* toggle
    virtual void toggle(const QModelIndex &);

    //* move up
    virtual void up();

    //* move down
    virtual void down();

protected:
    //* resize columns
    void resizeColumns() const;

    //* check exception
    bool checkException(InternalSettingsPtr);

    //* set changed state
    virtual void setChanged(bool value)
    {
        m_changed = value;
        emit changed(value);
    }

private:
    //* model
    ExceptionModel m_model;

    //* ui
    Ui_DarklyExceptionListWidget m_ui;

    //* changed state
    bool m_changed = false;
};

}

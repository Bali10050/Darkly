//////////////////////////////////////////////////////////////////////////////
// darklyexceptiondialog.cpp
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "darklyexceptiondialog.h"
#include "darklydetectwidget.h"

namespace Darkly
{
//___________________________________________
ExceptionDialog::ExceptionDialog(QWidget *parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);

    connect(m_ui.buttonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &QWidget::close);

    // store checkboxes from ui into list
    m_checkboxes.insert(BorderSize, m_ui.borderSizeCheckBox);

    // detect window properties
    connect(m_ui.detectDialogButton, &QAbstractButton::clicked, this, &ExceptionDialog::selectWindowProperties);

    // connections
    connect(m_ui.exceptionType, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(m_ui.exceptionEditor, &QLineEdit::textChanged, this, &ExceptionDialog::updateChanged);
    connect(m_ui.borderSizeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));

    for (CheckBoxMap::iterator iter = m_checkboxes.begin(); iter != m_checkboxes.end(); ++iter) {
        connect(iter.value(), &QAbstractButton::clicked, this, &ExceptionDialog::updateChanged);
    }

    connect(m_ui.hideTitleBar, &QAbstractButton::clicked, this, &ExceptionDialog::updateChanged);
}

//___________________________________________
void ExceptionDialog::setException(InternalSettingsPtr exception)
{
    // store exception internally
    m_exception = exception;

    // type
    m_ui.exceptionType->setCurrentIndex(m_exception->exceptionType());
    m_ui.exceptionEditor->setText(m_exception->exceptionPattern());
    m_ui.borderSizeComboBox->setCurrentIndex(m_exception->borderSize());
    m_ui.hideTitleBar->setChecked(m_exception->hideTitleBar());

    // mask
    for (CheckBoxMap::iterator iter = m_checkboxes.begin(); iter != m_checkboxes.end(); ++iter) {
        iter.value()->setChecked(m_exception->mask() & iter.key());
    }

    setChanged(false);
}

//___________________________________________
void ExceptionDialog::save()
{
    m_exception->setExceptionType(m_ui.exceptionType->currentIndex());
    m_exception->setExceptionPattern(m_ui.exceptionEditor->text());
    m_exception->setBorderSize(m_ui.borderSizeComboBox->currentIndex());
    m_exception->setHideTitleBar(m_ui.hideTitleBar->isChecked());

    // mask
    unsigned int mask = None;
    for (CheckBoxMap::iterator iter = m_checkboxes.begin(); iter != m_checkboxes.end(); ++iter) {
        if (iter.value()->isChecked()) {
            mask |= iter.key();
        }
    }

    m_exception->setMask(mask);

    setChanged(false);
}

//___________________________________________
void ExceptionDialog::updateChanged()
{
    bool modified(false);
    if (m_exception->exceptionType() != m_ui.exceptionType->currentIndex()) {
        modified = true;
    } else if (m_exception->exceptionPattern() != m_ui.exceptionEditor->text()) {
        modified = true;
    } else if (m_exception->borderSize() != m_ui.borderSizeComboBox->currentIndex()) {
        modified = true;
    } else if (m_exception->hideTitleBar() != m_ui.hideTitleBar->isChecked()) {
        modified = true;
    } else {
        // check mask
        for (CheckBoxMap::iterator iter = m_checkboxes.begin(); iter != m_checkboxes.end(); ++iter) {
            if (iter.value()->isChecked() != (bool)(m_exception->mask() & iter.key())) {
                modified = true;
                break;
            }
        }
    }

    setChanged(modified);
}

//___________________________________________
void ExceptionDialog::selectWindowProperties()
{
    // create widget
    if (!m_detectDialog) {
        m_detectDialog = new DetectDialog(this);
        connect(m_detectDialog, &DetectDialog::detectionDone, this, &ExceptionDialog::readWindowProperties);
    }

    m_detectDialog->detect();
}

//___________________________________________
void ExceptionDialog::readWindowProperties(bool valid)
{
    Q_CHECK_PTR(m_detectDialog);
    if (valid) {
        // window info
        const QVariantMap properties = m_detectDialog->properties();

        switch (m_ui.exceptionType->currentIndex()) {
        default:
        case InternalSettings::ExceptionWindowClassName:
            m_ui.exceptionEditor->setText(properties.value(QStringLiteral("resourceClass")).toString());
            break;

        case InternalSettings::ExceptionWindowTitle:
            m_ui.exceptionEditor->setText(properties.value(QStringLiteral("caption")).toString());
            break;
        }
    }

    delete m_detectDialog;
    m_detectDialog = nullptr;
}

}

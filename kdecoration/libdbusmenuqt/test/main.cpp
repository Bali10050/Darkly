/*
    SPDX-FileCopyrightText: 2017 David Edmundson <davidedmundson@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QApplication>

#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QMainWindow>
#include <QMenuBar>

using namespace Qt::StringLiterals;

class MainWindow : public QMainWindow
{
public:
    MainWindow();
};

MainWindow::MainWindow()
    : QMainWindow()
{
    /*set an initial menu with the following
    Menu A
      - Item
      - Checkable Item
      - Item With Icon
      - A separator
      - Menu B
         - Item B1
     Menu C
      - DynamicItem ${timestamp}

      TopLevelItem
    */

    QAction *t;
    auto menuA = new QMenu(u"Menu A"_s, this);
    menuA->addAction(u"Item"_s);

    t = menuA->addAction(u"Checkable Item"_s);
    t->setCheckable(true);

    t = menuA->addAction(QIcon::fromTheme(u"document-edit"_s), u"Item with icon"_s);

    menuA->addSeparator();

    auto menuB = new QMenu(u"Menu B"_s, this);
    menuB->addAction(u"Item B1"_s);
    menuA->addMenu(menuB);

    menuBar()->addMenu(menuA);

    auto menuC = new QMenu(u"Menu C"_s, this);
    connect(menuC, &QMenu::aboutToShow, this, [menuC]() {
        menuC->clear();
        menuC->addAction(u"Dynamic Item " + QDateTime::currentDateTime().toString());
    });

    menuBar()->addMenu(menuC);

    menuBar()->addAction(u"Top Level Item"_s);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow mw;
    mw.show();
    return app.exec();
}

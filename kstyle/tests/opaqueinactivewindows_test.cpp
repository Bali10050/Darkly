/*
 * Unit tests for OpaqueInactiveWindows
 *
 * Translucent areas keep their configured opacity while their window is
 * active and become fully opaque while it is inactive, only when enabled.
 */

#include <QStandardPaths>
#include <QTest>
#include <QWidget>

#include "darklyopacity.h"

class OpaqueInactiveWindowsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void init();

    void testDefaultIsOff();
    void testDisabledKeepsOpacity();
    void testInactiveWindowIsOpaque();
    void testActiveWindowKeepsOpacity();
    void testNoWidgetKeepsOpacity();
};

void OpaqueInactiveWindowsTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
}

void OpaqueInactiveWindowsTest::init()
{
    Darkly::StyleConfigData::self()->setDefaults();
}

void OpaqueInactiveWindowsTest::testDefaultIsOff()
{
    QCOMPARE(Darkly::StyleConfigData::opaqueInactiveWindows(), false);
}

void OpaqueInactiveWindowsTest::testDisabledKeepsOpacity()
{
    QWidget window; // never shown, so never active
    QCOMPARE(Darkly::effectiveOpacity(75, &window), 75);
}

void OpaqueInactiveWindowsTest::testInactiveWindowIsOpaque()
{
    Darkly::StyleConfigData::setOpaqueInactiveWindows(true);
    QWidget window;
    QWidget child(&window);
    QCOMPARE(Darkly::effectiveOpacity(75, &window), 100);
    QCOMPARE(Darkly::effectiveOpacity(0, &child), 100);
}

void OpaqueInactiveWindowsTest::testActiveWindowKeepsOpacity()
{
    Darkly::StyleConfigData::setOpaqueInactiveWindows(true);
    QWidget window;
    QWidget child(&window);
    window.show();
    window.activateWindow();
    if (!QTest::qWaitForWindowActive(&window))
        QSKIP("window activation is not supported by this platform");
    QCOMPARE(Darkly::effectiveOpacity(75, &window), 75);
    QCOMPARE(Darkly::effectiveOpacity(75, &child), 75);
}

void OpaqueInactiveWindowsTest::testNoWidgetKeepsOpacity()
{
    Darkly::StyleConfigData::setOpaqueInactiveWindows(true);
    QCOMPARE(Darkly::effectiveOpacity(75, nullptr), 75);
}

QTEST_MAIN(OpaqueInactiveWindowsTest)
#include "opaqueinactivewindows_test.moc"

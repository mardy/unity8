/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "listviewwithpageheader.h"

#include <QAbstractItemModel>
#include <QQmlEngine>
#include <QQuickView>
#include <QtTestGui>
#include <private/qquicklistmodel_p.h>
#include <private/qquickanimation_p.h>
#include <private/qquickitem_p.h>

class ListViewWithPageHeaderTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
    }

    void init()
    {
        view = new QQuickView();
        view->engine()->addImportPath(BUILT_PLUGINS_DIR);
        view->setSource(QUrl::fromLocalFile(LISTVIEWWITHPAGEHEADER_FOLDER "/test.qml"));
        lvwph = dynamic_cast<ListViewWithPageHeader*>(view->rootObject()->findChild<QQuickFlickable*>());
        model = view->rootObject()->findChild<QQuickListModel*>();
        otherDelegate = view->rootObject()->findChild<QQmlComponent*>();
        QVERIFY(lvwph);
        QVERIFY(model);
        QVERIFY(otherDelegate);
        view->show();
        QTest::qWaitForWindowExposed(view);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 200., false);
        verifyItem(2, 400., 350., false);
        verifyItem(3, 750., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void cleanup()
    {
        delete view;
    }

    void verifyItem(int visibleIndex, qreal pos, qreal height, bool culled)
    {
        ListViewWithPageHeader::ListItem *item = lvwph->m_visibleItems[visibleIndex];
        QTRY_COMPARE(item->y(), pos);
        QTRY_COMPARE(item->height(), height);
        QCOMPARE(QQuickItemPrivate::get(item->m_item)->culled, culled);
    }

    void changeContentY(qreal change)
    {
        const qreal dest = lvwph->contentY() + change;
        if (dest > lvwph->contentY()) {
            const qreal jump = 25;
            while (lvwph->contentY() + jump < dest) {
                lvwph->setContentY(lvwph->contentY() + jump);
                QTest::qWait(0);
            }
        } else {
            const qreal jump = -25;
            while (lvwph->contentY() + jump > dest) {
                lvwph->setContentY(lvwph->contentY() + jump);
                QTest::qWait(0);
            }
        }
        lvwph->setContentY(dest);
        QTest::qWait(0);
    }

    void scrollToTop()
    {
        const qreal jump = -25;
        while (!lvwph->isAtYBeginning()) {
            if (lvwph->contentY() + jump > -lvwph->minYExtent()) {
                lvwph->setContentY(lvwph->contentY() + jump);
            } else {
                lvwph->setContentY(lvwph->contentY() - 1);
            }
            QTest::qWait(0);
        }
    }

    void scrollToBottom()
    {
        const qreal jump = 25;
        while (!lvwph->isAtYEnd()) {
            if (lvwph->contentY() + lvwph->height() + jump < lvwph->contentHeight()) {
                lvwph->setContentY(lvwph->contentY() + jump);
            } else {
                lvwph->setContentY(lvwph->contentY() + 1);
            }
            QTest::qWait(0);
        }
    }

    void testCreationDeletion()
    {
        // Nothing, init/cleanup already tests this
    }

    void testDrag1PixelUp()
    {
        lvwph->setContentY(lvwph->contentY() + 1);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 49., 150., false);
        verifyItem(1, 199., 200., false);
        verifyItem(2, 399., 350., false);
        verifyItem(3, 749., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testHeaderDetachDragDown()
    {
        QTest::mousePress(view, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0));
        QTest::qWait(100);
        QTest::mouseMove(view, QPoint(0, 5));
        QTest::qWait(100);
        QTest::mouseMove(view, QPoint(0, 10));
        QTest::qWait(100);
        QTest::mouseMove(view, QPoint(0, 15));
        QTest::qWait(100);
        QTest::mouseMove(view, QPoint(0, 20));
        QTest::qWait(100);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 55., 150., false);
        verifyItem(1, 205., 200., false);
        verifyItem(2, 405., 350., false);
        verifyItem(3, 755., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), -5.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), -5.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), -5.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        QTest::mouseRelease(view, Qt::LeftButton, Qt::NoModifier, QPoint(0, 15));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 200., false);
        verifyItem(2, 400., 350., false);
        verifyItem(3, 750., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testDrag375PixelUp()
    {
        changeContentY(375);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -325., 150., true);
        verifyItem(1, -175, 200., false);
        verifyItem(2, 25, 350., false);
        verifyItem(3, 375, 350., false);
        verifyItem(4, 725, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 375.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 375.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testDrag520PixelUp()
    {
        changeContentY(520);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 1);
        verifyItem(0, -320., 200., true);
        verifyItem(1, -120, 350., false);
        verifyItem(2, 230, 350., false);
        verifyItem(3, 580, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 520.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 520.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testDragHeaderUpThenShow()
    {
        changeContentY(120);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -70., 150., false);
        verifyItem(1, 80., 200., false);
        verifyItem(2, 280., 350., false);
        verifyItem(3, 630., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 120.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 120.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        changeContentY(-30);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -70., 150., false);
        verifyItem(1, 80., 200., false);
        verifyItem(2, 280., 350., false);
        verifyItem(3, 630., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 120.);
        QCOMPARE(lvwph->m_clipItem->clip(), true);
        QTRY_COMPARE(lvwph->m_headerItem->y(), 70.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 90.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 30.);
    }

    void testDragHeaderUpThenShowWithoutHidingTotally()
    {
        changeContentY(10);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 40., 150., false);
        verifyItem(1, 190., 200., false);
        verifyItem(2, 390., 350., false);
        verifyItem(3, 740., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 10.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 10.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        changeContentY(-1);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 41., 150., false);
        verifyItem(1, 191., 200., false);
        verifyItem(2, 391., 350., false);
        verifyItem(3, 741., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 9.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QTRY_COMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 9.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testPositionAtBeginningIndex0Visible()
    {
        changeContentY(375);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -325., 150., true);
        verifyItem(1, -175, 200., false);
        verifyItem(2, 25, 350., false);
        verifyItem(3, 375, 350., false);
        verifyItem(4, 725, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 375.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 375.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        lvwph->positionAtBeginning();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 200., false);
        verifyItem(2, 400., 350., false);
        verifyItem(3, 750., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testPositionAtBeginningIndex0NotVisible()
    {
        changeContentY(520);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 1);
        verifyItem(0, -320., 200., true);
        verifyItem(1, -120, 350., false);
        verifyItem(2, 230, 350., false);
        verifyItem(3, 580, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 520.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 520.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        lvwph->positionAtBeginning();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 200., false);
        verifyItem(2, 400., 350., false);
        verifyItem(3, 750., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testIndex0GrowOnScreen()
    {
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 200., false);
        verifyItem(2, 400., 350., false);
        verifyItem(3, 750., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        model->setProperty(0, "size", 400);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 400., false);
        verifyItem(1, 450., 200., false);
        verifyItem(2, 650., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testIndex0GrowOffScreen()
    {
        changeContentY(375);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -325., 150., true);
        verifyItem(1, -175, 200., false);
        verifyItem(2, 25, 350., false);
        verifyItem(3, 375, 350., false);
        verifyItem(4, 725, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 375.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 375.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        model->setProperty(0, "size", 400);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -575., 400., true);
        verifyItem(1, -175, 200., false);
        verifyItem(2, 25, 350., false);
        verifyItem(3, 375, 350., false);
        verifyItem(4, 725, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 250.);
        QCOMPARE(lvwph->m_clipItem->y(), 375.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 375.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        scrollToTop();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 400., false);
        verifyItem(1, 450, 200., false);
        verifyItem(2, 650, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 250.);
        QCOMPARE(lvwph->m_clipItem->y(), -250.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), -250.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), -250.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        changeContentY(30);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 20., 400., false);
        verifyItem(1, 420, 200., false);
        verifyItem(2, 620, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 250.);
        QCOMPARE(lvwph->m_clipItem->y(), -220.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), -250.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), -220.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testIndex0GrowNotCreated()
    {
        changeContentY(520);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 1);
        verifyItem(0, -320., 200., true);
        verifyItem(1, -120, 350., false);
        verifyItem(2, 230, 350., false);
        verifyItem(3, 580, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 520.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 520.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        model->setProperty(0, "size", 400);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 1);
        verifyItem(0, -320., 200., true);
        verifyItem(1, -120, 350., false);
        verifyItem(2, 230, 350., false);
        verifyItem(3, 580, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 520.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 520.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        scrollToTop();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 400., false);
        verifyItem(1, 450, 200., false);
        verifyItem(2, 650, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 250.);
        QCOMPARE(lvwph->m_clipItem->y(), -250.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), -250.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), -250.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testShowHideShowHeaderAtBottom()
    {
        scrollToBottom();
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        changeContentY(-30);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), true);
        QCOMPARE(lvwph->m_headerItem->y(), 1208.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1228.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 30.);

        changeContentY(30);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        changeContentY(-30);
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), true);
        QCOMPARE(lvwph->m_headerItem->y(), 1208.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1228.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 30.);
    }

    void testChangeDelegateAtBottom()
    {
        scrollToBottom();
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        lvwph->setDelegate(otherDelegate);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 6);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 35., false);
        verifyItem(1, 85, 35., false);
        verifyItem(2, 120, 35., false);
        verifyItem(3, 155, 35., false);
        verifyItem(4, 190, 35., false);
        verifyItem(5, 225, 35., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testGrowHeaderAtTop()
    {
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 200., false);
        verifyItem(2, 400., 350., false);
        verifyItem(3, 750., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        lvwph->m_headerItem->setHeight(100);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 100., 150., false);
        verifyItem(1, 250., 200., false);
        verifyItem(2, 450., 350., false);
        verifyItem(3, 800., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 100.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testGrowHeaderAtBottom()
    {
        scrollToBottom();
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        lvwph->m_headerItem->setHeight(100);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, -650.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 100.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        changeContentY(-30);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, -650.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), true);
        QCOMPARE(lvwph->m_headerItem->y(), 1158.);
        QCOMPARE(lvwph->m_headerItem->height(), 100.);
        QCOMPARE(lvwph->contentY(), 1228.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 30.);


        scrollToTop();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 100., 150., false);
        verifyItem(1, 250., 200., false);
        verifyItem(2, 450., 350., false);
        verifyItem(3, 800., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 50.);
        QCOMPARE(lvwph->m_clipItem->y(), -50.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), -50.);
        QCOMPARE(lvwph->m_headerItem->height(), 100.);
        QCOMPARE(lvwph->contentY(), -50.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testGrowHeaderPartlyShown()
    {
        scrollToBottom();
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        changeContentY(-30);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), true);
        QCOMPARE(lvwph->m_headerItem->y(), 1208.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1228.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 30.);

        lvwph->m_headerItem->setHeight(100);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, -700.);
        QCOMPARE(lvwph->m_clipItem->y(), 1308.);
        QCOMPARE(lvwph->m_clipItem->clip(), true);
        QCOMPARE(lvwph->m_headerItem->y(), 1208.);
        QCOMPARE(lvwph->m_headerItem->height(), 100.);
        QCOMPARE(lvwph->contentY(), 1228.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 80.);

        scrollToTop();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 100., 150., false);
        verifyItem(1, 250., 200., false);
        verifyItem(2, 450., 350., false);
        verifyItem(3, 800., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 100.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testShrinkHeaderMoreThanPartlyShown()
    {
        scrollToBottom();
        lvwph->m_headerItem->setHeight(100);
        changeContentY(-30);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, -650.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), true);
        QCOMPARE(lvwph->m_headerItem->y(), 1158.);
        QCOMPARE(lvwph->m_headerItem->height(), 100.);
        QCOMPARE(lvwph->contentY(), 1228.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 30.);

        lvwph->m_headerItem->setHeight(50);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, -670.);
        QCOMPARE(lvwph->m_clipItem->y(), 1228.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 1158.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1228.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
        QTRY_VERIFY(lvwph->isAtYEnd());

        scrollToTop();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 200., false);
        verifyItem(2, 400., 350., false);
        verifyItem(3, 750., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 30.);
        QCOMPARE(lvwph->m_clipItem->y(), -30.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), -30.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), -30.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testSetEmptyHeaderAtTop()
    {
        lvwph->setHeader(nullptr);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 0., 150., false);
        verifyItem(1, 150., 200., false);
        verifyItem(2, 350., 350., false);
        verifyItem(3, 700., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem, (QQuickItem*)nullptr);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testSetEmptyHeaderAtBottom()
    {
        scrollToBottom();
        lvwph->setHeader(nullptr);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, -750.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem, (QQuickItem*)nullptr);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        scrollToTop();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 0., 150., false);
        verifyItem(1, 150., 200., false);
        verifyItem(2, 350., 350., false);
        verifyItem(3, 700., 350., true);
        QCOMPARE(lvwph->m_minYExtent, -50.);
        QCOMPARE(lvwph->m_clipItem->y(), 50.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem, (QQuickItem*)nullptr);
        QCOMPARE(lvwph->contentY(), 50.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testSetEmptyHeaderWhenPartlyShownClipped()
    {
        scrollToBottom();
        changeContentY(-30);
        lvwph->setHeader(nullptr);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, -720.);
        QCOMPARE(lvwph->m_clipItem->y(), 1228.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem, (QQuickItem*)nullptr);
        QCOMPARE(lvwph->contentY(), 1228.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
        QTRY_VERIFY(lvwph->isAtYEnd());

        scrollToTop();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 0., 150., false);
        verifyItem(1, 150., 200., false);
        verifyItem(2, 350., 350., false);
        verifyItem(3, 700., 350., true);
        QCOMPARE(lvwph->m_minYExtent, -20.);
        QCOMPARE(lvwph->m_clipItem->y(), 20.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem, (QQuickItem*)nullptr);
        QCOMPARE(lvwph->contentY(), 20.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testSetEmptyHeaderWhenPartlyShownNotClipped()
    {
        changeContentY(30);
        lvwph->setHeader(nullptr);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -30., 150., false);
        verifyItem(1, 120., 200., false);
        verifyItem(2, 320., 350., false);
        verifyItem(3, 670., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 30.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem, (QQuickItem*)nullptr);
        QCOMPARE(lvwph->contentY(), 30.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testSetNullDelegate()
    {
        lvwph->setDelegate(nullptr);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 0);
        QCOMPARE(lvwph->m_firstVisibleIndex, -1);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
        QTRY_COMPARE(lvwph->contentHeight(), 50.);
        QVERIFY(lvwph->isAtYBeginning());
        QVERIFY(lvwph->isAtYEnd());
    }

    void testInsertItems()
    {
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 1), Q_ARG(QVariant, 100));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 1), Q_ARG(QVariant, 125));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 125., false);
        verifyItem(2, 325., 100., false);
        verifyItem(3, 425., 200., false);
        verifyItem(4, 625., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testInsertItemsOnNotShownPosition()
    {
        changeContentY(700);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QTRY_COMPARE(lvwph->m_firstVisibleIndex, 2);
        verifyItem(0, -300., 350., false);
        verifyItem(1, 50, 350., false);
        verifyItem(2, 400, 350., false);
        verifyItem(3, 750, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 700.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 700.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 1), Q_ARG(QVariant, 100));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 1), Q_ARG(QVariant, 125));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 4);
        verifyItem(0, -300., 350., false);
        verifyItem(1, 50, 350., false);
        verifyItem(2, 400, 350., false);
        verifyItem(3, 750, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 700.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 700.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        scrollToTop();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 125., false);
        verifyItem(2, 325., 100., false);
        verifyItem(3, 425., 200., false);
        verifyItem(4, 625., 350., true);
        QTRY_COMPARE(lvwph->m_minYExtent, 225.);
        QCOMPARE(lvwph->m_clipItem->y(), -225.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), -225.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), -225.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testInsertItemsAtEndOfViewport()
    {
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 3), Q_ARG(QVariant, 100));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 3), Q_ARG(QVariant, 125));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 200., false);
        verifyItem(2, 400., 350., false);
        verifyItem(3, 750., 125., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testInsertItemsBeforeValidIndex()
    {
        changeContentY(520);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 1);
        verifyItem(0, -320., 200., true);
        verifyItem(1, -120, 350., false);
        verifyItem(2, 230, 350., false);
        verifyItem(3, 580, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 520.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 520.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 1), Q_ARG(QVariant, 100));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 1), Q_ARG(QVariant, 125));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -320., 200., true);
        verifyItem(1, -120, 350., false);
        verifyItem(2, 230, 350., false);
        verifyItem(3, 580, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 520.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 520.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testInsertItemsBeforeViewport()
    {
        changeContentY(375);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -325., 150., true);
        verifyItem(1, -175, 200., false);
        verifyItem(2, 25, 350., false);
        verifyItem(3, 375, 350., false);
        verifyItem(4, 725, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 375.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 375.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 1), Q_ARG(QVariant, 100));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 1), Q_ARG(QVariant, 125));

        int i = 0;
        while(i < 10 && lvwph->m_visibleItems.count() != 5) {
            qDebug() << "Loop" << i;
            Q_FOREACH(ListViewWithPageHeader::ListItem *item, lvwph->m_visibleItems)
                qDebug() << item->m_item;
            QTest::qWait(1000);
        }
        qDebug() << "Before compare";
        Q_FOREACH(ListViewWithPageHeader::ListItem *item, lvwph->m_visibleItems)
            qDebug() << item->m_item;
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 2);
        verifyItem(0, -275., 100., true);
        verifyItem(1, -175, 200., false);
        verifyItem(2, 25, 350., false);
        verifyItem(3, 375, 350., false);
        verifyItem(4, 725, 350., true);
        QCOMPARE(lvwph->m_minYExtent, -50.);
        QCOMPARE(lvwph->m_clipItem->y(), 375.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 375.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        scrollToTop();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 125., false);
        verifyItem(2, 325., 100., false);
        verifyItem(3, 425., 200., false);
        verifyItem(4, 625., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 225.);
        QCOMPARE(lvwph->m_clipItem->y(), -225.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), -225.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), -225.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testInsertItemsAtBottom()
    {
        scrollToBottom();

        QVERIFY(lvwph->isAtYEnd());

        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 6), Q_ARG(QVariant, 100));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 6), Q_ARG(QVariant, 125));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        verifyItem(3, 542, 125., true);
        verifyItem(4, 667, 100., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
        QVERIFY(!lvwph->isAtYEnd());

        scrollToBottom();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 4);
        verifyItem(0, -383., 350., true);
        verifyItem(1, -33, 350., false);
        verifyItem(2, 317, 125., false);
        verifyItem(3, 442, 100., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1483.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1483.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
        QVERIFY(lvwph->isAtYEnd());
    }

    void testInsertItemAtTop()
    {
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 75., false);
        verifyItem(1, 125., 150., false);
        verifyItem(2, 275., 200., false);
        verifyItem(3, 475., 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
        QVERIFY(lvwph->isAtYBeginning());
    }

    void testInsertItem10SmallItemsAtTopWhenAtBottom()
    {
        scrollToBottom();

        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));
        QMetaObject::invokeMethod(model, "insertItem", Q_ARG(QVariant, 0), Q_ARG(QVariant, 75));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 13);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        changeContentY(-1700);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 12);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -308., 75., true);
        verifyItem(1, -233., 75., true);
        verifyItem(2, -158., 75., true);
        verifyItem(3, -83., 75., true);
        verifyItem(4, -8., 75., false);
        verifyItem(5, 67., 75., false);
        verifyItem(6, 142., 75., false);
        verifyItem(7, 217., 75., false);
        verifyItem(8, 292., 75., false);
        verifyItem(9, 367., 75., false);
        verifyItem(10, 442., 150., false);
        verifyItem(11, 592., 200., true);
        QCOMPARE(lvwph->m_minYExtent, 750.);
        QCOMPARE(lvwph->m_clipItem->y(), -392.);
        QCOMPARE(lvwph->m_clipItem->clip(), true);
        QCOMPARE(lvwph->m_headerItem->y(), -442.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), -442.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 50.);
    }

    void testRemoveItemsAtTop()
    {
        QMetaObject::invokeMethod(model, "removeItems", Q_ARG(QVariant, 0), Q_ARG(QVariant, 2));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 350., false);
        verifyItem(1, 400., 350., false);
        verifyItem(2, 750., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
        QVERIFY(lvwph->isAtYBeginning());
    }

    void testRemoveNonCreatedItemsAtTopWhenAtBottom()
    {
        scrollToBottom();

        QMetaObject::invokeMethod(model, "removeItems", Q_ARG(QVariant, 0), Q_ARG(QVariant, 2));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 1);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testRemoveLastItemsAtBottom()
    {
        scrollToBottom();

        QMetaObject::invokeMethod(model, "removeItems", Q_ARG(QVariant, 4), Q_ARG(QVariant, 2));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 1);
        verifyItem(0, -358., 200., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, -150.);
        QCOMPARE(lvwph->m_clipItem->y(), 558.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 558.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testRemoveItemOutOfViewport()
    {
        changeContentY(520);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 1);
        verifyItem(0, -320., 200., true);
        verifyItem(1, -120, 350., false);
        verifyItem(2, 230, 350., false);
        verifyItem(3, 580, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 520.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 520.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);


        QMetaObject::invokeMethod(model, "removeItems", Q_ARG(QVariant, 1), Q_ARG(QVariant, 1));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -270., 150., true);
        verifyItem(1, -120., 350., false);
        verifyItem(2, 230, 350., false);
        verifyItem(3, 580, 350., true);
        QCOMPARE(lvwph->m_minYExtent, -200.);
        QCOMPARE(lvwph->m_clipItem->y(), 520.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 520.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testMoveFirstItems()
    {
        QMetaObject::invokeMethod(model, "moveItems", Q_ARG(QVariant, 0), Q_ARG(QVariant, 1), Q_ARG(QVariant, 1));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 200., false);
        verifyItem(1, 250., 150., false);
        verifyItem(2, 400., 350., false);
        verifyItem(3, 750., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

    }

    void testMoveFirstOutOfVisibleItems()
    {
        QMetaObject::invokeMethod(model, "moveItems", Q_ARG(QVariant, 0), Q_ARG(QVariant, 4), Q_ARG(QVariant, 1));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 200., false);
        verifyItem(1, 250., 350., false);
        verifyItem(2, 600., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 0.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 0.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

    }

    void testMoveFirstToLastAtBottom()
    {
        scrollToBottom();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 3);
        QCOMPARE(lvwph->m_firstVisibleIndex, 3);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        QMetaObject::invokeMethod(model, "moveItems", Q_ARG(QVariant, 0), Q_ARG(QVariant, 5), Q_ARG(QVariant, 1));

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 2);
        verifyItem(0, -508., 350., true);
        verifyItem(1, -158, 350., false);
        verifyItem(2, 192, 350., false);
        verifyItem(3, 542, 150., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 1258.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 1258.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
        QVERIFY(!lvwph->isAtYEnd());
    }

    void testChangeSizeVisibleItemNotOnViewport()
    {
        changeContentY(440);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -390., 150., true);
        verifyItem(1, -240., 200., true);
        verifyItem(2, -40, 350., false);
        verifyItem(3, 310, 350., false);
        verifyItem(4, 660, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 440.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 440.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);

        model->setProperty(1, "size", 100);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -290., 150., true);
        verifyItem(1, -140., 100., true);
        verifyItem(2, -40, 350., false);
        verifyItem(3, 310, 350., false);
        verifyItem(4, 660, 350., true);
        QCOMPARE(lvwph->m_minYExtent, -100.);
        QCOMPARE(lvwph->m_clipItem->y(), 440.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 440.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

    void testShowHeaderCloseToTheTop()
    {
        changeContentY(375);

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -325., 150., true);
        verifyItem(1, -175, 200., false);
        verifyItem(2, 25, 350., false);
        verifyItem(3, 375, 350., false);
        verifyItem(4, 725, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 0.);
        QCOMPARE(lvwph->m_clipItem->y(), 375.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), 0.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 375.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);;

        lvwph->showHeader();

        QTRY_VERIFY(!lvwph->m_headerShowAnimation->isRunning());
        QTRY_COMPARE(lvwph->m_visibleItems.count(), 5);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, -375., 150., true);
        verifyItem(1, -225, 200., true);
        verifyItem(2, -25, 350., false);
        verifyItem(3, 325, 350., false);
        verifyItem(4, 675, 350., true);
        QCOMPARE(lvwph->m_minYExtent, 50.);
        QCOMPARE(lvwph->m_clipItem->y(), 375.);
        QCOMPARE(lvwph->m_clipItem->clip(), true);
        QCOMPARE(lvwph->m_headerItem->y(), 325.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), 325.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 50.);;

        scrollToTop();

        QTRY_COMPARE(lvwph->m_visibleItems.count(), 4);
        QCOMPARE(lvwph->m_firstVisibleIndex, 0);
        verifyItem(0, 50., 150., false);
        verifyItem(1, 200., 200., false);
        verifyItem(2, 400., 350., false);
        verifyItem(3, 750., 350., true);
        QCOMPARE(lvwph->m_minYExtent, 50.);
        QCOMPARE(lvwph->m_clipItem->y(), -50.);
        QCOMPARE(lvwph->m_clipItem->clip(), false);
        QCOMPARE(lvwph->m_headerItem->y(), -50.);
        QCOMPARE(lvwph->m_headerItem->height(), 50.);
        QCOMPARE(lvwph->contentY(), -50.);
        QCOMPARE(lvwph->m_headerItemShownHeight, 0.);
    }

private:
    QQuickView *view;
    ListViewWithPageHeader *lvwph;
    QQuickListModel *model;
    QQmlComponent *otherDelegate;
};

QTEST_MAIN(ListViewWithPageHeaderTest)

#include "listviewwithpageheadertest.moc"

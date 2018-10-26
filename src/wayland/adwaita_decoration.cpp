/****************************************************************************
**
** Copyright (C) 2016 Robin Burchell <robin.burchell@viroteck.net>
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "adwaita_decoration.h"

#include <QtGui/QCursor>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainter>
#include <QtGui/QPalette>

namespace QtWaylandClient
{
AdwaitaDecoration::AdwaitaDecoration() {}

QRectF AdwaitaDecoration::closeButtonRect() const
{
	return m_style.closeButtonRect(
	    QRect(margins().left() - 1, 0,
	          window()->frameGeometry().width() - margins().right() + 1,
	          m_style.height()));
}

QRectF AdwaitaDecoration::maximizeButtonRect() const
{
	return closeButtonRect().translated(-30, 0);
}

QRectF AdwaitaDecoration::minimizeButtonRect() const
{
	return closeButtonRect().translated(-30, 0);
}

QMargins AdwaitaDecoration::margins() const
{
	return QMargins(3, m_style.height(), 3, 3);
}

void AdwaitaDecoration::paint(QPaintDevice *device)
{
	QRect surface_rect(QPoint(), window()->frameGeometry().size());
	QRect titlebar_rect(
	    margins().left() - 1, 0,
	    surface_rect.width() - (margins().left() - 1) - (margins().right() - 1),
	    margins().top());
	QRect left_border_rect(
	    0, titlebar_rect.height(), margins().left(),
	    surface_rect.height() - titlebar_rect.height() - margins().bottom());
	QRect right_border_rect(
	    surface_rect.width() - margins().right(), titlebar_rect.height(),
	    margins().right(),
	    surface_rect.height() - titlebar_rect.height() - margins().bottom());
	QRect bottom_border_rect(titlebar_rect.x(),
	                         surface_rect.height() - margins().bottom(),
	                         titlebar_rect.width(), margins().bottom());

	QPainter painter(device);
	painter.setRenderHint(QPainter::Antialiasing);

	m_style.drawBackground(&painter, DecorationStyle::Mode::ACTIVE,
	                       titlebar_rect);
	m_style.drawTitle(&painter, DecorationStyle::Mode::ACTIVE, titlebar_rect,
	                  window()->title());
	m_style.drawCloseButton(&painter, DecorationStyle::Mode::ACTIVE,
	                        titlebar_rect);
	// painter.restore();
	m_style.drawLeftBorder(&painter, DecorationStyle::Mode::ACTIVE,
	                       left_border_rect);
	m_style.drawRightBorder(&painter, DecorationStyle::Mode::ACTIVE,
	                        right_border_rect);

	m_style.drawBottomBorder(&painter, DecorationStyle::Mode::ACTIVE,
	                         bottom_border_rect);
}

bool AdwaitaDecoration::clickButton(Qt::MouseButtons b, Button btn)
{
	if (isLeftClicked(b)) {
		m_clicking = btn;
		return false;
	} else if (isLeftReleased(b)) {
		if (m_clicking == btn) {
			m_clicking = None;
			return true;
		} else {
			m_clicking = None;
		}
	}
	return false;
}

bool AdwaitaDecoration::handleMouse(QWaylandInputDevice *inputDevice,
                                    const QPointF &local, const QPointF &global,
                                    Qt::MouseButtons b,
                                    Qt::KeyboardModifiers mods)

{
	Q_UNUSED(global);

	// Figure out what area mouse is in
	if (closeButtonRect().contains(local)) {
		if (clickButton(b, Close))
			QWindowSystemInterface::handleCloseEvent(window());
	} else if (maximizeButtonRect().contains(local)) {
		if (clickButton(b, Maximize))
			window()->setWindowState(waylandWindow()->isMaximized()
			                             ? Qt::WindowNoState
			                             : Qt::WindowMaximized);
	} else if (minimizeButtonRect().contains(local)) {
		if (clickButton(b, Minimize))
			window()->setWindowState(Qt::WindowMinimized);
	} else if (local.y() <= margins().top()) {
		processMouseTop(inputDevice, local, b, mods);
	} else if (local.y() > window()->height() + margins().top()) {
		processMouseBottom(inputDevice, local, b, mods);
	} else if (local.x() <= margins().left()) {
		processMouseLeft(inputDevice, local, b, mods);
	} else if (local.x() > window()->width() + margins().left()) {
		processMouseRight(inputDevice, local, b, mods);
	} else {
#if QT_CONFIG(cursor)
		waylandWindow()->restoreMouseCursor(inputDevice);
#endif
		setMouseButtons(b);
		return false;
	}

	setMouseButtons(b);
	return true;
}

bool AdwaitaDecoration::handleTouch(QWaylandInputDevice *inputDevice,
                                    const QPointF &local, const QPointF &global,
                                    Qt::TouchPointState state,
                                    Qt::KeyboardModifiers mods)
{
	Q_UNUSED(inputDevice);
	Q_UNUSED(global);
	Q_UNUSED(mods);
	bool handled = state == Qt::TouchPointPressed;
	if (handled) {
		if (closeButtonRect().contains(local))
			QWindowSystemInterface::handleCloseEvent(window());
		else if (maximizeButtonRect().contains(local))
			window()->setWindowState(waylandWindow()->isMaximized()
			                             ? Qt::WindowNoState
			                             : Qt::WindowMaximized);
		else if (minimizeButtonRect().contains(local))
			window()->setWindowState(Qt::WindowMinimized);
		else if (local.y() <= margins().top())
			waylandWindow()->shellSurface()->move(inputDevice);
		else
			handled = false;
	}

	return handled;
}

void AdwaitaDecoration::processMouseTop(QWaylandInputDevice *inputDevice,
                                        const QPointF &local,
                                        Qt::MouseButtons b,
                                        Qt::KeyboardModifiers mods)
{
	Q_UNUSED(mods);
	if (local.y() <= margins().bottom()) {
		if (local.x() <= margins().left()) {
			// top left bit
#if QT_CONFIG(cursor)
			waylandWindow()->setMouseCursor(inputDevice, Qt::SizeFDiagCursor);
#endif
			startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_TOP_LEFT, b);
		} else if (local.x() > window()->width() + margins().left()) {
			// top right bit
#if QT_CONFIG(cursor)
			waylandWindow()->setMouseCursor(inputDevice, Qt::SizeBDiagCursor);
#endif
			startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_TOP_RIGHT, b);
		} else {
			// top reszie bit
#if QT_CONFIG(cursor)
			waylandWindow()->setMouseCursor(inputDevice, Qt::SplitVCursor);
#endif
			startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_TOP, b);
		}
	} else {
#if QT_CONFIG(cursor)
		waylandWindow()->restoreMouseCursor(inputDevice);
#endif
		startMove(inputDevice, b);
	}
}

void AdwaitaDecoration::processMouseBottom(QWaylandInputDevice *inputDevice,
                                           const QPointF &local,
                                           Qt::MouseButtons b,
                                           Qt::KeyboardModifiers mods)
{
	Q_UNUSED(mods);
	if (local.x() <= margins().left()) {
		// bottom left bit
#if QT_CONFIG(cursor)
		waylandWindow()->setMouseCursor(inputDevice, Qt::SizeBDiagCursor);
#endif
		startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_BOTTOM_LEFT, b);
	} else if (local.x() > window()->width() + margins().left()) {
		// bottom right bit
#if QT_CONFIG(cursor)
		waylandWindow()->setMouseCursor(inputDevice, Qt::SizeFDiagCursor);
#endif
		startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_BOTTOM_RIGHT, b);
	} else {
		// bottom bit
#if QT_CONFIG(cursor)
		waylandWindow()->setMouseCursor(inputDevice, Qt::SplitVCursor);
#endif
		startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_BOTTOM, b);
	}
}

void AdwaitaDecoration::processMouseLeft(QWaylandInputDevice *inputDevice,
                                         const QPointF &local,
                                         Qt::MouseButtons b,
                                         Qt::KeyboardModifiers mods)
{
	Q_UNUSED(local);
	Q_UNUSED(mods);
#if QT_CONFIG(cursor)
	waylandWindow()->setMouseCursor(inputDevice, Qt::SplitHCursor);
#endif
	startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_LEFT, b);
}

void AdwaitaDecoration::processMouseRight(QWaylandInputDevice *inputDevice,
                                          const QPointF &local,
                                          Qt::MouseButtons b,
                                          Qt::KeyboardModifiers mods)
{
	Q_UNUSED(local);
	Q_UNUSED(mods);
#if QT_CONFIG(cursor)
	waylandWindow()->setMouseCursor(inputDevice, Qt::SplitHCursor);
#endif
	startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_RIGHT, b);
}

QT_END_NAMESPACE
};  // namespace QtWaylandClient

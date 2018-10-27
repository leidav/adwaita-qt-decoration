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
	return m_style.maximizeButtonRect(
	    QRect(margins().left() - 1, 0,
	          window()->frameGeometry().width() - margins().right() + 1,
	          m_style.height()));
}

QRectF AdwaitaDecoration::minimizeButtonRect() const
{
	return m_style.minimizeButtonRect(
	    QRect(margins().left() - 1, 0,
	          window()->frameGeometry().width() - margins().right() + 1,
	          m_style.height()));
}

QMargins AdwaitaDecoration::margins() const
{
	int margin;
	if (waylandWindow()->isMaximized()) {
		margin = 0;
	} else {
		margin = 3;
	}
	return QMargins(margin, m_style.height(), margin, margin);
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

	auto mode = DecorationStyle::State::INACTIVE;

	if (window()->isActive()) {
		mode = DecorationStyle::State::ACTIVE;
	}

	QPainter painter(device);
	painter.setRenderHint(QPainter::Antialiasing);

	m_style.drawBackground(&painter, mode, titlebar_rect);
	m_style.drawTitle(&painter, mode, titlebar_rect, window()->title());

	auto close_button_mode = DecorationStyle::State::ACTIVE;
	auto minimize_button_mode = DecorationStyle::State::ACTIVE;
	auto maximize_button_mode = DecorationStyle::State::ACTIVE;

	if (mode == DecorationStyle::State::ACTIVE) {
		if (m_hover == Close) {
			close_button_mode = DecorationStyle::State::HOVER;
		} else if (m_hover == Minimize) {
			minimize_button_mode = DecorationStyle::State::HOVER;
		} else if (m_hover == Maximize) {
			maximize_button_mode = DecorationStyle::State::HOVER;
		}
		if (m_clicking == Close) {
			close_button_mode = DecorationStyle::State::PRESS;
		} else if (m_clicking == Minimize) {
			minimize_button_mode = DecorationStyle::State::PRESS;
		} else if (m_clicking == Maximize) {
			maximize_button_mode = DecorationStyle::State::PRESS;
		}
	} else {
		close_button_mode = minimize_button_mode = maximize_button_mode =
		    DecorationStyle::State::INACTIVE;
	}

	m_style.drawCloseButton(&painter, close_button_mode, titlebar_rect);
	m_style.drawMaximizeButton(&painter, maximize_button_mode, titlebar_rect);
	m_style.drawMinimizeButton(&painter, minimize_button_mode, titlebar_rect);

	m_style.drawLeftBorder(&painter, mode, left_border_rect);
	m_style.drawRightBorder(&painter, mode, right_border_rect);
	m_style.drawBottomBorder(&painter, mode, bottom_border_rect);
}

bool AdwaitaDecoration::clickButton(Qt::MouseButtons b, Button btn)
{
	bool release = false;
	if (isLeftClicked(b)) {
		m_clicking = btn;
	} else if (isLeftReleased(b)) {
		if (m_clicking == btn) {
			m_clicking = None;
			release = true;
		} else {
			m_clicking = None;
		}
	} else {
		m_hover = btn;
	}
	return release;
}

bool AdwaitaDecoration::handleMouse(QWaylandInputDevice *inputDevice,
                                    const QPointF &local, const QPointF &global,
                                    Qt::MouseButtons b,
                                    Qt::KeyboardModifiers mods)
{
	Q_UNUSED(global);

	bool handled = false;
	Button hover_old = m_hover;
	Button clicking_old = m_clicking;
	// Figure out what area mouse is in
	if (closeButtonRect().contains(local)) {
		if (clickButton(b, Close)) {
			QWindowSystemInterface::handleCloseEvent(window());
		}
		handled = true;
	} else if (maximizeButtonRect().contains(local)) {
		if (clickButton(b, Maximize)) {
			window()->setWindowState(waylandWindow()->isMaximized()
			                             ? Qt::WindowNoState
			                             : Qt::WindowMaximized);
		}
		handled = true;
	} else if (minimizeButtonRect().contains(local)) {
		if (clickButton(b, Minimize)) {
			window()->setWindowState(Qt::WindowMinimized);
		}
		handled = true;
	} else if (local.y() <= margins().top()) {
		processMouseTop(inputDevice, local, b, mods);
		handled = true;
		m_hover = None;
		m_clicking = None;
	} else if (local.y() > window()->height() + margins().top()) {
		processMouseBottom(inputDevice, local, b, mods);
		handled = true;
		m_hover = None;
		m_clicking = None;
	} else if (local.x() <= margins().left()) {
		processMouseLeft(inputDevice, local, b, mods);
		handled = true;
		m_hover = None;
		m_clicking = None;
	} else if (local.x() > window()->width() + margins().left()) {
		processMouseRight(inputDevice, local, b, mods);
		handled = true;
		m_hover = None;
		m_clicking = None;
	} else {
#if QT_CONFIG(cursor)
		waylandWindow()->restoreMouseCursor(inputDevice);
#endif
		m_hover = None;
		m_clicking = None;
	}

	setMouseButtons(b);
	update();
	if ((hover_old != m_hover) || (clicking_old != m_clicking)) {
		waylandWindow()->requestUpdate();
	}
	return handled;
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

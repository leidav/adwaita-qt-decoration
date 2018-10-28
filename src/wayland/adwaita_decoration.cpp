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
	return m_style.closeButtonRect(titlebarRect().toRect());
}
QRectF AdwaitaDecoration::maximizeButtonRect() const
{
	return m_style.maximizeButtonRect(titlebarRect().toRect());
}
QRectF AdwaitaDecoration::minimizeButtonRect() const
{
	return m_style.minimizeButtonRect(titlebarRect().toRect());
}
QRectF AdwaitaDecoration::leftBorderRect() const
{
	return QRectF(
	    0, margins().top(), margins().left(),
	    m_surface_rect.height() - margins().top() - margins().bottom());
}

QRectF AdwaitaDecoration::rightBorderRect() const
{
	return QRectF(
	    m_surface_rect.width() - margins().right(), margins().top(),
	    margins().right(),
	    m_surface_rect.height() - margins().top() - margins().bottom());
}

QRectF AdwaitaDecoration::bottomBorderRect() const
{
	return QRectF(titlebarRect().x(),
	              m_surface_rect.height() - margins().bottom(),
	              titlebarRect().width(), margins().bottom());
}

QRectF AdwaitaDecoration::titlebarRect() const
{
	return QRectF(margins().left() - 1, margins().top() - m_style.height(),
	              m_surface_rect.width() - (margins().left() - 1) -
	                  (margins().right() - 1),
	              m_style.height());
}
QRectF AdwaitaDecoration::topLeftCornerRect() const
{
	return QRectF(0, 0, margins().left() + titlebarRect().height() / 2,
	              margins().top() - titlebarRect().height() / 2);
}

QRectF AdwaitaDecoration::topRightCornerRect() const
{
	return QRectF(
	    window()->width() + margins().left() - titlebarRect().height() / 2, 0,
	    margins().right() + titlebarRect().height() / 2,
	    margins().top() - titlebarRect().height() / 2);
}

QRectF AdwaitaDecoration::bottomLeftCornerRect() const
{
	return QRectF(
	    0, window()->height() + margins().top() - titlebarRect().height() / 2,
	    margins().left() + titlebarRect().height() / 2,
	    margins().top() - titlebarRect().height() / 2);
}

QRectF AdwaitaDecoration::bottomRightCornerRect() const
{
	return QRectF(
	    window()->width() + margins().left() - titlebarRect().height() / 2,
	    window()->height() + margins().top() - titlebarRect().height() / 2,
	    margins().right() + titlebarRect().height() / 2,
	    margins().top() - titlebarRect().height() / 2);
}

QMargins AdwaitaDecoration::margins() const
{
	int margin = waylandWindow()->isMaximized() ? 16 : 16;
	return QMargins(margin, margin + m_style.height(), margin, margin);
}

void AdwaitaDecoration::paint(QPaintDevice *device)
{
	m_surface_rect =
	    QRect(QPoint(window()->frameGeometry().x(), window()->geometry().y()),
	          window()->frameGeometry().size());
	auto mode = DecorationStyle::State::INACTIVE;

	if (window()->isActive()) {
		mode = DecorationStyle::State::ACTIVE;
	}

	QPainter painter(device);
	painter.setRenderHint(QPainter::Antialiasing);

	m_style.drawBackground(&painter, mode, titlebarRect().toRect());
	m_style.drawTitle(&painter, mode, titlebarRect().toRect(),
	                  window()->title());

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

	m_style.drawCloseButton(&painter, close_button_mode,
	                        titlebarRect().toRect());
	m_style.drawMaximizeButton(&painter, maximize_button_mode,
	                           titlebarRect().toRect(),
	                           waylandWindow()->isMaximized());
	m_style.drawMinimizeButton(&painter, minimize_button_mode,
	                           titlebarRect().toRect());

	m_style.drawLeftBorder(&painter, mode, leftBorderRect().toRect());
	m_style.drawRightBorder(&painter, mode, rightBorderRect().toRect());
	m_style.drawBottomBorder(&painter, mode, bottomBorderRect().toRect());
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

	if (handleWindowButtons(inputDevice, local, b, mods)) {
		handled = true;
	} else {
		m_hover = None;
		m_clicking = None;
		handled = moveAndResizeWindow(inputDevice, local, b, mods);
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
		else if (maximizeButtonRect().contains(local)) {
			window()->setWindowState(waylandWindow()->isMaximized()
			                             ? Qt::WindowNoState
			                             : Qt::WindowMaximized);
		} else if (minimizeButtonRect().contains(local)) {
			window()->setWindowState(Qt::WindowMinimized);
		} else if (local.y() <= margins().top()) {
			waylandWindow()->shellSurface()->move(inputDevice);
		} else {
			handled = false;
		}
	}
	return handled;
}

bool AdwaitaDecoration::moveAndResizeWindow(QWaylandInputDevice *inputDevice,
                                            const QPointF &local,
                                            Qt::MouseButtons b,
                                            Qt::KeyboardModifiers mods)
{
	bool handled = false;
	if (titlebarRect().contains(local)) {
		waylandWindow()->restoreMouseCursor(inputDevice);
		startMove(inputDevice, b);
		handled = true;
	} else if (topLeftCornerRect().contains(local)) {
		waylandWindow()->setMouseCursor(inputDevice, Qt::SizeFDiagCursor);
		startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_TOP_LEFT, b);
		handled = true;
	} else if (topRightCornerRect().contains(local)) {
		waylandWindow()->setMouseCursor(inputDevice, Qt::SizeBDiagCursor);
		startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_TOP_RIGHT, b);
		handled = true;
	} else if (bottomLeftCornerRect().contains(local)) {
		waylandWindow()->setMouseCursor(inputDevice, Qt::SizeBDiagCursor);
		startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_BOTTOM_LEFT, b);
		handled = true;
	} else if (bottomRightCornerRect().contains(local)) {
		waylandWindow()->setMouseCursor(inputDevice, Qt::SizeFDiagCursor);
		startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_BOTTOM_RIGHT, b);
		handled = true;
	} else if (local.x() <= margins().left()) {
		waylandWindow()->setMouseCursor(inputDevice, Qt::SplitHCursor);
		startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_LEFT, b);
		handled = true;
	} else if (local.x() >= window()->width() + margins().left()) {
		waylandWindow()->setMouseCursor(inputDevice, Qt::SplitHCursor);
		startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_RIGHT, b);
		handled = true;
	} else {
		if (local.y() <= (margins().top() - titlebarRect().height())) {
			waylandWindow()->setMouseCursor(inputDevice, Qt::SplitVCursor);
			startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_TOP, b);
			handled = true;
		} else if (local.y() >= window()->height() + margins().top()) {
			waylandWindow()->setMouseCursor(inputDevice, Qt::SplitVCursor);
			startResize(inputDevice, WL_SHELL_SURFACE_RESIZE_BOTTOM, b);
			handled = true;
		} else {
			waylandWindow()->restoreMouseCursor(inputDevice);
		}
	}
	return handled;
}

bool AdwaitaDecoration::handleWindowButtons(QWaylandInputDevice *inputDevice,
                                            const QPointF &local,
                                            Qt::MouseButtons b,
                                            Qt::KeyboardModifiers mods)
{
	bool handled = false;
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
	}
	return handled;
}

QT_END_NAMESPACE
};  // namespace QtWaylandClient

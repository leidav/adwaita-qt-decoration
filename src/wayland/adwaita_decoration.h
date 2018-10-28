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

#ifndef ADWAITA_DECORATION_H
#define ADWAITA_DECORATION_H

#include <adwaita_decoration_style.h>

#include <qpa/qwindowsysteminterface.h>

#include <QtWaylandClient/private/qwaylandabstractdecoration_p.h>
#include <QtWaylandClient/private/qwaylanddecorationplugin_p.h>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

namespace QtWaylandClient
{
enum Button { None, Close, Maximize, Minimize };

QT_BEGIN_NAMESPACE
class Q_WAYLAND_CLIENT_EXPORT AdwaitaDecoration
    : public QWaylandAbstractDecoration
{
public:
	AdwaitaDecoration();

protected:
	QMargins margins() const override;
	void paint(QPaintDevice *device) override;
	bool handleMouse(QWaylandInputDevice *inputDevice, const QPointF &local,
	                 const QPointF &global, Qt::MouseButtons b,
	                 Qt::KeyboardModifiers mods) override;
	bool handleTouch(QWaylandInputDevice *inputDevice, const QPointF &local,
	                 const QPointF &global, Qt::TouchPointState state,
	                 Qt::KeyboardModifiers mods) override;

private:
	bool moveAndResizeWindow(QWaylandInputDevice *inputDevice,
	                         const QPointF &local, Qt::MouseButtons b,
	                         Qt::KeyboardModifiers mods);
	bool handleWindowButtons(QWaylandInputDevice *inputDevice,
	                         const QPointF &local, Qt::MouseButtons b,
	                         Qt::KeyboardModifiers mods);

	bool clickButton(Qt::MouseButtons b, Button btn);

	QRectF topLeftCornerRect() const;
	QRectF topRightCornerRect() const;
	QRectF bottomLeftCornerRect() const;
	QRectF bottomRightCornerRect() const;

	QRectF titlebarRect() const;
	QRectF leftBorderRect() const;
	QRectF rightBorderRect() const;
	QRectF bottomBorderRect() const;

	QRectF closeButtonRect() const;
	QRectF maximizeButtonRect() const;
	QRectF minimizeButtonRect() const;

	Button m_clicking = None;
	Button m_hover = None;
	QRect m_surface_rect;
	AdwaitaDecorationStyle m_style;
};
};  // namespace QtWaylandClient
QT_END_NAMESPACE
#endif

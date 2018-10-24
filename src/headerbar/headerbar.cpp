#include "headerbar.h"
#include <adwaita_decoration_style.h>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

HeaderBar::HeaderBar(QWidget *parent)
    : HeaderBar(new AdwaitaDecorationStyle(), parent)
{
}
HeaderBar::HeaderBar(DecorationStyle *style, QWidget *parent)
    : QWidget(parent),
      m_style(style),
      m_button_hover(false),
      m_button_press(false)
{
	setMinimumHeight(m_style->height());
	setMaximumHeight(m_style->height());
	setMouseTracking(true);
}

void HeaderBar::paintEvent(QPaintEvent *event)
{
	auto mode = DecorationStyle::Mode::INACTIVE;
	if (window()->isActiveWindow()) {
		mode = DecorationStyle::Mode::ACTIVE;
	}

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	QRect rect(0, 0, geometry().width(), geometry().height());
	m_style->drawBackground(&painter, mode, rect);
	m_style->drawTitle(&painter, mode, rect, "TitleBar");
	if (mode == DecorationStyle::Mode::ACTIVE) {
		if (m_button_press) {
			mode = DecorationStyle::Mode::PRESS;

		} else if (m_button_hover) {
			mode = DecorationStyle::Mode::HOVER;
		}
	}
	m_style->drawCloseButton(&painter, mode, rect);
}

void HeaderBar::mouseMoveEvent(QMouseEvent *event)
{
	QRect button_rect = m_style->closeButtonRect(
	    QRect(0, 0, geometry().width(), geometry().height()));
	bool hover = false;
	if (button_rect.contains(event->pos(), true)) {
		hover = true;
	} else {
		hover = false;
		m_button_press = false;
	}
	if (m_button_hover != hover) {
		m_button_hover = hover;
		repaint();
	}
}

void HeaderBar::mousePressEvent(QMouseEvent *event)
{
	QRect button_rect = m_style->closeButtonRect(
	    QRect(0, 0, geometry().width(), geometry().height()));
	if (button_rect.contains(event->pos(), true)) {
		m_button_press = true;
		repaint();
	}
}

void HeaderBar::mouseReleaseEvent(QMouseEvent *event)
{
	m_button_press = false;
	repaint();
}

void HeaderBar::leaveEvent(QEvent *event)
{
	m_button_hover = false;
	m_button_press = false;
	repaint();
}

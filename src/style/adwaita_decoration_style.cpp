#include "adwaita_decoration_style.h"
#include <QIcon>

AdwaitaDecorationStyle::AdwaitaDecorationStyle()
    : m_height(min_height_with_border),
      m_font("Cantarell", font_size, QFont::Bold, false),
      m_gradient_stops(),
      m_button_gradient_stops()
{
	QIcon::setThemeName("Adwaita");
	m_close_button_icon = QIcon::fromTheme("window-close-symbolic");
	m_minimize_button_icon = QIcon::fromTheme("window-minimize-symbolic");
	m_maximize_button_icon = QIcon::fromTheme("window-maximize-symbolic");

	m_gradient_stops.append(QGradientStop(0.0, headerbar_color));
	m_gradient_stops.append(QGradientStop(0.9, headerbar_color));
	m_gradient_stops.append(QGradientStop(1.0, darker(headerbar_color, 13)));

	m_button_gradient_stops.append(
	    QGradientStop(0.0, button_gradient_start_color));
	m_button_gradient_stops.append(
	    QGradientStop(1.0, button_gradient_stop_color));

	QFontMetrics metric(m_font);
	m_height = qMax(metric.height() + static_cast<int>(font_padding_y) * 2,
	                static_cast<int>(min_height_with_border));
}

int AdwaitaDecorationStyle::height() const
{
	return m_height;
}

QFont AdwaitaDecorationStyle::titleFont() const
{
	return m_font;
}

QRect AdwaitaDecorationStyle::closeButtonRect(const QRect &rect) const
{
	int size = buttonSize(rect);
	return QRect(rect.x() + rect.width() - 1 - size - button_padding,
	             rect.y() + rect.height() - 1 - size - button_padding, size,
	             size);
}

QRect AdwaitaDecorationStyle::maximizeButtonRect(const QRect &rect) const
{
	QRect r = closeButtonRect(rect);
	return r.translated(-(r.width() + button_padding), 0);
}

QRect AdwaitaDecorationStyle::minimizeButtonRect(const QRect &rect) const
{
	QRect r = closeButtonRect(rect);
	return r.translated(-(r.width() + button_padding) * 2, 0);
}

void AdwaitaDecorationStyle::drawBackground(QPainter *painter, State mode,
                                            const QRect &rect)
{
	painter->save();
	updateGradient(rect);
	if (mode == State::ACTIVE) {
		QLinearGradient gradient(0, rect.y() + 1, 0,
		                         rect.y() + rect.height() - 1);
		gradient.setStops(m_gradient_stops);
		painter->setPen(border_color);
		painter->setBrush(gradient);
	} else {
		painter->setPen(border_color);
		painter->setBrush(QColor(headerbar_inactive_color));
	}
	QRectF titlebar_rect(rect);
	titlebar_rect.setHeight(rect.height() + titlebar_border_radius);
	titlebar_rect.adjust(0.5, 0.5, -0.5, -0.5);

	painter->setClipRect(rect);
	painter->drawRoundedRect(titlebar_rect, titlebar_border_radius,
	                         titlebar_border_radius, Qt::AbsoluteSize);
	if (mode == State::ACTIVE) {
		painter->setPen(bottom_line_color);
	} else {
		painter->setPen(bottom_line_inactive_color);
	}
	painter->drawLine(
	    QPointF(rect.x() + 0.5, rect.y() + rect.height() - 0.5),
	    QPointF(rect.x() + rect.width() - 0.5, rect.y() + rect.height() - 0.5));

	painter->setPen(highlight_color);
	painter->drawLine(
	    QPointF(rect.x() + titlebar_border_radius + 0.5, rect.y() + 1 + 0.5),
	    QPointF(rect.x() + rect.width() - 0.5 - titlebar_border_radius,
	            rect.y() + 1 + 0.5));
	painter->restore();
}

void AdwaitaDecorationStyle::drawTitle(QPainter *painter, State mode,
                                       const QRect &rect, QString title)
{
	painter->save();
	if (mode == State::ACTIVE) {
		painter->setPen(text_color);
	} else {
		painter->setPen(text_inactive_color);
	}
	painter->setFont(m_font);
	painter->drawText(rect, Qt::AlignCenter, title);
	painter->restore();
}

void AdwaitaDecorationStyle::drawCloseButton(QPainter *painter, State mode,
                                             const QRect &rect)
{
	painter->save();
	QRectF button_rect = closeButtonRect(rect);
	button_rect.adjust(0.5, 0.5, -0.5, -0.5);
	drawButtonBackground(painter, mode, button_rect);
	drawButtonIcon(painter, mode, button_rect, m_close_button_icon);
	painter->restore();
}

void AdwaitaDecorationStyle::drawMaximizeButton(QPainter *painter,
                                                DecorationStyle::State mode,
                                                const QRect &rect)
{
	painter->save();
	QRectF button_rect = maximizeButtonRect(rect);
	button_rect.adjust(0.5, 0.5, -0.5, -0.5);
	drawButtonBackground(painter, mode, button_rect);
	drawButtonIcon(painter, mode, button_rect, m_maximize_button_icon);
	painter->restore();
}

void AdwaitaDecorationStyle::drawMinimizeButton(QPainter *painter,
                                                DecorationStyle::State mode,
                                                const QRect &rect)
{
	painter->save();
	QRectF button_rect = minimizeButtonRect(rect);
	button_rect.adjust(0.5, 0.5, -0.5, -0.5);
	drawButtonBackground(painter, mode, button_rect);
	drawButtonIcon(painter, mode, button_rect, m_minimize_button_icon);
	painter->restore();
}

void AdwaitaDecorationStyle::drawButtonBackground(QPainter *painter,
                                                  DecorationStyle::State mode,
                                                  const QRectF &button_rect)
{
	if (mode == State::HOVER) {
		QLinearGradient gradient(0, button_rect.y(), 0,
		                         button_rect.y() + button_rect.height());
		gradient.setStops(m_button_gradient_stops);
		painter->setBrush(gradient);
		painter->setPen(QColor(button_border_color));
		painter->drawRoundedRect(button_rect, button_border_radius,
		                         button_border_radius, Qt::AbsoluteSize);

		painter->setPen(highlight_color);
		painter->drawLine(QPointF(button_rect.x() + button_border_radius + 1,
		                          button_rect.y() + 1),
		                  QPointF(button_rect.x() + button_rect.width() -
		                              button_border_radius - 1,
		                          button_rect.y() + 1));
	} else if (mode == State::PRESS) {
		painter->setBrush(QColor(button_press_color));
		painter->setPen(QColor(button_border_color));
		painter->drawRoundedRect(button_rect, button_border_radius,
		                         button_border_radius, Qt::AbsoluteSize);
	}
}

void AdwaitaDecorationStyle::drawButtonIcon(QPainter *painter,
                                            DecorationStyle::State mode,
                                            const QRectF &button_rect,
                                            QIcon &icon)
{
	int size = button_rect.height();
	QRect icon_rect(button_rect.x() + size / 2 - 8,
	                button_rect.y() + size / 2 - 8, 16, 16);

	if (mode == State::INACTIVE) {
		icon.paint(painter, icon_rect, Qt::AlignCenter, QIcon::Disabled,
		           QIcon::On);
	} else {
		icon.paint(painter, icon_rect, Qt::AlignCenter, QIcon::Active,
		           QIcon::On);
	}
}

void AdwaitaDecorationStyle::drawShadow(QPainter *painter,
                                        DecorationStyle::State mode,
                                        const QRect &rect)
{
	painter->save();
	QLinearGradient gradient(0, 0, 0, rect.height());
	gradient.setStops(m_gradient_stops);
	painter->setPen(border_color);
	painter->setBrush(gradient);
	painter->restore();
}

void AdwaitaDecorationStyle::drawLeftBorder(QPainter *painter,
                                            DecorationStyle::State mode,
                                            const QRect &rect)
{
	painter->save();
	painter->setPen(border_color);
	QLineF line(rect.x() + rect.width() - 1 + 0.5, rect.y() + 0.5,
	            rect.x() + rect.width() - 1 + 0.5,
	            rect.y() + rect.height() - 0.5);
	painter->drawLine(line);
	painter->restore();
}

void AdwaitaDecorationStyle::drawRightBorder(QPainter *painter,
                                             DecorationStyle::State mode,
                                             const QRect &rect)
{
	painter->save();
	painter->setPen(border_color);
	QLineF line(rect.x() + 0.5, rect.y() + 0.5, rect.x() + 0.5,
	            rect.y() + rect.height() - 0.5);
	painter->drawLine(line);
	painter->restore();
}

void AdwaitaDecorationStyle::drawBottomBorder(QPainter *painter,
                                              DecorationStyle::State mode,
                                              const QRect &rect)
{
	painter->save();
	painter->setPen(border_color);
	QLineF line(rect.x() + 0.5, rect.y() + 0.5, rect.x() + rect.width() - 0.5,
	            rect.y() + 0.5);
	painter->drawLine(line);
	painter->restore();
}

void AdwaitaDecorationStyle::updateGradient(const QRect &rect)
{
	double stop =
	    (static_cast<double>(gradient_bottom_distance) / rect.height());
	m_gradient_stops[1].first = 1.0 - stop;
}

int AdwaitaDecorationStyle::buttonSize(const QRect &titlebar_rect) const
{
	return titlebar_rect.height() - 2 - button_padding * 2;
}

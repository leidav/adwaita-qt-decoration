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
	m_gradient_stops.append(QGradientStop(0.0, headerbar_color));
	m_gradient_stops.append(QGradientStop(0.9, headerbar_color));
	m_gradient_stops.append(QGradientStop(1.0, darker(headerbar_color, 13)));

	m_button_gradient_stops.append(QGradientStop(0.0, highlight_color));
	m_button_gradient_stops.append(QGradientStop(0.1, QColor(0xf7f7f6)));
	m_button_gradient_stops.append(QGradientStop(1.0, QColor(0xdfdfde)));

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
	return QRect(rect.width() - 1 - size - button_padding,
	             rect.height() - 1 - size - button_padding, size, size);
}

void AdwaitaDecorationStyle::drawBackground(QPainter *painter, Mode mode,
                                            const QRect &rect)
{
	updateGradient(rect);
	if (mode == Mode::ACTIVE) {
		QLinearGradient gradient(0, 1, 0, rect.height() - 1);
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
	if (mode == Mode::ACTIVE) {
		painter->setPen(bottom_line_color);
	} else {
		painter->setPen(bottom_line_inactive_color);
	}
	painter->drawLine(QPointF(0.5, rect.height() - 0.5),
	                  QPointF(rect.width() - 0.5, rect.height() - 0.5));

	painter->setPen(highlight_color);
	painter->drawLine(
	    QPointF(titlebar_border_radius + 0.5, 1.5),
	    QPointF(rect.width() - 0.5 - titlebar_border_radius, 1.5));
}

void AdwaitaDecorationStyle::drawTitle(QPainter *painter, Mode mode,
                                       const QRect &rect, QString title)
{
	if (mode == Mode::ACTIVE) {
		painter->setPen(text_color);
	} else {
		painter->setPen(text_inactive_color);
	}
	painter->setFont(m_font);
	painter->drawText(rect, Qt::AlignCenter, title);
}

void AdwaitaDecorationStyle::drawCloseButton(QPainter *painter, Mode mode,
                                             const QRect &rect)
{
	QRectF button_rect = closeButtonRect(rect);
	button_rect.adjust(0.5, 0.5, -0.5, -0.5);
	if (mode == Mode::HOVER) {
		QLinearGradient gradient(0, button_rect.y(), 0,
		                         button_rect.y() + button_rect.height());
		gradient.setStops(m_button_gradient_stops);
		painter->setBrush(gradient);
		painter->setPen(QColor(0xb7b7b4));
		painter->drawRoundedRect(button_rect, button_border_radius,
		                         button_border_radius, Qt::AbsoluteSize);
	} else if (mode == Mode::PRESS) {
		painter->setBrush(QColor(button_press_color));
		painter->setPen(QColor(0xb7b7b4));
		painter->drawRoundedRect(button_rect, button_border_radius,
		                         button_border_radius, Qt::AbsoluteSize);
	}

	int size = buttonSize(rect);
	QRect icon_rect(button_rect.x() + size / 2 - 8,
	                button_rect.y() + size / 2 - 8, 16, 16);

	if (mode == Mode::INACTIVE) {
		m_close_button_icon.paint(painter, icon_rect, Qt::AlignCenter,
		                          QIcon::Disabled, QIcon::On);
	} else {
		m_close_button_icon.paint(painter, icon_rect, Qt::AlignCenter,
		                          QIcon::Active, QIcon::On);
	}
}

void AdwaitaDecorationStyle::drawShadow(QPainter *painter,
                                        DecorationStyle::Mode mode,
                                        const QRect &rect)
{
	QLinearGradient gradient(0, 0, 0, rect.height());
	gradient.setStops(m_gradient_stops);
	painter->setPen(border_color);
	painter->setBrush(gradient);
}

void AdwaitaDecorationStyle::updateGradient(const QRect &rect)
{
	double stop =
	    (static_cast<double>(gradient_bottom_distance) / rect.height());
	m_gradient_stops[1].first = 1.0 - stop;
}

int AdwaitaDecorationStyle::buttonSize(const QRect &rect) const
{
	return rect.height() - 2 - button_padding * 2;
}

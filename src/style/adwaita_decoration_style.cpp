#include "adwaita_decoration_style.h"
#include <QIcon>
#include <cmath>

int kernelSize(int radius)
{
	// https://github.com/GNOME/mutter/blob/gnome-3-30/src/compositor/meta-shadow-factory.c
	// return static_cast<int>(0.5 + radius * (0.75 * std::sqrt(2 * M_PI)));
	return radius * 2 + 1;
	// return static_cast<int>(0.5 + radius * (0.75 * sqrt(2 * M_PI)));
}

static void horizontalBoxBlur(uint8_t out[], const uint8_t in[], int width,
                              int height, int radius)
{
	int kernelsize = kernelSize(radius);
	uint8_t *output = &out[3];
	for (int y = 0; y < height; y++) {
		const uint8_t *input_line = &in[y * width * 4 + 3];

		int sum = 0;
		for (int i = 0; i < kernelsize; i++) {
			sum += input_line[i * 4];
		}
		output[(radius * height + y) * 4] =
		    static_cast<uint8_t>(sum / kernelsize);
		for (int x = radius + 1; x < width - radius; x++) {
			sum = sum - input_line[(x - radius - 1) * 4] +
			      input_line[(x + radius) * 4];
			output[(x * height + y) * 4] =
			    static_cast<uint8_t>(sum / kernelsize);
		}
	}
}

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
	m_maximize_button_icon_restore =
	    QIcon::fromTheme("window-restore-symbolic");

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
	int kernelsize = kernelSize(shadow_radius);
	createShadowPixmaps(kernelsize * 8, kernelsize * 8, shadow_radius);
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
                                                const QRect &rect,
                                                bool window_maximized)
{
	painter->save();
	QRectF button_rect = maximizeButtonRect(rect);
	button_rect.adjust(0.5, 0.5, -0.5, -0.5);
	drawButtonBackground(painter, mode, button_rect);
	if (window_maximized) {
		drawButtonIcon(painter, mode, button_rect,
		               m_maximize_button_icon_restore);
	} else {
		drawButtonIcon(painter, mode, button_rect, m_maximize_button_icon);
	}
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
	int kernelsize = kernelSize(shadow_radius);
	int tilesize = kernelsize * 2;
	int inner_offset = 0;
	if (mode == DecorationStyle::State::INACTIVE) {
		inner_offset = 0;
	}
	// top left
	painter->drawPixmap(
	    QRect(rect.x() + inner_offset,
	          rect.y() + vertical_shadow_offset + inner_offset, tilesize,
	          tilesize),
	    m_shadow, QRect(shadow_radius, shadow_radius, tilesize, tilesize));
	// top right
	painter->drawPixmap(QRect(rect.x() + rect.width() - tilesize - inner_offset,
	                          rect.y() + inner_offset + vertical_shadow_offset,
	                          tilesize, tilesize),
	                    m_shadow,
	                    QRect(m_shadow.width() - tilesize - shadow_radius,
	                          shadow_radius, tilesize, tilesize));
	// bottom left
	painter->drawPixmap(
	    QRect(rect.x() + inner_offset,
	          rect.y() + rect.height() - tilesize - inner_offset +
	              vertical_shadow_offset,
	          tilesize, tilesize),
	    m_shadow,
	    QRect(shadow_radius, m_shadow.height() - tilesize - shadow_radius,
	          tilesize, tilesize));
	// bottom right
	painter->drawPixmap(QRect(rect.x() + rect.width() - tilesize - inner_offset,
	                          rect.y() + rect.height() - tilesize -
	                              inner_offset + vertical_shadow_offset,
	                          tilesize, tilesize),
	                    m_shadow,
	                    QRect(m_shadow.width() - tilesize - shadow_radius,
	                          m_shadow.height() - tilesize - shadow_radius,
	                          tilesize, tilesize));

	// top
	painter->drawTiledPixmap(
	    QRectF(rect.x() + tilesize + inner_offset,
	           rect.y() + inner_offset + vertical_shadow_offset,
	           rect.width() - (tilesize + inner_offset) * 2, tilesize),
	    m_shadow_top);
	// bottom
	painter->drawTiledPixmap(
	    QRectF(rect.x() + tilesize + inner_offset,
	           rect.y() + rect.height() - tilesize - inner_offset +
	               vertical_shadow_offset,
	           rect.width() - (tilesize + inner_offset) * 2, tilesize),
	    m_shadow_bottom);
	// left
	painter->drawTiledPixmap(
	    QRectF(rect.x() + inner_offset,
	           rect.y() + tilesize + inner_offset + vertical_shadow_offset,
	           tilesize, rect.height() - (tilesize + inner_offset) * 2),
	    m_shadow_left);
	// right
	painter->drawTiledPixmap(
	    QRectF(rect.x() + rect.width() - tilesize - inner_offset,
	           rect.y() + tilesize + inner_offset + vertical_shadow_offset,
	           tilesize, rect.height() - (tilesize + inner_offset) * 2),
	    m_shadow_right);
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

void AdwaitaDecorationStyle::createShadowPixmaps(int width, int height,
                                                 int blur_radius)

{
	int kernelsize = kernelSize(blur_radius);
	QColor color(0, 0, 0, 127);

	QImage img1(width + kernelsize, height + kernelsize,
	            QImage::Format_ARGB32_Premultiplied);
	QImage img2(img1.height(), img1.width(), img1.format());
	img1.fill(Qt::transparent);
	img2.fill(Qt::transparent);
	QPainter p(&img1);
	p.fillRect(kernelsize + blur_radius, kernelsize + blur_radius,
	           img1.width() - 2 * (kernelsize + blur_radius),
	           img1.height() - 2 * (kernelsize + blur_radius), color);

	horizontalBoxBlur(img2.scanLine(0), img1.scanLine(0), img1.width(),
	                  img1.height(), blur_radius);
	horizontalBoxBlur(img1.scanLine(0), img2.scanLine(0), img2.width(),
	                  img2.height(), blur_radius);
	horizontalBoxBlur(img2.scanLine(0), img1.scanLine(0), img1.width(),
	                  img1.height(), blur_radius);
	horizontalBoxBlur(img1.scanLine(0), img2.scanLine(0), img2.width(),
	                  img2.height(), blur_radius);
	horizontalBoxBlur(img2.scanLine(0), img1.scanLine(0), img1.width(),
	                  img1.height(), blur_radius);
	horizontalBoxBlur(img1.scanLine(0), img2.scanLine(0), img2.width(),
	                  img2.height(), blur_radius);
	m_shadow = QPixmap::fromImage(img1);
	m_shadow_top = m_shadow.copy(kernelsize * 3, shadow_radius, kernelsize,
	                             kernelsize * 2);
	m_shadow_bottom = m_shadow.copy(
	    kernelsize * 3, m_shadow.height() - kernelsize * 2 - shadow_radius,
	    kernelsize, kernelsize * 2);
	m_shadow_left = m_shadow.copy(shadow_radius, kernelsize * 3, kernelsize * 2,
	                              kernelsize);
	m_shadow_right =
	    m_shadow.copy(m_shadow.width() - kernelsize * 2 - shadow_radius,
	                  kernelsize * 3, kernelsize * 2, kernelsize);
}

#ifndef ADWAITA_DECORATION_STYLE_H
#define ADWAITA_DECORATION_STYLE_H

#include <decoration_style.h>
#include <QIcon>

#include "helper.h"

enum AdwaitaColors {
	base_color = 0xFFFFFF,
	text_color = 0x0,
	bg_color = 0xE8E8E7,
	fg_color = 0x2E3436,

	headerbar_color = lighter(bg_color, 5),
	headerbar_inactive_color = 0xe9e9e8,
	bottom_line_inactive_color = 0xc1c1be,
	text_inactive_color = 0x8c8f90,
	button_press_color = 0xdadad8,
	highlight_color = 0xfdfdfd,
	border_color = 0xa2a2a2,
	bottom_line_color = 0x929292,

};

enum AdwaitaMetrics {
	font_size = 11,
	border_width = 1,
	titlebar_border_radius = 7,
	button_border_radius = 3,
	min_height = 36,
	min_height_with_border = (min_height + border_width * 2),
	font_padding_y = 6,
	font_padding_x = 12,
	button_padding = 4,
	gradient_bottom_distance = 3
};

class AdwaitaDecorationStyle : public DecorationStyle
{
public:
	AdwaitaDecorationStyle();

	int height() const override;

	QFont titleFont() const override;

	QRect closeButtonRect(const QRect &titlebar_rect) const override;

	QRect maximizeButtonRect(const QRect &titlebar_rect) const override;

	QRect minimizeButtonRect(const QRect &titlebar_rect) const override;

	void drawBackground(QPainter *painter, State mode,
	                    const QRect &rect) override;

	void drawTitle(QPainter *painter, State mode, const QRect &rect,
	               QString title) override;

	void drawCloseButton(QPainter *painter, State mode,
	                     const QRect &rect) override;
	void drawMaximizeButton(QPainter *painter, State mode,
	                        const QRect &rect) override;
	void drawMinimizeButton(QPainter *painter, State mode,
	                        const QRect &rect) override;

	void drawShadow(QPainter *painter, State mode, const QRect &rect) override;

	void drawLeftBorder(QPainter *painter, State mode,
	                    const QRect &rect) override;
	void drawRightBorder(QPainter *painter, State mode,
	                     const QRect &rect) override;
	void drawBottomBorder(QPainter *painter, State mode,
	                      const QRect &rect) override;

private:
	void drawButtonBackground(QPainter *painter, State mode,
	                          const QRectF &button_rect);
	void drawButtonIcon(QPainter *painter, State mode,
	                    const QRectF &button_rect, QIcon &icon);

	void updateGradient(const QRect &rect);
	int buttonSize(const QRect &titlebar_rect) const;

	int m_height;
	QRect m_buttonRect;
	QFont m_font;
	QGradientStops m_gradient_stops;
	QGradientStops m_button_gradient_stops;
	QIcon m_close_button_icon;
	QIcon m_minimize_button_icon;
	QIcon m_maximize_button_icon;
};

#endif  // ADWAITADECORATIONSTYLE_H

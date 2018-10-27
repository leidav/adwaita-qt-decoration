#ifndef DECORATION_STYLE_H
#define DECORATION_STYLE_H

#include <QObject>
#include <QPainter>

class DecorationStyle
{
public:
	enum class State { ACTIVE, INACTIVE, HOVER, PRESS };

	virtual int height() const = 0;

	virtual QFont titleFont() const = 0;

	virtual QRect closeButtonRect(const QRect &titlebar_rect) const = 0;

	virtual QRect maximizeButtonRect(const QRect &titlebar_rect) const = 0;

	virtual QRect minimizeButtonRect(const QRect &titlebar_rect) const = 0;

	virtual void drawBackground(QPainter *painter, State mode,
	                            const QRect &rect) = 0;

	virtual void drawTitle(QPainter *painter, State mode, const QRect &rect,
	                       QString title) = 0;
	virtual void drawCloseButton(QPainter *painter, State mode,
	                             const QRect &rect) = 0;
	virtual void drawMaximizeButton(QPainter *painter, State mode,
	                                const QRect &rect) = 0;
	virtual void drawMinimizeButton(QPainter *painter, State mode,
	                                const QRect &rect) = 0;

	virtual void drawShadow(QPainter *painter, State mode,
	                        const QRect &rect) = 0;
	virtual void drawLeftBorder(QPainter *painter, State mode,
	                            const QRect &rect) = 0;
	virtual void drawRightBorder(QPainter *painter, State mode,
	                             const QRect &rect) = 0;
	virtual void drawBottomBorder(QPainter *painter, State mode,
	                              const QRect &rect) = 0;
};

#endif

#ifndef DECORATION_STYLE_H
#define DECORATION_STYLE_H

#include <QObject>
#include <QPainter>

class DecorationStyle
{
public:
	enum class Mode { ACTIVE, INACTIVE, HOVER, PRESS };

	virtual int height() const = 0;

	virtual QFont titleFont() const = 0;

	virtual QRect closeButtonRect(const QRect &titlebar_rect) const = 0;

	virtual void drawBackground(QPainter *painter, Mode mode,
	                            const QRect &rect) = 0;

	virtual void drawTitle(QPainter *painter, Mode mode, const QRect &rect,
	                       QString title) = 0;
	virtual void drawCloseButton(QPainter *painter, Mode mode,
	                             const QRect &rect) = 0;

	virtual void drawShadow(QPainter *painter, Mode mode,
	                        const QRect &rect) = 0;
	virtual void drawLeftBorder(QPainter *painter, Mode mode,
	                            const QRect &rect) = 0;
	virtual void drawRightBorder(QPainter *painter, Mode mode,
	                             const QRect &rect) = 0;
	virtual void drawBottomBorder(QPainter *painter, Mode mode,
	                              const QRect &rect) = 0;
};

#endif

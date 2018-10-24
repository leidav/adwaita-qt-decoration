#ifndef HEADERBAR_H
#define HEADERBAR_H

#include <decoration_style.h>
#include <QWidget>

class HeaderBar : public QWidget
{
	Q_OBJECT
public:
	explicit HeaderBar(QWidget *parent = nullptr);
	explicit HeaderBar(DecorationStyle *style, QWidget *parent = nullptr);
	void paintEvent(QPaintEvent *event) override;

	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void leaveEvent(QEvent *event) override;

signals:

public slots:
private:
	DecorationStyle *m_style;
	QString m_title;
	bool m_button_hover;
	bool m_button_press;
};

#endif

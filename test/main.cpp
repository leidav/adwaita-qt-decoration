#include <headerbar.h>

#include <QApplication>
#include <QLayout>
#include <QMainWindow>
#include <QPushButton>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	app.addLibraryPath("../lib");

	QMainWindow window;

	window.setWindowTitle("MainWindow");
	window.resize(640, 480);
	QWidget* central_widget = new QWidget;
	window.setCentralWidget(central_widget);

	QVBoxLayout* layout = new QVBoxLayout;
	HeaderBar* headerbar = new HeaderBar;
	layout->addWidget(headerbar);
	central_widget->setLayout(layout);
	window.show();

	return app.exec();
}

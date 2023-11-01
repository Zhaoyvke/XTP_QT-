#include "MyMainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication v_app(argc, argv);

	MyMainWindow v_myWindow;

	v_myWindow.showNormal();

	v_app.exec();//mainWindow×èÈûÔÙ´Ë
	
	return 0;
}

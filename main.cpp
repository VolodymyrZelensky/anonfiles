#include "anonfiles.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationVersion("2.0");
	a.setApplicationName("Anonfiles");
	anonfiles w;
	w.show();
	return a.exec();
}

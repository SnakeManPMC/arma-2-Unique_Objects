#include <QtCore/QCoreApplication>
#include "uniqueobjects.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	uniqueObjects(argc, argv);

	return 0;
}

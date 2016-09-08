#include "classification.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTextCodec * textCodec = QTextCodec::codecForName("System");
	QTextCodec::setCodecForLocale(textCodec);

	QString translatorFileName = "classification_zh.qm";
	QTranslator *translator = new QTranslator(&a);
	if (translator->load(translatorFileName))
		a.installTranslator(translator);

	Classification w;
	w.show();
	return a.exec();
}

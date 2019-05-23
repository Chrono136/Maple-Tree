#ifndef QTCOMPRESSOR_H
#define QTCOMPRESSOR_H

#include <QFile>
#include <QObject>
#include <QDir>
#include <QDataStream>

class QtCompressor : public QObject
{
    Q_OBJECT
public:
    explicit QtCompressor(QObject *parent = nullptr);

    //A recursive function that scans all files inside the source folder
    //and serializes all files in a row of file names and compressed
    //binary data in a single file
    static bool compress(QString sourceFolder, QString destinationFile);

    //A function that deserializes data from the compressed file and
    //creates any needed subfolders before saving the file
	static bool decompress(QString sourceFile, QString destinationFolder);

	//counts the number of files in a directory
	static int count(QString directory);

	static QtCompressor* self;

private:
	static QFile file;
	static QDataStream dataStream;
	static int curFile;
	static int numFiles;
	static int countDown;

	static bool handleCompress(QString sourceFolder, QString prefex);
	
signals:
	void updateProgress(qint64 min, qint64 max);
};

#endif // QTCOMPRESSOR_H

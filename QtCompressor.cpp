#include "QtCompressor.h"
#include "configuration.h"

QtCompressor* QtCompressor::self;
QFile QtCompressor::file;
QDataStream QtCompressor::dataStream;
int QtCompressor::curFile = 1;
int QtCompressor::numFiles = 0;
int QtCompressor::countDown = 1;

QtCompressor::QtCompressor(QObject *parent) :
    QObject(parent)
{
}

bool QtCompressor::compress(QString sourceFolder, QString destinationFile)
{
    QDir src(sourceFolder);
    if(!src.exists())//folder not found
    {
        return false;
    }

    file.setFileName(destinationFile);
    if(!file.open(QIODevice::WriteOnly))//could not open file
    {
        return false;
    }

    dataStream.setDevice(&file);
	bool success;

	countDown += numFiles = count(sourceFolder);
	Configuration::log("Compressing " + sourceFolder, true);

	if (!(success = handleCompress(sourceFolder, ""))) {
		Configuration::log("Compression Failed: " + sourceFolder, true);
	}else{
		Configuration::log("Compression Successful: " + sourceFolder, true);
	}

    file.close();
    return success;
}

bool QtCompressor::handleCompress(QString sourceFolder, QString prefex)
{
	QDir dir(sourceFolder);
	if (!dir.exists())
		return false;

	//1 - list all folders inside the current folder
	dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
	QFileInfoList foldersList = dir.entryInfoList();

	//2 - For each folder in list: call the same function with folders' paths
	for (int i = 0; i < foldersList.length(); i++)
	{
		QString folderName = foldersList.at(i).fileName();
		QString folderPath = dir.absolutePath() + "/" + folderName;
		QString newPrefex = prefex + "/" + folderName;

		handleCompress(folderPath, newPrefex);
	}

	//3 - List all files inside the current folder
	dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
	QFileInfoList filesList = dir.entryInfoList();

	//4- For each file in list: add file path and compressed binary data
	for (int i = 0; i < filesList.length(); i++)
	{
		QFile file(dir.absolutePath() + "/" + filesList.at(i).fileName());
		if (!file.open(QIODevice::ReadOnly))//couldn't open file
		{
			return false;
		}

		QString filename(prefex + "/" + filesList.at(i).fileName());
		Configuration::log("Compressing: " + self->file.fileName() + " << " + filename);

		dataStream << --countDown;
		dataStream << filename;
		dataStream << qCompress(file.readAll(), 9);

		emit self->updateProgress(curFile++, numFiles);

		file.close();
	}

	return true;
}

bool QtCompressor::decompress(QString sourceFile, QString destinationFolder)
{
	//validation
	QFile src(sourceFile);
	if (!src.exists())
	{//file not found, to handle later
		return false;
	}
	QDir dir;
	if (!dir.mkpath(destinationFolder))
	{//could not create folder
		return false;
	}

	file.setFileName(sourceFile);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	dataStream.setDevice(&file);
	int count = 0;
	int max = 0;

	while (!dataStream.atEnd())
	{
		int index;
		QString fileName;
		QByteArray data;

		//extract file name and data in order
		dataStream >> index >> fileName >> data;
		Configuration::log("Decompression: " + destinationFolder + " << " + fileName);

		if (max == 0) max = index;

		//create any needed folder
		QString subfolder;
		for (int i = fileName.length() - 1; i > 0; i--)
		{
			if ((QString(fileName.at(i)) == QString("\\")) || (QString(fileName.at(i)) == QString("/")))
			{
				subfolder = fileName.left(i);
				dir.mkpath(destinationFolder + "/" + subfolder);
				break;
			}
		}

		QFile outFile(destinationFolder + "/" + fileName);
		if (!outFile.open(QIODevice::WriteOnly))
		{
			file.close();
			return false;
		}
		outFile.write(qUncompress(data));
		outFile.close();

		emit self->updateProgress(count++, max - 1);
	}

	file.close();
	return true;
}

int QtCompressor::count(QString directory)
{
	QDir dir(directory);
	if (!dir.exists())
		return false;

	int num = 0;

	dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
	QFileInfoList foldersList = dir.entryInfoList();

	for (int i = 0; i < foldersList.length(); i++)
	{
		QString folderName = foldersList.at(i).fileName();
		QString folderPath = dir.absolutePath() + "/" + folderName;

		num += count(folderPath);
	}

	dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
	QFileInfoList filesList = dir.entryInfoList();
	num += filesList.count();

	return num;
}

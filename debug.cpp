#include "debug.h"
#include "mapleseed.h"

bool Debug::isEnabled = false;

Debug::Debug(QObject *parent) : QObject(parent)
{
}

void Debug::categoryFilter(QLoggingCategory *category)
{
    if (strcmp(category->categoryName(), "default") == 0)
    {
        category->setEnabled(QtDebugMsg, isEnabled);
    }
    else if (strcmp(category->categoryName(), "qt.gamepad") == 0)
    {
        category->setEnabled(QtDebugMsg, isEnabled);
    }
    else
    {
        category->setEnabled(QtDebugMsg, false);
    }
}

void Debug::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    QString qtype;
    switch (type)
    {
    case QtDebugMsg:
        qtype = "Debug";
        break;
    case QtInfoMsg:
        qtype = "Info";
        break;
    case QtWarningMsg:
        qtype = "Warning";
        break;
    case QtCriticalMsg:
        qtype = "Critical";
        break;
    case QtFatalMsg:
        qtype = "Fatal";
        break;
    }
#ifdef QT_DEBUG
    MapleSeed::self->messageLog(QString("%1: %2 (%3:%4, %5)")
                                .arg(qtype)
                                .arg(localMsg.constData())
                                .arg(file)
                                .arg(context.line)
                                .arg(function));
#else
    MapleSeed::self->messageLog(QString("%1: %2").arg(qtype).arg(localMsg.constData()));
#endif
}

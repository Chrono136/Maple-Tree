#ifndef DEBUG_H
#define DEBUG_H

#include <QObject>
#include <QtDebug>
#include <QLoggingCategory>

class Debug : public QObject
{
    Q_OBJECT
public:
    explicit Debug(QObject *parent = nullptr);

    static void categoryFilter(QLoggingCategory *category);

    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    static bool debugEnabled;

signals:

public slots:
};

#endif // DEBUG_H

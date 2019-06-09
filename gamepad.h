#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <QObject>
#include <QtConcurrent>
#include <QtGamepad>

class QGamepad;

class Gamepad : public QObject
{
    Q_OBJECT
public:
    explicit Gamepad(QObject *parent = nullptr);
    ~Gamepad();

    void init();

    static void pressed(int deviceId, QGamepadManager::GamepadButton button, double value);
    static void enable();
    static void disable();
    static void terminate();

    static Gamepad *instance;
    static bool isEnabled;

signals:
    void gameStart(bool pressed);
    void gameClose(bool pressed);
    void gameUp(bool pressed);
    void gameDown(bool pressed);

public slots:

private:
    QGamepad *m_gamepad;
    QGamepadManager* manager;
};

#endif // GAMEPAD_H

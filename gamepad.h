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
    void closeGame();

    static void button(int deviceId, QGamepadManager::GamepadButton button, double value = 0);
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
    bool l1 = false;
    bool l2 = false;
    bool r1 = false;
    bool r2 = false;
    bool select = false;
};

#endif // GAMEPAD_H

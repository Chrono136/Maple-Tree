#include "gamepad.h"
#include "configuration.h"

Gamepad *Gamepad::instance;
bool Gamepad::isEnabled = false;

Gamepad::Gamepad(QObject *parent) : QObject(parent), m_gamepad(nullptr)
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.gamepad.debug=true"));
    manager = QGamepadManager::instance();
    qInfo() << "Gamepad handler initialized";
}

Gamepad::~Gamepad()
{
    delete m_gamepad;
    qInfo() << "Gamepad handler terminated";
}

void Gamepad::init()
{
    connect(manager, &QGamepadManager::gamepadConnected, this, [=](int deviceId)
    {
        qInfo("Gamepad detected: (%i)", deviceId);
        m_gamepad = new QGamepad(deviceId, this);
        connect(m_gamepad, &QGamepad::buttonUpChanged, this, [=](bool pressed) { emit gameUp(pressed && isEnabled); });
        connect(m_gamepad, &QGamepad::buttonDownChanged, this, [=](bool pressed) { emit gameDown(pressed && isEnabled); });
        connect(m_gamepad, &QGamepad::buttonStartChanged, this, [=](bool pressed) { emit gameStart(pressed && isEnabled); });
        //connect(m_gamepad, &QGamepad::buttonGuideChanged, this, [=](bool pressed) { emit gameClose(pressed && isEnabled); });
    });
    connect(manager, &QGamepadManager::gamepadButtonPressEvent, this, Gamepad::pressed);
}

void Gamepad::pressed(int deviceId, QGamepadManager::GamepadButton button, double value)
{
    qInfo() << "Gamepad(" << deviceId << ") button:" << button << "=" << value;
}

void Gamepad::enable()
{
    qInfo() << "Gamepad mode" << (isEnabled = true);
}

void Gamepad::disable()
{
    qInfo() << "Gamepad mode" << (isEnabled = false);
}

void Gamepad::terminate()
{
    isEnabled = false;
    if (instance)
    {
        delete instance;
    }
}

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

    });
    connect(manager, &QGamepadManager::gamepadButtonPressEvent, this, Gamepad::button);
    connect(manager, &QGamepadManager::gamepadButtonReleaseEvent, this,
            [=](int deviceId, QGamepadManager::GamepadButton button) { Gamepad::button(deviceId, button); });
}

void Gamepad::closeGame()
{
    if (l1 && l2 && r1 && r2 && select)
    {
        emit instance->gameClose(true);
    }
}

void Gamepad::button(int deviceId, QGamepadManager::GamepadButton button, double value)
{
    qDebug() << "Gamepad(" << deviceId << ") button:" << button << "=" << value;
    if (button == QGamepadManager::GamepadButton::ButtonL1)
    {
        instance->l1 = static_cast<bool>(value);
    }
    if (button == QGamepadManager::GamepadButton::ButtonL2)
    {
        instance->l2 = static_cast<bool>(value);
    }
    if (button == QGamepadManager::GamepadButton::ButtonR1)
    {
        instance->r1 = static_cast<bool>(value);
    }
    if (button == QGamepadManager::GamepadButton::ButtonR2)
    {
        instance->r2 = static_cast<bool>(value);
    }
    if (button == QGamepadManager::GamepadButton::ButtonSelect)
    {
        instance->select = static_cast<bool>(value);
    }
    instance->closeGame();
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

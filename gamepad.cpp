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
        qInfo("Gamepad connected: (%i)", deviceId);
        m_gamepads.insert(deviceId, m_gamepad = new QGamepad(deviceId, this));
    });
    connect(manager, &QGamepadManager::gamepadDisconnected, this, [=](int deviceId)
    {
        qInfo("Gamepad disconnected: (%i)", deviceId);
        m_gamepads.remove(deviceId);
    });
    connect(manager, &QGamepadManager::gamepadButtonPressEvent, this, Gamepad::button);
    connect(manager, &QGamepadManager::gamepadButtonReleaseEvent, this, &Gamepad::release);
}

void Gamepad::closeGame()
{
    if (l1 && l2 && r1 && r2 && select)
    {
        qDebug() << "Terminating cemu process";
        emit instance->gameClose(true);
    }
}

void Gamepad::button(int deviceId, QGamepadManager::GamepadButton button, double value)
{
    auto buttons = instance->manager;
    qDebug() << "Gamepad(" << deviceId << ") button:" << button << "=" << value;

    if (button == buttons->ButtonUp)
    {
        emit instance->gameUp(static_cast<bool>(value) && isEnabled);
    }
    if (button == buttons->ButtonDown)
    {
        emit instance->gameDown(static_cast<bool>(value) && isEnabled);
    }
    if (button == buttons->ButtonA)
    {
        emit instance->gameStart(static_cast<bool>(value) && isEnabled);
    }
    if (button == buttons->ButtonL1)
    {
        if ((instance->l1 = static_cast<bool>(value)))
        {
            emit instance->prevTab(static_cast<bool>(value) && isEnabled);
        }
    }
    if (button == buttons->ButtonL2)
    {
        instance->l2 = static_cast<bool>(value);
    }
    if (button == buttons->ButtonR1)
    {if ((instance->r1 = static_cast<bool>(value)))
        {
            emit instance->nextTab(static_cast<bool>(value) && isEnabled);
        }
    }
    if (button == buttons->ButtonR2)
    {
        instance->r2 = static_cast<bool>(value);
    }
    if (button == buttons->ButtonSelect)
    {
        instance->select = static_cast<bool>(value);
        instance->closeGame();
    }
}

void Gamepad::release(int deviceId, QGamepadManager::GamepadButton button)
{
    return Gamepad::button(deviceId, button, 0);
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

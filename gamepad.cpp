#include "gamepad.h"
#include "configuration.h"
#include <QLoggingCategory>

Gamepad *Gamepad::instance;
bool Gamepad::isEnabled = false;

Gamepad::Gamepad(QObject *parent) : QObject(parent), m_gamepad(nullptr)
{
    manager = QGamepadManager::instance();
}

Gamepad::~Gamepad()
{
    delete m_gamepad;
    Configuration::self->log("Gamepad::~Gamepad() -> terminated");
}

void Gamepad::init()
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.gamepad.debug=true"));
    Configuration::self->log("Gamepad::init() -> Waiting for gamepad...");
    connect(manager, &QGamepadManager::gamepadConnected, this, [=](int deviceId)
    {
        m_gamepad = new QGamepad(deviceId, this);
        Configuration::self->log(QString("Gamepad::init() -> Gamepad connected: (%1)").arg(deviceId), true);
        connect(m_gamepad, &QGamepad::buttonUpChanged, this, [=](bool pressed) { emit gameUp(pressed && isEnabled); });
        connect(m_gamepad, &QGamepad::buttonDownChanged, this, [=](bool pressed) { emit gameDown(pressed && isEnabled); });
        connect(m_gamepad, &QGamepad::buttonStartChanged, this, [=](bool pressed) { emit gameStart(pressed && isEnabled); });
        //connect(m_gamepad, &QGamepad::buttonGuideChanged, this, [=](bool pressed) { emit gameClose(pressed && isEnabled); });
    });
    connect(manager, &QGamepadManager::gamepadButtonPressEvent, this, Gamepad::pressed);
}

void Gamepad::pressed(int deviceId, QGamepadManager::GamepadButton button, double value)
{
    Configuration::self->log(QString("Gamepad::init() -> Gamepad(%1) button: %2=%3").arg(deviceId).arg(button).arg(value));
}

void Gamepad::terminate()
{
    isEnabled = false;
    if (instance)
    {
        delete instance;
    }
}

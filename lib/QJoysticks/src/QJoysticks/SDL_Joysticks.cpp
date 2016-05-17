/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QFile>
#include <QTimer>
#include <QJoysticks/SDL_Joysticks.h>

//==================================================================================================
// Joystick mappings stuff
//==================================================================================================

static QString GENERIC_MAPPINGS;

#if defined Q_OS_WIN
#define GENERIC_MAPPINGS_PATH ":/QJoysticks/SDL/GenericMappings/Windows.txt"
#elif defined Q_OS_MAC
#define GENERIC_MAPPINGS_PATH ":/QJoysticks/SDL/GenericMappings/OSX.txt"
#elif defined Q_OS_LINUX
#define GENERIC_MAPPINGS_PATH ":/QJoysticks/SDL/GenericMappings/Linux.txt"
#endif

//==================================================================================================
// SDL_Joysticks::SDL_Joysticks
//==================================================================================================

SDL_Joysticks::SDL_Joysticks() {
    m_tracker = -1;
    SDL_Init (SDL_INIT_AUDIO | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER);

    QFile database (":/QJoysticks/SDL/Database.txt");
    if (database.open (QFile::ReadOnly)) {
        while (!database.atEnd()) {
            QString line = QString::fromUtf8 (database.readLine());
            SDL_GameControllerAddMapping (line.toStdString().c_str());
        }

        database.close();
    }

    QFile genericMappings (GENERIC_MAPPINGS_PATH);
    if (genericMappings.open (QFile::ReadOnly)) {
        GENERIC_MAPPINGS = QString::fromUtf8 (genericMappings.readAll());
        genericMappings.close();
    }

    QTimer::singleShot (1500, Qt::PreciseTimer, this, SLOT (update()));
}

//==================================================================================================
// SDL_Joysticks::joysticks
//==================================================================================================

QList<QJoystickDevice*> SDL_Joysticks::joysticks() {
    QList<QJoystickDevice*> list;

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        list.append (getJoystick (i));

    return list;
}

//==================================================================================================
// SDL_Joysticks::rumble
//==================================================================================================

void SDL_Joysticks::rumble (QJoystickRumble request) {
    SDL_Haptic* haptic = SDL_HapticOpen (request.joystick->id);

    if (haptic != Q_NULLPTR) {
        SDL_HapticRumbleInit (haptic);
        SDL_HapticRumblePlay (haptic, 1, 1000);
    }
}

//==================================================================================================
// SDL_Joysticks::update
//==================================================================================================

void SDL_Joysticks::update() {
    SDL_Event event;

    while (SDL_PollEvent (&event)) {
        switch (event.type) {
        case SDL_JOYDEVICEADDED:
            configureJoystick (&event);
            break;
        case SDL_JOYDEVICEREMOVED:
            SDL_JoystickClose (SDL_JoystickOpen (event.jdevice.which));
            SDL_GameControllerClose (SDL_GameControllerOpen (event.cdevice.which));
            emit countChanged();
            break;
        case SDL_CONTROLLERAXISMOTION:
            emit axisEvent (getAxisEvent (&event));
            break;
        case SDL_JOYBUTTONUP:
            emit buttonEvent (getButtonEvent (&event));
            break;
        case SDL_JOYBUTTONDOWN:
            emit buttonEvent (getButtonEvent (&event));
            break;
        case SDL_JOYHATMOTION:
            emit POVEvent (getPOVEvent (&event));
            break;
        }
    }

    QTimer::singleShot (20, this, SLOT (update()));
}

//==================================================================================================
// SDL_Joysticks::configureJoystick
//==================================================================================================

void SDL_Joysticks::configureJoystick (const SDL_Event* event) {
    if (!SDL_IsGameController (event->cdevice.which)) {
        SDL_Joystick* js = SDL_JoystickOpen (event->jdevice.which);

        if (js != Q_NULLPTR) {
            char guid [1024];
            SDL_JoystickGetGUIDString (SDL_JoystickGetGUID (js), guid, sizeof (guid));

            QString mapping = QString ("%1,%2,%3")
                              .arg (guid)
                              .arg (SDL_JoystickName (js))
                              .arg (GENERIC_MAPPINGS);

            SDL_GameControllerAddMapping (mapping.toStdString().c_str());
            SDL_JoystickClose (js);
        }
    }

    SDL_GameControllerOpen (event->cdevice.which);

    ++m_tracker;
    emit countChanged();
}

//==================================================================================================
// SDL_Joysticks::getDynamicID
//==================================================================================================

int SDL_Joysticks::getDynamicID (int id) {
    id = abs (m_tracker - (id + 1));

    if (id >= SDL_NumJoysticks())
        id -= 1;

    return id;
}

//==================================================================================================
// SDL_Joysticks::getJoystick
//==================================================================================================

QJoystickDevice* SDL_Joysticks::getJoystick (int id) {
    QJoystickDevice* joystick  = new QJoystickDevice;
    SDL_Joystick* sdl_joystick = SDL_JoystickOpen (id);

    joystick->id = getDynamicID (id);

    if (sdl_joystick != Q_NULLPTR) {
        joystick->blacklisted = false;
        joystick->name        = SDL_JoystickName (sdl_joystick);
        joystick->numPOVs     = SDL_JoystickNumHats (sdl_joystick);
        joystick->numAxes     = SDL_JoystickNumAxes (sdl_joystick);
        joystick->numButtons  = SDL_JoystickNumButtons (sdl_joystick);
    }

    return joystick;
}

//==================================================================================================
// SDL_Joysticks::getPOVEvent
//==================================================================================================

QJoystickPOVEvent SDL_Joysticks::getPOVEvent (const SDL_Event* sdl_event) {
    QJoystickPOVEvent event;
    event.pov      = sdl_event->jhat.hat;
    event.joystick = getJoystick (sdl_event->jdevice.which);

    switch (sdl_event->jhat.value) {
    case SDL_HAT_RIGHTUP:
        event.angle = 45;
        break;
    case SDL_HAT_RIGHTDOWN:
        event.angle = 135;
        break;
    case SDL_HAT_LEFTDOWN:
        event.angle = 225;
        break;
    case SDL_HAT_LEFTUP:
        event.angle = 315;
        break;
    case SDL_HAT_UP:
        event.angle = 0;
        break;
    case SDL_HAT_RIGHT:
        event.angle = 90;
        break;
    case SDL_HAT_DOWN:
        event.angle = 180;
        break;
    case SDL_HAT_LEFT:
        event.angle = 270;
        break;
    default:
        event.angle = -1;
        break;
    }

    return event;
}

//==================================================================================================
// SDL_Joysticks::getAxisEvent
//==================================================================================================

QJoystickAxisEvent SDL_Joysticks::getAxisEvent (const SDL_Event* sdl_event) {
    QJoystickAxisEvent event;

    event.axis = sdl_event->caxis.axis;
    event.value = (float) sdl_event->caxis.value / 32767;
    event.joystick = getJoystick (sdl_event->cdevice.which);

    return event;
}

//==================================================================================================
// SDL_Joysticks::getButtonEvent
//==================================================================================================

QJoystickButtonEvent SDL_Joysticks::getButtonEvent (const SDL_Event* sdl_event) {
    QJoystickButtonEvent event;

    event.button = sdl_event->jbutton.button;
    event.pressed = sdl_event->jbutton.state == SDL_PRESSED;
    event.joystick = getJoystick (sdl_event->jdevice.which);

    return event;
}
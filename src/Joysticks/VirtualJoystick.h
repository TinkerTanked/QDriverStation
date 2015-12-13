/*
 * Copyright (c) 2015 WinT 3794 <http://wint3794.org>
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

#pragma once
#ifndef _QDRIVER_STATION_JOYSTICKS_VIRTUAL_JOYSTICK_H
#define _QDRIVER_STATION_JOYSTICKS_VIRTUAL_JOYSTICK_H

#include <QKeyEvent>
#include <QCloseEvent>

#include "Joysticks.h"
#include "KeyEventFilter.h"
#include "ui_VirtualJoystick.h"

/**
 * \class VirtualJoystick
 * \brief Moves the robot with the keyboard
 *
 * The \c VirtualJoystick class allows the user to operate the robot using input
 * from the computer's keyboard.
 *
 * Additionally, this class is in charge of processing both the keyboard input
 * and the joystick input. The generated data is then sent to the \a JoystickTab
 * and the Driver Station
 */
class VirtualJoystick : public QDialog {
    Q_OBJECT

  public:
    VirtualJoystick();

    /**
     * Loads the saved settings on the UI
     */
    void ReadSettings();

    /**
     * Returns the number of axes for the selected joystick
     * @note The joystick can be either the 'real' one or the virtual one
     */
    int GetNumAxes (int js);

    /**
     * Returns the number of hats for the selected joystick
     * @note The joystick can be either the 'real' one or the virtual one
     */
    int GetNumHats (int js);

    /**
     * Returns the number of buttons for the selected joystick
     * @note The joystick can be either the 'real' one or the virtual one
     */
    int GetNumButtons (int js);

  signals:
    /**
     * Emitted when the joystick count is adjusted and updated
     */
    void CountChanged (int);

    /**
     * Emitted when the joystick count is adjusted and updated
     */
    void CountChanged (QStringList);

    /**
     * Emitted when the system detects a change in the state of the hats of
     * one of the connected joysticks
     */
    void HatEvent (GM_Hat);

    /**
     * Emitted when the system detects a change in the state of the axes of
     * one of the connected joysticks
     */
    void AxisEvent (GM_Axis);

    /**
     * Emitted when the system detects a change in the state of the buttons of
     * one of the connected joysticks
     */
    void ButtonEvent (GM_Button);

  private slots:
    /**
     * Returns the total number of joysticks, including the SDL joysticks and
     * the virtual joystick
     */
    void OnCountChanged (int count);

    /**
     * Returns a list with the names of all joysticks, including the SDL
     * joysticks and the virtual joystick
     */
    void OnCountChanged (QStringList input);

    /**
     * Parses the key event and updates the axes of the virtual joystick
     */
    void RegisterKey (QKeyEvent* event, bool pressed);

    /**
     * Filters the \a key to update the \a value of the axis assigned to a
     * specific key
     */
    void ReadAxes (int key, double value, bool pressed);

    /**
     * Filters the \a key to update the \a pressed state of the button assigned
     * to a specific key
     */
    void ReadButtons (int key, bool pressed);

    /**
     * Resizes the window to a minimum size
     */
    void ResizeToFit();

    /**
     * Saves the new range value on the settings registry
     */
    void OnRangeChanged (double value);

    /**
     * Changes the \a enabled state of the virtual joystick and updates the
     * required internal values
     */
    void SetVirtualJoystickEnabled (bool enabled);

    /**
     * Sends the hat data to the Driver Station
     */
    void OnHatEvent (GM_Hat hat);

    /**
     * Sends the axis data to the Driver Station
     */
    void OnAxisEvent (GM_Axis axis);

    /**
     * Sends the button data to the Driver Station
     */
    void OnButtonEvent (GM_Button button);


  private:
    /**
     * If set to \c true, the application will use some keys of the keyboard to
     * simulate a virtual joystick
     */
    bool m_enabled;

    /**
     * Contains the UI components and widgets
     */
    Ui::VirtualJoystick ui;

    /**
     * Represents the virtual joystick
     */
    GM_Joystick m_joystick;

    /**
     * Allows us to get access to every key event in the application
     */
    KeyEventFilter m_filter;
};

#endif

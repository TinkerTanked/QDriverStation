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
#ifndef _QDRIVER_STATION_MAINWINDOW_SMART_PROGRESSBAR_H
#define _QDRIVER_STATION_MAINWINDOW_SMART_PROGRESSBAR_H

#include <QProgressBar>

/**
 * \class SmartProgressbar
 *
 * Implements a progress bar that can change its color based
 * on the current value that it holds
 */
class SmartProgressbar : public QProgressBar {
    Q_OBJECT

  public:
    explicit SmartProgressbar (QWidget* parent = 0);
    enum Type { kBatteryProgressbar, kCpuUsageProgressbar };

  public slots:
    void SetType (Type type);
    void UpdateColor (int value);

  private:
    Type m_type;
};

#endif

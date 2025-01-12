/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "winframelesswindowcontroller.h"

#include <windowsx.h>
#include <dwmapi.h>

#include "log.h"

using namespace mu::appshell;

WinFramelessWindowController::WinFramelessWindowController()
    : FramelessWindowController()
{
    qApp->installNativeEventFilter(this);
}

void WinFramelessWindowController::init(QWindow* window)
{
    IF_ASSERT_FAILED(window) {
        return;
    }

    HWND hwnd = (HWND)window->winId();

    SetWindowLongPtr(hwnd, GWL_STYLE,
                     static_cast<LONG>(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME
                                       | WS_CLIPCHILDREN));

    const MARGINS shadow_on = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(hwnd, &shadow_on);

    SetWindowPos(hwnd, Q_NULLPTR, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(hwnd, SW_SHOW);
}

bool WinFramelessWindowController::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
    if (eventType != "windows_generic_MSG") {
        return false;
    }

    MSG* msg = static_cast<MSG*>(message);
    if (msg == nullptr) {
        return false;
    }

    switch (msg->message) {
    case WM_NCCALCSIZE: {
        return calculateWindowSize(msg, result);
    }
    case WM_NCHITTEST: {
        return processMouseMove(msg, result);
    }
    default:
        break;
    }

    return false;
}

bool WinFramelessWindowController::calculateWindowSize(MSG* message, long* result) const
{
    NCCALCSIZE_PARAMS& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(message->lParam);
    if (params.rgrc[0].top != 0) {
        params.rgrc[0].top -= 1;
    }

    /// NOTE: remove window frame
    *result = WVR_REDRAW;
    return true;
}

bool WinFramelessWindowController::processMouseMove(MSG* message, long* result) const
{
    const LONG borderWidth = 8;
    RECT winrect;
    GetWindowRect(message->hwnd, &winrect);

    long x = GET_X_LPARAM(message->lParam);
    long y = GET_Y_LPARAM(message->lParam);

    QRect titleBarMoveArea = windowTitleBarMoveArea();

    /// NOTE: titlebar`s move area
    if (x >= winrect.left + titleBarMoveArea.x() && x < winrect.left + titleBarMoveArea.x() + titleBarMoveArea.width()
        && y > winrect.top + borderWidth + titleBarMoveArea.y() && y < winrect.top + titleBarMoveArea.y() + titleBarMoveArea.height()) {
        *result = HTCAPTION;
        return true;
    }

    /// NOTE: bottom left corner
    if (x >= winrect.left && x < winrect.left + borderWidth
        && y < winrect.bottom && y >= winrect.bottom - borderWidth) {
        *result = HTBOTTOMLEFT;
        return true;
    }

    /// NOTE: bottom right corner
    if (x < winrect.right && x >= winrect.right - borderWidth
        && y < winrect.bottom && y >= winrect.bottom - borderWidth) {
        *result = HTBOTTOMRIGHT;
        return true;
    }

    /// NOTE: top left corner
    if (x >= winrect.left && x < winrect.left + borderWidth
        && y >= winrect.top && y < winrect.top + borderWidth) {
        *result = HTTOPLEFT;
        return true;
    }

    /// NOTE: top right corner
    if (x < winrect.right && x >= winrect.right - borderWidth
        && y >= winrect.top && y < winrect.top + borderWidth) {
        *result = HTTOPRIGHT;
        return true;
    }

    /// NOTE: left border
    if (x >= winrect.left && x < winrect.left + borderWidth) {
        *result = HTLEFT;
        return true;
    }

    /// NOTE: right border
    if (x < winrect.right && x >= winrect.right - borderWidth) {
        *result = HTRIGHT;
        return true;
    }

    /// NOTE: bottom border
    if (y < winrect.bottom && y >= winrect.bottom - borderWidth) {
        *result = HTBOTTOM;
        return true;
    }

    /// NOTE: top border
    if (y >= winrect.top && y < winrect.top + borderWidth) {
        *result = HTTOP;
        return true;
    }

    return false;
}

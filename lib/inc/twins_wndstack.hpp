/******************************************************************************
 * @brief   TWins - stack of windows
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once

#include "twins.hpp"
#include "twins_vector.hpp"

// -----------------------------------------------------------------------------

namespace twins
{

/** @brief struct holding window stack */
struct WndStack
{
    /** @brief show \p pWindow on top */
    void pushWnd(const twins::Widget *pWindow)
    {
        m_windows.append(pWindow);
        twins::drawWidget(pWindow);
        twins::flushBuffer();
    }

    /** @brief hide top-window */
    void popWnd()
    {
        if (m_windows.size())
        {
            m_windows.remove(m_windows.size()-1);
            redraw();
        }
    }

    /** @brief return top window */
    const twins::Widget *window()
    {
        assert(m_windows.size());
        return *m_windows.back();
    }

    /** @brief return top window state */
    twins::IWindowState *state()
    {
        return window()->window.getState();
    }

    /** @brief redraw window */
    void redraw()
    {
        for (auto p_wnd : m_windows)
        {
            twins::drawWidget(p_wnd);
            // signal that invalidate list must be cleared
            p_wnd->window.getState()->invalidate(WIDGET_ID_NONE);
        }
        twins::flushBuffer();
    }

    /** all windows iterator */
    const auto begin() { return m_windows.begin(); }
    const auto end()   { return m_windows.end(); }

private:
    twins::Vector<const twins::Widget *> m_windows;
};

// -----------------------------------------------------------------------------

} // namespace

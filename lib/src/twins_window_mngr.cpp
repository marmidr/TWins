/******************************************************************************
 * @brief   TWins - manager of windows
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#include "twins_window_mngr.hpp"

// -----------------------------------------------------------------------------

namespace twins
{

// -----------------------------------------------------------------------------

void WndManager::pushWnd(twins::IWindowState *pWindow)
{
    mWindows.append(pWindow);
    twins::resetInternalState();
    twins::drawWidget(pWindow->getWidgets());
}

void WndManager::popWnd()
{
    if (mWindows.size())
    {
        mWindows.remove(mWindows.size()-1);

        if (mWindows.size())
        {
            redrawAll();
        }
        else
        {
            twins::screenClrAll();
            twins::flushBuffer();
        }
    }
}

void WndManager::redrawAll()
{
    for (auto p_wnd : mWindows)
    {
        twins::drawWidget(p_wnd->getWidgets());
        // signal that invalidate list must be cleared
        p_wnd->invalidate(WIDGET_ID_NONE);
    }

    twins::flushBuffer();
}

// -----------------------------------------------------------------------------

}

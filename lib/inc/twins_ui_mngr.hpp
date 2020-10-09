/******************************************************************************
 * @brief   TWins - user interface manager
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once

#include "twins_common.hpp"
#include "twins_window_mngr.hpp"

// -----------------------------------------------------------------------------

namespace twins
{

/** @brief User Iterface Manager
 *         Attempt to organize dependency hell.
 */
class UIMngrBase
{
public:
    /** @brief safe, guaranted method to get the PAL */
    virtual twins::IPal& pal() = 0;
    /** @brief return window manager */
    virtual twins::WndManager& wMngr() = 0;
    /** @brief bottom-most window */
    virtual const twins::Widget* mainWnd() = 0;
};

UIMngrBase& uim();

// -----------------------------------------------------------------------------

} // namespace

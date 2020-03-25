/******************************************************************************
 * @brief   TWins - demo window definition
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins.hpp"

// -----------------------------------------------------------------------------

enum WndMainIDs
{
    ID_INVALID, // value 0 is reserved to WIDGET_ID_NONE
    ID_WND,
        ID_PGCONTROL,
            ID_PAGE_1,
                ID_PANEL_VERSIONS,
                    ID_LABEL_FW_VERSION,
                    ID_LABEL_DATE,
                    ID_LABEL_TIME,
                ID_PANEL_STATE,
                    ID_LED_PUMP,
                    ID_LED_LOCK,
                    ID_LED_BATTERY,
                ID_PANEL_KEY,
                    ID_LABEL_KEYSEQ,
                    ID_LABEL_KEYNAME,
                ID_CHBX_ENBL,
                ID_CHBX_LOCK,
                ID_BTN_YES,
                ID_BTN_NO,
                ID_BTN_CANCEL,
                ID_PRGBAR_1,
            ID_PAGE_2,
            ID_PAGE_3,
};

// -----------------------------------------------------------------------------

extern twins::IWindowState * getWindMainState();
extern const twins::Widget wndMain;

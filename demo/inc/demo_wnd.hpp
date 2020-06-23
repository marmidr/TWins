/******************************************************************************
 * @brief   TWins - demo window definition
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once
#include "twins.hpp"

// -----------------------------------------------------------------------------

/** Main window widgets unique id's */
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
                ID_BTN_POPUP,
                ID_PRGBAR_1,
                ID_PRGBAR_2,
                ID_PRGBAR_3,
            ID_PAGE_2,
                ID_LABEL_LBHELP,
                ID_LISTBOX,
                ID_RADIO_1,
                ID_RADIO_2,
                ID_RADIO_3,
            ID_PAGE_3,
                ID_PANEL_EDT,
                    ID_EDT_1,
                    ID_EDT_2,
                ID_CUSTOMWGT1,
                ID_PANEL_CHBX,
                    ID_LBL_CHBXTITLE,
                    ID_CHBX_A,
                    ID_CHBX_B,
                    ID_CHBX_C,
                    ID_CHBX_D,
            ID_PAGE_4,
                ID_PANEL_EMPTY_1,
                    ID_LBL_EMPTY_1,
                ID_PANEL_EMPTY_2,
                    ID_LBL_EMPTY_2,
            ID_PAGE_5,
                ID_TBX_LOREMIPSUM,
        ID_LABEL_FTR,
};


// popup window IDs
enum WndYesNoIDs
{
    IDYN_INVALID,
    IDYN_WND,
        IDYN_LBL_MSG,
        IDYN_BTN_YES,
        IDYN_BTN_NO,
        IDYN_BTN_CANCEL
};

// -----------------------------------------------------------------------------

extern twins::IWindowState * getWndMainState();
extern const twins::Widget * pWndMainWidgets;
extern const uint16_t wndMainNumPages;

extern twins::IWindowState * getWndYesNoState();
extern const twins::Widget * pWndYesNoWidgets;

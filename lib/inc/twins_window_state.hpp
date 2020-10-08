/******************************************************************************
 * @brief   TWins - IWindowState definition
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *****************************************************************************/

#pragma once

/// @note Do not include this file directly!

// -----------------------------------------------------------------------------

/** @brief Forward declaration */
struct Widget;

template <class T>
class Vector;

/** @brief Window state and event handler */
class IWindowState
{
public:
    virtual ~IWindowState() = default;
    virtual void init(const twins::Widget *pWindowWgts) {}
    virtual const twins::Widget *getWidgets() const { return {}; }
    // events
    virtual void onButtonDown(const twins::Widget* pWgt) {}
    virtual void onButtonUp(const twins::Widget* pWgt) {}
    virtual void onEditChange(const twins::Widget* pWgt, twins::String &&str) {}
    virtual bool onEditInputEvt(const twins::Widget* pWgt, const twins::KeyCode &kc, twins::String &str, int16_t &cursorPos) { return false; }
    virtual void onCheckboxToggle(const twins::Widget* pWgt) {}
    virtual void onPageControlPageChange(const twins::Widget* pWgt, uint8_t newPageIdx) {}
    virtual void onListBoxSelect(const twins::Widget* pWgt, int16_t selIdx) {}
    virtual void onListBoxChange(const twins::Widget* pWgt, int16_t newIdx) {}
    virtual void onComboBoxSelect(const twins::Widget* pWgt, int16_t selIdx) {}
    virtual void onComboBoxChange(const twins::Widget* pWgt, int16_t newIdx) {}
    virtual void onComboBoxDrop(const twins::Widget* pWgt, bool dropState) {}
    virtual void onRadioSelect(const twins::Widget* pWgt) {}
    virtual void onCustomWidgetDraw(const twins::Widget* pWgt) {}
    virtual bool onCustomWidgetInputEvt(const twins::Widget* pWgt, const twins::KeyCode &kc) { return false; }
    virtual bool onWindowUnhandledInputEvt(const twins::Widget* pWgt, const twins::KeyCode &kc) { return false; }
    // common state queries
    virtual bool isEnabled(const twins::Widget* pWgt) { return true; }
    virtual bool isFocused(const twins::Widget* pWgt) { return false; }
    virtual bool isVisible(const twins::Widget* pWgt) { return true; }
    virtual twins::WID& getFocusedID() = 0;
    // widget-specific queries
    virtual void getWindowCoord(const twins::Widget* pWgt, twins::Coord &coord) {}
    virtual void getWindowTitle(const twins::Widget* pWgt, twins::String &title) {}
    virtual bool getCheckboxChecked(const twins::Widget* pWgt) { return false; }
    virtual void getLabelText(const twins::Widget* pWgt, twins::String &out) {}
    virtual void getEditText(const twins::Widget* pWgt, twins::String &out) {}
    virtual bool getLedLit(const twins::Widget* pWgt) { return false; }
    virtual void getLedText(const twins::Widget* pWgt, twins::String &out) {}
    virtual void getProgressBarState(const twins::Widget* pWgt, int &pos, int &max) {}
    virtual int  getPageCtrlPageIndex(const twins::Widget* pWgt) { return 0; }
    virtual void getListBoxState(const twins::Widget* pWgt, int16_t &itemIdx, int16_t &selIdx, int16_t &itemsCount) {}
    virtual void getListBoxItem(const twins::Widget* pWgt, int itemIdx, twins::String &out) {}
    virtual void getComboBoxState(const twins::Widget* pWgt, int16_t &itemIdx, int16_t &selIdx, int16_t &itemsCount, bool &dropDown) {}
    virtual void getComboBoxItem(const twins::Widget* pWgt, int itemIdx, twins::String &out) {}
    virtual int  getRadioIndex(const twins::Widget* pWgt) { return -1; }
    virtual void getTextBoxLines(const twins::Widget* pWgt, const twins::Vector<twins::StringRange> **ppLines, bool &changed) {}
    // requests
    virtual void invalidate(twins::WID id, bool instantly = false) {}
};

// -----------------------------------------------------------------------------

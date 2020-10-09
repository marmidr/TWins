/******************************************************************************
 * @brief   TWins - window definition constexpr transformation
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *          Proof of concept: https://godbolt.org/z/TkMfcC
 *****************************************************************************/

#pragma once

// field set in user Widget definition;
#define __TWINS_LINK_SECRET     const Widget *pChilds

#include "twins.hpp"

// -----------------------------------------------------------------------------

namespace twins
{

/**
 * @brief Count all widgets in pointed window (or any other type of widget)
 */
constexpr int getWgtsCount(const twins::Widget *pWgt)
{
    int n = 1;

    for (const auto *ch = pWgt->link.pChilds; ch && ch->id != twins::WIDGET_ID_NONE; ch++)
        n += getWgtsCount(ch);

    return n;
}

/**
 * @brief Count all Pages in pointed window
 */
constexpr int getPagesCount(const twins::Widget *pWgt)
{
    int n = pWgt->type == twins::Widget::Page;

    for (const auto *ch = pWgt->link.pChilds; ch && ch->id != twins::WIDGET_ID_NONE; ch++)
        n += getWgtsCount(ch);

    return n;
}

/**
 * @brief Recurrent widget transformation function
 *        Widgets that belongs to the same parent are put together, next to each other, to ease TAB navigation.
 * @par arr Destination array of widgets
 * @par pWgt Pointer to Window - the root widget
 */
template<unsigned N>
constexpr int transformWidgetTreeToArray(twins::Array<twins::Widget, N> &arr, const twins::Widget* pWgt, const int wgtIdx, int freeSlotIdx)
{
    int child_idx = freeSlotIdx;

    // copy widget
    arr[wgtIdx] = *pWgt;
    arr[wgtIdx].link = {};
    arr[wgtIdx].link.ownIdx = wgtIdx;

    // count the childs
    int n_childs = 0;
    for (auto *p_child = pWgt->link.pChilds; p_child && p_child->id != twins::WIDGET_ID_NONE; p_child++)
        n_childs++;

    arr[wgtIdx].link.childsIdx = child_idx;
    arr[wgtIdx].link.childsCnt = n_childs;
    freeSlotIdx += n_childs;

    // copy childs
    for (auto *p_child = pWgt->link.pChilds; p_child && p_child->id != twins::WIDGET_ID_NONE; p_child++)
    {
        freeSlotIdx = transformWidgetTreeToArray<N>(arr, p_child, child_idx, freeSlotIdx);
        arr[child_idx].link.parentIdx = wgtIdx;
        child_idx++;
    }

    return freeSlotIdx;
}

template<const twins::Widget *pWINDOW, int N = getWgtsCount(pWINDOW) + 1>
constexpr twins::Array<twins::Widget, N> transforWindowDefinition()
{
    static_assert(N < 65000, "Limit of widgets per window reached");
    twins::Array<twins::Widget, N> arr;

    transformWidgetTreeToArray<N>(arr, pWINDOW, 0, 1);
    return arr;
}

// -----------------------------------------------------------------------------

} // namespace

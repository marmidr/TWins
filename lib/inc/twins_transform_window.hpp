/******************************************************************************
 * @brief   TWins - window definition constexpr transformation
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 *          Proof of concept: https://godbolt.org/z/TkMfcC
 *****************************************************************************/

#pragma once

// field set in user Widget definition;
// because it's first field in union, it's name may be ommited
#define __TWINS_LINK_SECRET     const Widget *pChilds

#include "twins.hpp"

// -----------------------------------------------------------------------------

namespace twins
{

constexpr int getWgtsCount(const twins::Widget *w)
{
    int n = 1;

    for (const auto *ch = w->link.pChilds; ch && ch->id != twins::WIDGET_ID_NONE; ch++)
        n += getWgtsCount(ch);

    return n;
}

template<unsigned N>
constexpr int copyTransformWidget(twins::Array<twins::Widget, N> &arr, const twins::Widget* pWgt, const int wgtIdx, int freeSlotIdx)
{
    int child_idx = freeSlotIdx;

    // copy widget
    arr[wgtIdx] = *pWgt;
    arr[wgtIdx].link = {};

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
        freeSlotIdx = copyTransformWidget<N>(arr, p_child, child_idx, freeSlotIdx);
        arr[child_idx].link.parentIdx = wgtIdx;
        child_idx++;
    }

    return freeSlotIdx;
}

template<const twins::Widget *pWINDOW, unsigned N = getWgtsCount(pWINDOW) + 1>
constexpr twins::Array<twins::Widget, N> transforWindowDefinition()
{
    twins::Array<twins::Widget, N> arr;

    copyTransformWidget<N>(arr, pWINDOW, 0, 1);
    return arr;
}

// -----------------------------------------------------------------------------

} // namespace

#include "pch.h"
#include "TabView.h"

typedef struct tagTABVIEW
{
    HWND hWnd;
    int nViewBorder;
    int nTabHeight;
    HWND hWndTabCtrl;
} TABVIEW, *LPTABVIEW;

#define TABVIEW_PROP "TabViewData"
#define DEFAULT_TAB_HEIGHT 40
#define DEFAULT_VIEW_BORDER 6

static LRESULT AddTab(LPTABVIEW pTabView, LPTVWITEM lptvwitem);
static LRESULT RemoveTab(LPTABVIEW pTabView, int index);
static LRESULT OnEraseBkgnd(LPTABVIEW pTabView, WPARAM wParam);
static LRESULT SetActiveTab(LPTABVIEW pTabView, int index);
static LRESULT OnNotify(LPTABVIEW pTabView, int idFrom, LPNMHDR pnmhdr);
static LRESULT OnContextMenu(LPTABVIEW pTabView, WPARAM wParam, LPARAM lParam);
static LRESULT SetView(LPTABVIEW pTabView, int index, HWND hWndView);
static void RemoveProps(const TABVIEW* pTabview);
static void UpdateLayout(LPTABVIEW pTabView, WPARAM type, LPARAM lParam);
static void UpdateClientLayout(LPTABVIEW pTabView);

LRESULT CALLBACK TabViewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    TABVIEW* pTabView = GetProp(hWnd, TABVIEW_PROP);

    switch (msg) {
    case WM_CREATE:
        break;
    case WM_NCCREATE:
        pTabView = GlobalAlloc(GPTR, sizeof(TABVIEW));
        if (!pTabView) {
            return -1;
        }

        pTabView->hWnd = hWnd;
        pTabView->nViewBorder = DEFAULT_VIEW_BORDER;
        pTabView->nTabHeight = DEFAULT_TAB_HEIGHT;
        SetProp(hWnd, TABVIEW_PROP, pTabView);
        result = 1;
        break;
    case TVWM_SETTABCTRL:
        if (pTabView) {
            pTabView->hWndTabCtrl = (HWND)lParam;
        }
        break;
    case TVWM_ADDTAB:
        result = AddTab(pTabView, (LPTVWITEM)lParam);
        break;
    case TVWM_SETACTIVETAB:
        result = SetActiveTab(pTabView, (int)wParam);
        break;
    case TVWM_SETVIEWBORDER:
        if (pTabView) {
            pTabView->nViewBorder = (int)wParam;
            UpdateClientLayout(pTabView);
        }
        break;
    case TVWM_GETVIEWBORDER:
        if (pTabView) {
            result = pTabView->nViewBorder;
        }
        break;
    case TVWM_SETTABHEIGHT:
        if (pTabView) {
            pTabView->nTabHeight = (int)wParam;
            UpdateClientLayout(pTabView);
        }
        break;
    case TVWM_GETTABHEIGHT:
        if (pTabView) {
            result = pTabView->nTabHeight;
        }
        break;
    case TVWM_GETTABCOUNT:
        if (pTabView) {
            result = TabCtrl_GetItemCount(pTabView->hWndTabCtrl);
        }
        break;
    case TVWM_SETTABITEM:
        if (pTabView) {
            LPTCITEM pitem = (LPTCITEM)lParam;
            result = TabCtrl_SetItem(pTabView->hWndTabCtrl, (int)wParam, pitem);
        }
        break;
    case TVWM_GETTABITEM:
        if (pTabView) {
            LPTCITEM pitem = (LPTCITEM)lParam;
            result = TabCtrl_GetItem(pTabView->hWndTabCtrl, (int)wParam, pitem);
        }
        break;
    case TVWM_REMOVETAB:
        result = RemoveTab(pTabView, (int)wParam);
        break;
    case TVWM_GETVIEW:
        if (pTabView) {
            result = (LRESULT)GetProp(pTabView->hWndTabCtrl, MAKEINTATOM((int)wParam + 1));
        }
        break;
    case TVWM_SETVIEW:
        if (pTabView) {
            result = SetView(pTabView, (int)wParam, (HWND)lParam);
        }
        break;
    case TVWM_HITTEST:
        if (pTabView) {
            TCHITTESTINFO hti;
            hti.pt.x = LOWORD(lParam);
            hti.pt.y = HIWORD(lParam);
            result = TabCtrl_HitTest(pTabView->hWndTabCtrl, &hti);
        }
        break;
    case WM_SIZE:
        UpdateLayout(pTabView, wParam, lParam);
        break;
    case WM_DESTROY:
        pTabView = (TABVIEW*)GetProp(hWnd, TABVIEW_PROP);
        if (pTabView) {
            RemoveProps(pTabView);
            GlobalFree(pTabView);
        }
        break;
    case WM_ERASEBKGND:
        result = OnEraseBkgnd(pTabView, wParam);
        break;
    case WM_NOTIFY:
        result = OnNotify(pTabView, (int)wParam, (LPNMHDR)lParam);
        break;
    case WM_CONTEXTMENU:
        result = OnContextMenu(pTabView, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return result;
}

BOOL InitTabView(void)
{
    static LONG bTabViewRegistered = 0;

    if (InterlockedCompareExchange(&bTabViewRegistered, 1, 0) == 0) {
        WNDCLASS wc;

        memset(&wc, 0, sizeof(WNDCLASS));

        wc.lpfnWndProc = TabViewProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = WC_TABVIEW;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
        if (!RegisterClass(&wc)) {
            return FALSE;
        }
    }

    return TRUE;
}

LRESULT AddTab(LPTABVIEW pTabView, LPTVWITEM lptvwitem)
{
    if (pTabView->hWndTabCtrl) {
        int nCount = TabCtrl_GetItemCount(pTabView->hWndTabCtrl);
        int index = TabCtrl_InsertItem(pTabView->hWndTabCtrl, nCount, &lptvwitem->tci);
        if (index != -1) {
            SetProp(pTabView->hWndTabCtrl, MAKEINTATOM(index + 1), lptvwitem->hWndView);
            return 0;
        }
    }

    return 1;
}

LRESULT RemoveTab(LPTABVIEW pTabView, int index)
{
    LRESULT result;
    int nCount;

    if (!pTabView->hWndTabCtrl) {
        return 0;
    }

    HWND hWndView = GetProp(pTabView->hWndTabCtrl, MAKEINTATOM(index + 1));
    if (hWndView) {
        RemoveProp(pTabView->hWndTabCtrl, MAKEINTATOM(index + 1));
        DestroyWindow(hWndView);
    }

    result = TabCtrl_DeleteItem(pTabView->hWndTabCtrl, index);
    if (!result) {
        return result;
    }

    // Update property indexes
    nCount = TabCtrl_GetItemCount(pTabView->hWndTabCtrl);
    for (int i = index; i < nCount; i++) {
        HWND hWnd = GetProp(pTabView->hWndTabCtrl, MAKEINTATOM(i + 2));
        RemoveProp(pTabView->hWndTabCtrl, MAKEINTATOM(i + 2));
        SetProp(pTabView->hWndTabCtrl, MAKEINTATOM(i + 1), hWnd);
    }

    if (nCount > 0) {
        result = SetActiveTab(pTabView, 0);
    }

    return result;
}

void UpdateLayout(LPTABVIEW pTabView, WPARAM type, LPARAM lParam)
{
    RECT rcClient, rcTab, rcView;
    HWND hWndView;
    int nCurSel;

    int cx = LOWORD(lParam);
    int cy = HIWORD(lParam);

    SetWindowPos(pTabView->hWnd, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

    if (!pTabView->hWndTabCtrl) {
        return;
    }

    rcClient.left = 0;
    rcClient.top = 0;
    rcClient.right = cx;
    rcClient.bottom = cy;

    // Tab Control
    SetRect(&rcTab, rcClient.left, rcClient.top, rcClient.right, rcClient.top + pTabView->nTabHeight);
    MoveWindow(pTabView->hWndTabCtrl, rcTab.left, rcTab.top, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top,
               TRUE);

    // View
    nCurSel = TabCtrl_GetCurSel(pTabView->hWndTabCtrl);
    hWndView = GetProp(pTabView->hWndTabCtrl, MAKEINTATOM(nCurSel + 1));
    if (hWndView) {
        SetRect(&rcView, rcClient.left + pTabView->nViewBorder, rcTab.bottom + pTabView->nViewBorder,
                rcClient.right - pTabView->nViewBorder - 1, rcClient.bottom - pTabView->nViewBorder - 1);
        MoveWindow(hWndView, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, TRUE);
    }
}

void UpdateClientLayout(LPTABVIEW pTabView)
{
    RECT rcClient;
    GetClientRect(pTabView->hWnd, &rcClient);
    UpdateLayout(pTabView, SIZE_RESTORED, MAKELPARAM(rcClient.right, rcClient.bottom));
}

LRESULT OnEraseBkgnd(LPTABVIEW pTabView, WPARAM wParam)
{
    HDC hDC = (HDC)wParam;
    RECT rcClient;

    GetClipBox(hDC, &rcClient);

    FillRect(hDC, &rcClient, (HBRUSH)(COLOR_APPWORKSPACE + 1));

    return TRUE;
}

void RemoveProps(const TABVIEW* pTabview)
{
    int nCount = TabCtrl_GetItemCount(pTabview->hWndTabCtrl);
    for (int i = 0; i < nCount; i++) {
        RemoveProp(pTabview->hWndTabCtrl, MAKEINTATOM(i + 1));
    }

    RemoveProp(pTabview->hWnd, TABVIEW_PROP);
}

LRESULT SetActiveTab(LPTABVIEW pTabView, int index)
{
    if (!pTabView->hWndTabCtrl) {
        return 0;
    }

    int nCount = TabCtrl_GetItemCount(pTabView->hWndTabCtrl);

    for (int i = 0; i < nCount; i++) {
        HWND hWndView = GetProp(pTabView->hWndTabCtrl, MAKEINTATOM(i + 1));

        if (i == index) {
            TabCtrl_SetCurSel(pTabView->hWndTabCtrl, i);
            ShowWindow(hWndView, SW_SHOW);
        } else {
            ShowWindow(hWndView, SW_HIDE);
        }
    }

    UpdateClientLayout(pTabView);

    return 1;
}

LRESULT OnNotify(LPTABVIEW pTabView, int idFrom, LPNMHDR pnmhdr)
{
    if (pnmhdr->hwndFrom != pTabView->hWndTabCtrl) {
        return 0;
    }

    if (pnmhdr->code == TCN_SELCHANGE) {
        int index = TabCtrl_GetCurSel(pTabView->hWndTabCtrl);
        SetActiveTab(pTabView, index);
    }

    // Forward notifications to parent window
    return SendMessage(GetParent(pTabView->hWnd), WM_NOTIFY, idFrom, (LPARAM)pnmhdr);
}

LRESULT OnContextMenu(LPTABVIEW pTabView, WPARAM wParam, LPARAM lParam)
{
    TVWCONTEXTMENUINFO cmInfo;
    TCHITTESTINFO hti;
    HWND hWnd = (HWND)wParam;
    POINT pt;
    int nCurSel;

    if (hWnd != pTabView->hWndTabCtrl) {
        return 0;
    }

    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);

    hti.pt = pt;
    hti.flags = 0;

    ScreenToClient(pTabView->hWndTabCtrl, &hti.pt);

    nCurSel = TabCtrl_HitTest(pTabView->hWndTabCtrl, &hti);
    if (nCurSel == -1) {
        return 0;
    }

    cmInfo.hdr.hwndFrom = pTabView->hWndTabCtrl;
    cmInfo.hdr.idFrom = nCurSel;
    cmInfo.hdr.code = TVWN_CONTEXTMENU;
    cmInfo.pt = pt;

    SendMessage(GetParent(pTabView->hWnd), WM_NOTIFY, TVWN_CONTEXTMENU, (LPARAM)&cmInfo);

    return 0;
}

LRESULT SetView(LPTABVIEW pTabView, int index, HWND hWndView)
{
    if (!pTabView->hWndTabCtrl) {
        return 0;
    }

    HWND hWndOldView = GetProp(pTabView->hWndTabCtrl, MAKEINTATOM(index + 1));
    if (hWndOldView) {
        RemoveProp(pTabView->hWndTabCtrl, MAKEINTATOM(index + 1));
        DestroyWindow(hWndOldView);
    }

    SetProp(pTabView->hWndTabCtrl, MAKEINTATOM(index + 1), hWndView);

    UpdateClientLayout(pTabView);

    return 1;
}

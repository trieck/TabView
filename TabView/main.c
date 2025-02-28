
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <stdio.h>
#include "TabView.h"

#pragma comment(lib, "comctl32.lib")

// Global Variables
HWND hWndMain = NULL;   // this should fail in C89
HWND hWndTabView = NULL;
HFONT hFont = NULL;

// Function Prototypes
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static HWND CreateTabView(HWND hWndParent);
static HWND CreateTabControl(HWND hWndParent);
static HWND CreateView(HWND hWndParent, int index);
static BOOL OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void UpdateLayout(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT OnNotify(HWND hWnd, int idFrom, LPNMHDR pnmhdr);

#define TAB_VIEW_FRAME_CLASS_NAME "TabViewFrame"
#define TAB_CLOSE_MENU_ID 0x001

// Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    WNDCLASS wc;
    INITCOMMONCONTROLSEX icex;
    MSG msg;

    // Ensure `comctl32.dll` is loaded (for Windows 95)
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    if (!InitCommonControlsEx(&icex)) {
        return -1;
    }

    if (!InitTabView()) {
        return -1;
    }

    memset(&wc, 0, sizeof(WNDCLASS));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = TAB_VIEW_FRAME_CLASS_NAME;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
    if (!RegisterClass(&wc)) {
        return FALSE;
    }

    hWndMain = CreateWindow(TAB_VIEW_FRAME_CLASS_NAME, "Win32 SDK Tab View Test",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, NULL, hInstance, NULL);

    if (!hWndMain) {
        return -1;
    }

    ShowWindow(hWndMain, nShowCmd);
    UpdateWindow(hWndMain);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)(msg.wParam);
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (msg) {
    case WM_CREATE:
        if (!OnCreate(hWnd, wParam, lParam)) {
            return -1;
        }
        break;
    case WM_SIZE:
        UpdateLayout(hWnd, wParam, lParam);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_NOTIFY:
        result = OnNotify(hWnd, (int)wParam, (LPNMHDR)lParam);
        break;
    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return result;
}

HWND CreateTabView(HWND hWndParent)
{
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    HWND hWnd = CreateWindow(WC_TABVIEW, "", dwStyle, 0, 0, 0, 0, hWndParent, NULL, GetModuleHandle(NULL), NULL);
    if (!hWnd) {
        return NULL;
    }

    return hWnd;
}

HWND CreateTabControl(HWND hWndParent)
{
    HWND hWnd = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                             0, 0, 0, 0, hWndParent, (HMENU)0x1001, GetModuleHandle(NULL), NULL);
    if (!hWnd) {
        return NULL;
    }

    SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
        
    return hWnd;
}

HWND CreateView(HWND hWndParent, int index)
{
    char text[256];

    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL;
    DWORD dwExStyle = WS_EX_CLIENTEDGE;

    HWND hWnd = CreateWindowEx(dwExStyle, "EDIT", "", dwStyle, 0, 0, 0, 0, hWndParent, NULL, GetModuleHandle(NULL), NULL);
    if (!hWnd) {
        return NULL;
    }

    sprintf(text, "View #%d", index + 1);

    SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)text);

    return hWnd;
}

BOOL OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    TVWITEM tvi;
    HWND hWndTab;
    HWND hWndView;
    LOGFONT lf;
    char tabTitle[256], text[256];
	int nTabHeight, nViewBorder;
	int i;

    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfHeight = 18;
    lf.lfWeight = FW_SEMIBOLD;
    strcpy(lf.lfFaceName, "Tahoma");

    memset(&tvi, 0, sizeof(TVWITEM));
    tvi.tci.mask = TCIF_TEXT;
    tvi.tci.pszText = tabTitle;
    tvi.tci.cchTextMax = sizeof(tabTitle);

    hFont = CreateFontIndirect(&lf);
    if (!hFont) {
        return FALSE;
    }

    hWndTabView = CreateTabView(hWnd);
    if (!hWndTabView) {
        return FALSE;
    }

    hWndTab = CreateTabControl(hWndTabView);
    if (!hWndTab) {
        return FALSE;
    }
        
    SendMessage(hWndTabView, TVWM_SETTABCTRL, 0, (LPARAM)hWndTab);

    nTabHeight = (int)SendMessage(hWndTabView, TVWM_GETTABHEIGHT, 0, 0);
    SendMessage(hWndTabView, TVWM_SETTABHEIGHT, nTabHeight + 5, 0);

    nViewBorder = (int)SendMessage(hWndTabView, TVWM_GETVIEWBORDER, 0, 0);
    SendMessage(hWndTabView, TVWM_SETVIEWBORDER, nViewBorder + 3, 0);

    for (i = 0; i < 10; ++i) {
        sprintf(tabTitle, "Tab #%d", i + 1);
        hWndView = CreateView(hWndTabView, i);
        if (!hWndView) {
            return FALSE;
        }

        tvi.tci.pszText = tabTitle;
        tvi.hWndView = hWndView;
        SendMessage(hWndTabView, TVWM_ADDTAB, 0, (LPARAM)&tvi);

        sprintf(text, "*New* View #%d", i + 1);
        SendMessage(hWndView, WM_SETTEXT, 0, (LPARAM)text);
    }
    
    return TRUE;
}

void UpdateLayout(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    SendMessage(hWndTabView, WM_SIZE, wParam, lParam);
}

LRESULT OnNotify(HWND hWnd, int idFrom, LPNMHDR pnmhdr)
{
	if (pnmhdr->code == TVWN_CONTEXTMENU) {
        HMENU hMenu;
        TVWCONTEXTMENUINFO* pcmi = (TVWCONTEXTMENUINFO*)pnmhdr;
        POINT pt = pcmi->pt;
        HWND hWndTabCtrl = pcmi->hdr.hwndFrom;
        int index = (int)pcmi->hdr.idFrom;
        int result;

        hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, 1, "Close Tab");

        result = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL | TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hWndTabCtrl, NULL);
        if (result == TAB_CLOSE_MENU_ID) {
            SendMessage(hWndTabView, TVWM_REMOVETAB, index, 0);
        }

        DestroyMenu(hMenu);
    }

    return 0;
}

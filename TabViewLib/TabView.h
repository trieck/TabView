#pragma once

//====== TAB VIEW CONTROL ==========================================================

#ifdef _UNICODE
#define WC_TABVIEW L"TabView"
#else
#define WC_TABVIEW "TabView"
#endif

// TabView Control Styles
#define TVS_NONE            0x00000000

// Windows Messages
#define TVWM_FIRST           (WM_USER + 0x1000)
#define TVWM_SETTABCTRL      (TVWM_FIRST + 0)
#define TVWM_ADDTAB          (TVWM_FIRST + 1)
#define TVWM_REMOVETAB       (TVWM_FIRST + 2)
#define TVWM_SETACTIVETAB    (TVWM_FIRST + 3)
#define TVWM_GETACTIVETAB    (TVWM_FIRST + 4)
#define TVWM_GETVIEW         (TVWM_FIRST + 5)
#define TVWM_SETVIEW         (TVWM_FIRST + 6)
#define TVWM_SETTABITEM      (TVWM_FIRST + 7)
#define TVWM_GETTABITEM      (TVWM_FIRST + 8)
#define TVWM_HITTEST         (TVWM_FIRST + 9)
#define TVWM_SETVIEWBORDER   (TVWM_FIRST + 10)
#define TVWM_GETVIEWBORDER   (TVWM_FIRST + 11)
#define TVWM_SETTABHEIGHT    (TVWM_FIRST + 12)
#define TVWM_GETTABHEIGHT    (TVWM_FIRST + 13)
#define TVWM_GETTABCOUNT     (TVWM_FIRST + 16)

// TabView Control Notifications
#define TVWN_FIRST           (0U - 0x1000)
#define TVWN_SELCHANGING      (TVWN_FIRST - 1)
#define TVWN_SELCHANGED       (TVWN_FIRST - 2)
#define TVWN_CONTEXTMENU      (TVWN_FIRST - 3)

// TabView Control Structures
typedef struct tagTVWITEM {
    TCITEM tci;
    HWND hWndView;
} TVWITEM, *LPTVWITEM;

typedef struct tagTVWCONTEXTMENUINFO
{
    NMHDR hdr;
    POINT pt;
} TVWCONTEXTMENUINFO, * LPTVWCONTEXTMENUINFO;

// TabView Control Functions
BOOL InitTabView(void);

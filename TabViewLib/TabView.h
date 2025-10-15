#pragma once

//====== TAB VIEW CONTROL ==========================================================
#define WC_TABVIEW _T("TabView")

// TabView Control Styles
#define TVS_NONE            0x00000000

// Windows Messages
#define TVWM_FIRST           (WM_USER + 0x1000)

// Tab Control Management
#define TVWM_SETTABCTRL      (TVWM_FIRST + 0)
#define TVWM_GETTABCTRL      (TVWM_FIRST + 1)
#define TVWM_SHOWTABCTRL     (TVWM_FIRST + 2)

// Tab Management
#define TVWM_ADDTABA         (TVWM_FIRST + 3)
#define TVWM_ADDTABW         (TVWM_FIRST + 4)
#define TVWM_REMOVETAB       (TVWM_FIRST + 5)
#define TVWM_SETACTIVETAB    (TVWM_FIRST + 6)
#define TVWM_GETACTIVETAB    (TVWM_FIRST + 7)
#define TVWM_GETTABCOUNT     (TVWM_FIRST + 8)

// View Management
#define TVWM_GETVIEW         (TVWM_FIRST + 9)
#define TVWM_SETVIEW         (TVWM_FIRST + 10)
#define TVWM_SETVIEWBORDER   (TVWM_FIRST + 11)
#define TVWM_GETVIEWBORDER   (TVWM_FIRST + 12)
#define TVWM_SETBKCOLOR      (TVWM_FIRST + 13)
#define TVWM_GETBKCOLOR      (TVWM_FIRST + 14)

// Tab Item Management
#define TVWM_SETTABITEMA     (TVWM_FIRST + 15)
#define TVWM_SETTABITEMW     (TVWM_FIRST + 16)
#define TVWM_GETTABITEMA     (TVWM_FIRST + 17)
#define TVWM_GETTABITEMW     (TVWM_FIRST + 18)

// Tab Appearance
#define TVWM_SETTABHEIGHT    (TVWM_FIRST + 19)
#define TVWM_GETTABHEIGHT    (TVWM_FIRST + 20)
#define TVWM_SETTOPMARGIN    (TVWM_FIRST + 21)
#define TVWM_GETTOPMARGIN    (TVWM_FIRST + 22)

// Miscellaneous
#define TVWM_HITTEST         (TVWM_FIRST + 23)

#ifdef _UNICODE
#define TVWM_ADDTAB TVWM_ADDTABW
#define TVWM_SETTABITEM TVWM_SETTABITEMW
#define TVWM_GETTABITEM TVWM_GETTABITEMW
#else
#define TVWM_ADDTAB TVWM_ADDTABA
#define TVWM_SETTABITEM TVWM_SETTABITEMA
#define TVWM_GETTABITEM TVWM_GETTABITEMA
#endif

// TabView Control Structures
typedef struct tagTVWITEM
{
    TCITEM tci;
    HWND hWndView;
} TVWITEM, *LPTVWITEM;

typedef struct tagTVWCONTEXTMENUINFO
{
    NMHDR hdr;
    POINT pt;
} TVWCONTEXTMENUINFO, *LPTVWCONTEXTMENUINFO;

// TabView Control Functions

#ifdef __cplusplus
extern "C" {
#endif

BOOL WINAPI InitTabView(void);

#ifdef __cplusplus
}
#endif

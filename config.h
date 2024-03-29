#include "X11/XF86keysym.h"
#include "gaplessgrid.c"

static const unsigned int borderpx        = 1;        /* 窗口边框大小 */
static const unsigned int gappx           = 4;        /* 窗口间距 */
static const unsigned int snap            = 32;       /* snap pixel */
static const int showbar                  = 1;        /* 是否显示状态栏 */
static const int topbar                   = 1;        /* 状态栏显示位置 0 底部 1 顶部 */
static const unsigned int systraypinning  = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing  = 2;   /* systray spacing */
static const int systraypinningfailfirst  = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray              = 1;   /* 是否显示系统托盘 */

static const char *fonts[]                = { "JetBrainsMono Nerd Font:style=medium:size=13",
					      "WenQuanYi Micro Hei:style=Regular:size=13:antialias=true:autohint=true" };
static const char dmenufont[]             = "JetBrainsMono Nerd Font:size=13";
static const char col_gray1[]             = "#222222";
static const char col_gray2[]             = "#444444";
static const char col_gray3[]             = "#bbbbbb";
static const char col_gray4[]             = "#eeeeee";
static const char col_cyan[]              = "#005577";
static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};
static const unsigned int alphas[][3]      = {
    /*               fg      bg        border*/
    [SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = {" ", " ", " ", " ", " ", " ", " ", " ", " " };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            0,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.5; /* 平铺模式主窗口占比 [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "###",      gaplessgrid },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask        /* Alt */
#define MODKEY_SUPER Mod4Mask  /* Super */
#define TAGKEYS(KEY,TAG) \
	{ MODKEY_SUPER,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY_SUPER|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY_SUPER|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY_SUPER|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *alacrittycmd[]  = { "alacritty", NULL };
static const char *roficmd[]  = { "rofi", "-show", "drun", NULL };
static const char *touchpadctl[]  = { "/home/david/Downloads/dwm/tpctl.py", NULL };

static const Key keys[] = {
	/* modifier         		key        function        argument */
	{ MODKEY,                       XK_Return, spawn,          {.v = alacrittycmd } },    /* 打开终端 */
	{ MODKEY,                       XK_r,      spawn,          {.v = roficmd } },         /* 打开rofi */
	{ MODKEY_SUPER,                 XK_s,      togglebar,      {0} },                     /* 开启/关闭状态栏 */
	{ MODKEY_SUPER,                 XK_w,      focusstack,     {.i = +1 } },              /* 切换窗口焦点 */
	{ MODKEY_SUPER|ShiftMask,       XK_w,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY_SUPER,                 XK_Return, zoom,           {0} },                     /* 当前窗口置为主窗口 */
	{ MODKEY_SUPER,                 XK_Tab,    view,           {0} },                     /* 当前Tag与上一个Tag之间切换 */
	{ MODKEY_SUPER,                 XK_q,      killclient,     {0} },                     /* 关闭当前窗口 */
	{ MODKEY_SUPER,                 XK_t,      setlayout,      {.v = &layouts[0]} },      /* 平铺模式*/
	{ MODKEY_SUPER,                 XK_g,      setlayout,      {.v = &layouts[1]} },      /* 网格模式*/
	{ MODKEY_SUPER,                 XK_f,      setlayout,      {.v = &layouts[2]} },      /* 浮动模式*/
	{ MODKEY_SUPER,                 XK_m,      setlayout,      {.v = &layouts[3]} },      /* 全屏模式*/
	{ MODKEY_SUPER,                 XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY_SUPER,                 XK_Escape, quit,           {0} },                    /* 退出dwm */
	{ 0,            XF86XK_AudioRaiseVolume,   spawn,          SHCMD("amixer sset Master 5%+ unmute") },
	{ 0,            XF86XK_AudioLowerVolume,   spawn,          SHCMD("amixer sset Master 5%- unmute") },
	{ 0,            XF86XK_AudioMute,          spawn,          SHCMD("amixer sset Master toggle") },
	{ 0,            XF86XK_MonBrightnessUp,    spawn,          SHCMD("xbacklight -inc 10") },
	{ 0,            XF86XK_MonBrightnessDown,  spawn,          SHCMD("xbacklight -dec 10") },
	{ 0,            XF86XK_TouchpadToggle,     spawn,          {.v= touchpadctl } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};


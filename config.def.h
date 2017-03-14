/* See LICENSE file for copyright and license details. */

/* appearance */
static const char *fonts[] = {
    "San Francisco Display:style=Bold:size=12",
    "FontAwesome:size=9",
};

static const char dmenufont[]       = "San Francisco Text:style=Bold:size=12";
static const char normbordercolor[] = "#333333";
static const char normbgcolor[]     = "#4D4D4D";
static const char normfgcolor[]     = "#E3DAC4";
static const char selbordercolor[]  = "#717693";
static const char selbgcolor[]      = "#24B390";
static const char selfgcolor[]      = "#FFFFFF";


//Elegant Dark
//static const char normbordercolor[] = "#333333";
//static const char normbgcolor[]     = "#171717";
//static const char normfgcolor[]     = "#858585";
//static const char selbordercolor[]  = "#FDBE01";
//static const char selbgcolor[]      = "#171717";
//static const char selfgcolor[]      = "#D7D7CF";

static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, 0: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */

/* for tag arrow patch */
static void x_nexttag(const Arg *arg);
static void x_prevtag(const Arg *arg);
static void x_adjtag(int n);

/* False means using the scroll wheel on a window will not change focus */
static const Bool focusonwheelscroll = False;

/* tagging */
#define MAX_TAGNAME_LEN 14		/* excludes TAG_PREPEND */
#define TAG_PREPEND "%1i:"		/* formatted as 2 chars */
#define MAX_TAGLEN 16			/* altogether */
static char tags[][MAX_TAGLEN] = { "1", "2: music", "3", "4", "5", "6", "7: vid", "8", "9" };


static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */

	/* class      instance    title       tags mask     iscentered	isfloating   isterminal noswallow monitor */
	{ "Gimp",     NULL,       NULL,       0,            0,           1,			0,         0,        -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           0,			0,         0,        -1 },
	{ "Chromium-browser",     NULL,       NULL,       1 << 8,       0,           0,			0,         0,        -1 },
	{ "Transmission-gtk",  NULL,       NULL,       1 << 7,       1,           0,			0,         0,        -1 },
	{ "feh",     NULL,       NULL,       0,            1,           1,			0,         0,        -1 },
	{ "mpv",    NULL,       NULL,       1 << 6,            0,           0,			0,         0,        -1 },
	{ "Gnome-terminal",     NULL,       NULL,       0,            0,           0,			1,         1,        -1 },

};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "\uf009",      tile },    /* first entry is default */
	{ "\uf0c2",      NULL },    /* no layout function means floating behavior */
	{ "\uf185",      monocle },
	{ "\uf037",      centeredmaster },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "gnome-terminal", NULL };
static const char *music[]  = { "gnome-terminal", "-e", "cmus", NULL };
static const char *browser[]  = { "firefox", NULL };
static const char *altbrowser[]  = { "chromium-browser", NULL };
static const char *fm[]  = 	{ "thunar", NULL };
static const char *passmenu[]  = 	{ "passmenu", NULL };
static const char *previewclip[]  = 	{ "peek_clip.sh", NULL };
static const char *incbl[]  = 	{ "xbacklight", "-inc", "10", NULL };
static const char *decbl[]  = 	{ "xbacklight", "-dec", "10", NULL };
static const char *incvol[]  = 	{ "vol.sh",  "inc", NULL };
static const char *decvol[]  = 	{ "vol.sh",  "dec", NULL };
static const char *mutevol[]  = 	{ "vol.sh",  "mute", NULL };
static const char *screenshot[]  = { "screenshot.sh", NULL };
static const char *screenshotquiet[]  = { "screenshot.sh", "--silent", NULL };
static const char *lock[]  = { "xautolock", "-locknow",  NULL };


static void x_prevtag(const Arg *arg) {
    (void)arg;
    x_adjtag(-1);    
}

static void x_nexttag(const Arg *arg) {
    (void)arg;
    x_adjtag(+1);    
}

static void x_adjtag(int n) {
    {
        int i, curtags;
        int seltag = 0;
        Arg arg;

        /*
         *     * Check first tag currently selected.  If there are
         *         * several tags selected we only pick first one.
         *             */
        if (selmon != NULL) {
            curtags = (selmon->tagset[selmon->seltags] & TAGMASK);
        } else {
            return;
        }
        for (i = 0; i < LENGTH(tags); i++) {
            if ((curtags & (1 << i)) != 0) {
                seltag = i;
                break;
            }
        }

        /*
         *      * Calculate next selected tag wrapping around
         *           * when tag overflows.
         *                */
        seltag = (seltag + n) % (int)LENGTH(tags);
        if (seltag < 0)
            seltag += LENGTH(tags);

        arg.ui = (1 << seltag);
        view(&arg);
    }
}

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ False,                       0x1008ff12,      spawn,          {.v = mutevol } },
	{ MODKEY|ShiftMask,             XK_l,      spawn,          {.v = lock } },
	{ False,                        XK_Print,      spawn,          {.v = screenshot } },
	{ MODKEY,                        XK_Print,      spawn,          {.v = screenshotquiet } },
	{ False,                        XK_F2,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_F1,      spawn,          {.v = altbrowser } },
	{ MODKEY|ShiftMask,             XK_F1,      spawn,          {.v = browser } },
	{ MODKEY,                       XK_p,      spawn,          {.v = previewclip } },
	{ MODKEY,                       XK_F3,      spawn,          {.v = fm } },
	{ MODKEY,                       XK_F6,      spawn,          {.v = passmenu } },
	{ MODKEY|ShiftMask,             XK_m,      spawn,          {.v = music } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,             			XK_F5, spawn,          {.v = termcmd } },
	{ False,                       0x1008ff13,      spawn,          {.v = incvol } },
	{ False,                        0x1008ff11,      spawn,          {.v = decvol } },
	{ False,                       0x1008ff03,      spawn,          {.v = decbl } },
	{ False,                       0x1008ff02,      spawn,          {.v = incbl } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	// { MODKEY|ShiftMask,             XK_b,      toggleextrabar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,             		XK_F4,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_c,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ ShiftMask,                    XK_F12,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_n,      nametag,        {0} },
    { MODKEY,                       XK_bracketleft,   x_prevtag,       {0} },
    { MODKEY,                       XK_bracketright,  x_nexttag,       {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_Escape,      quit,           {0} },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
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


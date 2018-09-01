// IMPLEMENTATION MODULE win
#define M2_IMPORT_win

#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <time.h>
#include <errno.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/keysym.h>

enum win_EventCode {
	win_ButtonPress,   /* user clicked a mouse button */
	win_ButtonRelease, /* user released a mouse button */
	win_ClientMessage, /* NOT USED */
	win_CloseRequest,  /* user clicked the close box */
	win_ConfigureNotify, /* NOT USED */
	win_DestroyNotify, /* the window has been closed */
	win_Expose,        /* window need update */
	win_KeyPress,      /* user pressed a keyboard key */
	win_MappingNotify, /* NOT USED */
	win_Null           /* unhandled event - ignore */
};

int win_button, win_pointer_x, win_pointer_y;
STRING * win_key;

char        *geometry = "60x60";
Display     *display;
Window      Root;
int         screen;
XSizeHints  mysizehints;
XWMHints    mywmhints;
Window      win;
GC          gc_tacche;
GC          gc_in;
GC          gc_out;
GC          gc_led_on;
GC          gc_led_off;
GC          gc_black;
GC          gc_white;
GC			gc_brown;
GC			gc_blue;

int win_x0, win_y0;
Atom ATOM_WM_PROTOCOLS, ATOM_WM_DELETE_WINDOW;


int hsize, vsize;  // dim. corrente della finestra

static Pixel GetColor(char *name) {

	XColor				color;
	XWindowAttributes	attributes;

	XGetWindowAttributes(display, Root, &attributes);

	color.pixel = 0;
	if (!XParseColor(display, attributes.colormap, name, &color)) {
		fprintf(stderr, "netmon: can't parse %s.\n", name);
	} else if (!XAllocColor(display, attributes.colormap, &color)) {
		fprintf(stderr, "netmon: can't allocate %s.\n", name);
	}
	return color.pixel;
}




GC grafico_gc;


void set_grafico_gc(GC gc)
{
	grafico_gc = gc;
}


void grafico_size(int *hsize, int *vsize)
{
	int x, y;
	unsigned int new_hsize, new_vsize;
	unsigned border;
	unsigned depth;

	if( XGetGeometry(display, win, &Root, &x, &y, &new_hsize, &new_vsize,
		&border, &depth) ){
		*hsize = new_hsize;
		*vsize = new_vsize;
	} else {  /* ? */
		*hsize = 30;
		*vsize = 30;
	}
}


void HandleConfigureNotifyEvent(XEvent e)
{
	unsigned int new_hsize, new_vsize;

	new_hsize = e.xconfigure.width;
	new_vsize = e.xconfigure.height;
	if( new_hsize == hsize && new_vsize == vsize ){
		// non e' un ridimensionamento, ma qualcos'altro - ignora
		return;
	}
	//XResizeWindow(display, win, new_hsize, new_vsize);
	hsize = new_hsize;
	vsize = new_vsize;
}


int win_Open(int w, int h)
{
	unsigned int borderwidth = 1;
	XClassHint classHint;
	char *display_name = NULL;
	char *wname = "X Window";
	XTextProperty name;
	XGCValues gcv;
	unsigned long gcm;
	Pixel back_pix, fore_pix;
	int dummy=0;

	hsize = w;
	vsize = h;
	//for (i=1; argv[i]; i++) {
	//	if ( strcmp(argv[i], "-display") == 0 ) 
	//		display_name = argv[++i];
	//	else if( strcmp(argv[i], "-geometry") == 0 )
	//		geometry = argv[++i];
	//}
	asprintf(&geometry, "%dx%d", w, h);

	if (!(display = XOpenDisplay(display_name))) {
		fprintf(stderr, "%s: can't open display %s\n", 
						wname, XDisplayName(display_name));
		exit(1);
	}
	screen  = DefaultScreen(display);
	Root    = RootWindow(display, screen);

	/* Create a window to hold the stuff */
	mysizehints.flags = USSize | USPosition;
	mysizehints.x = 0;
	mysizehints.y = 0;

	back_pix = GetColor("white");
	fore_pix = GetColor("black");

	XWMGeometry(display, screen, geometry, NULL, borderwidth, &mysizehints,
				&mysizehints.x, &mysizehints.y,&mysizehints.width,&mysizehints.height, &dummy);

	win = XCreateSimpleWindow(display, Root, mysizehints.x, mysizehints.y,
				mysizehints.width, mysizehints.height, borderwidth, fore_pix, back_pix);
	
	XSetWMNormalHints(display, win, &mysizehints);
	classHint.res_name = wname;
	classHint.res_class = wname;
	XSetClassHint(display, win, &classHint);

	XSelectInput(display, win, ButtonPressMask
		| ExposureMask
		| ButtonReleaseMask
		/* | PointerMotionMask */
		| KeyPressMask
		| StructureNotifyMask
	);

	if (XStringListToTextProperty(&wname, 1, &name) == 0) {
		fprintf(stderr, "%s: can't allocate window name\n", wname);
		exit(1);
	}

	XSetWMName(display, win, &name);

	/* Create GC for drawing */
	
	gcm = GCForeground | GCBackground | GCGraphicsExposures;
	gcv.background = back_pix;
	gcv.graphics_exposures = 0;

	fore_pix = GetColor("gray40");
	gcv.foreground = fore_pix;
	gc_tacche = XCreateGC(display, Root, gcm, &gcv);

	fore_pix = GetColor("cyan");
	gcv.foreground = fore_pix;
	gc_in = XCreateGC(display, Root, gcm, &gcv);

	fore_pix = GetColor("red");
	gcv.foreground = fore_pix;
	gc_out = XCreateGC(display, Root, gcm, &gcv);

	fore_pix = GetColor("#ff8888");
	gcv.foreground = fore_pix;
	gc_led_on = XCreateGC(display, Root, gcm, &gcv);

	fore_pix = GetColor("#774444");
	gcv.foreground = fore_pix;
	gc_led_off = XCreateGC(display, Root, gcm, &gcv);

	fore_pix = GetColor("white");
	gcv.foreground = fore_pix;
	gc_white = XCreateGC(display, Root, gcm, &gcv);

	fore_pix = GetColor("black");
	gcv.foreground = fore_pix;
	gc_black = XCreateGC(display, Root, gcm, &gcv);

	fore_pix = GetColor("brown");
	gcv.foreground = fore_pix;
	gc_brown = XCreateGC(display, Root, gcm, &gcv);

	fore_pix = GetColor("blue");
	gcv.foreground = fore_pix;
	gc_blue = XCreateGC(display, Root, gcm, &gcv);

	mywmhints.initial_state = WithdrawnState;
	mywmhints.window_group = win;
	mywmhints.flags = StateHint | WindowGroupHint;
	XSetWMHints(display, win, &mywmhints);

	// come faccio a fare questo in M2?
	//XSetCommand(display, win, argv, argc);
	XSetCommand(display, win, NULL, 0);

	XMapWindow(display, win);
	win_x0 = 0;
	win_y0 = 0;

	/* Cfg gestione Close Window dal wm: */
	/* cfr Barkakati 490 */
	ATOM_WM_PROTOCOLS = XInternAtom(display, "WM_PROTOCOLS", False);
	ATOM_WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XChangeProperty(display, win, ATOM_WM_PROTOCOLS, XA_ATOM, 32,
		PropModeReplace, (unsigned char *) &ATOM_WM_DELETE_WINDOW, 1);

	return (int) win;
}

void win_Close()
{
	XCloseDisplay(display);
}

void win_SetWin(int win)
{
}

void win_Erase()
{
	XClearWindow(display, win);
}

void win_Move(int x, int y)
{
	win_x0 = x;
	win_y0 = y;
}

void win_DrawLine(int x, int y)
{
	XDrawLine(display, win, gc_black, win_x0, win_y0, x, y);
	win_x0 = x;
	win_y0 = y;
}


void win_DrawString(STRING *s)
{
	if( s == NULL )  return;
	XDrawString(display, win, gc_black, win_x0, win_y0, s->s, s->len);
}


void win_GetSize(int *w, int *h)
{
	*w = hsize;
	*h = vsize;
}


int win_GetEvent()
{
	XEvent theEvent;

	XNextEvent(display, &theEvent);
	switch ( theEvent.type ) {

	case Expose:
		return win_Expose;

	case DestroyNotify:
		XCloseDisplay(display);
		return win_DestroyNotify;

	case ButtonPress:
		win_button = theEvent.xbutton.button;
		win_pointer_x = theEvent.xbutton.x;
		win_pointer_y = theEvent.xbutton.y;
		return win_ButtonPress;

	case ButtonRelease:
		win_button = theEvent.xbutton.button;
		win_pointer_x = theEvent.xbutton.x;
		win_pointer_y = theEvent.xbutton.y;
		return win_ButtonRelease;

	case KeyPress:
		{
			int count;
			unsigned char xlat[20];
			KeySym key;
			count = XLookupString(&theEvent.xkey, xlat, 20, &key, NULL);
			//printf("Tasto premuto: [%c], count=%d, key=%x\n", *xlat, count, key);
			switch( key ){
			case XK_BackSpace: win_key = new_STRING("BS", 2); break;
			case XK_Tab:       win_key = new_STRING("TAB", 3); break;
			case XK_Linefeed:  win_key = new_STRING("LF", 2); break;
			case XK_Clear:     win_key = new_STRING("CLEAR", 5); break;
			case XK_Return:    win_key = new_STRING("RETURN", 6); break;
			case XK_Escape:    win_key = new_STRING("ESC", 3); break;
			case XK_Delete:    win_key = new_STRING("DEL", 3); break;
			case XK_Home:      win_key = new_STRING("HOME", 4); break;
			case XK_Left:      win_key = new_STRING("LEFT", 4); break;
			case XK_Up:        win_key = new_STRING("UP", 2); break;
			case XK_Right:     win_key = new_STRING("RIGHT", 5); break;
			case XK_Down:      win_key = new_STRING("DOWN", 4); break;
			case XK_Prior:     win_key = new_STRING("PRIOR", 5); break;
			case XK_Next:      win_key = new_STRING("NEXT", 4); break;
			case XK_End:       win_key = new_STRING("END", 3); break;
			case XK_Begin:     win_key = new_STRING("BEGIN", 5); break;
			case XK_Print:     win_key = new_STRING("PRINT", 5); break;
			case XK_Insert:    win_key = new_STRING("INSERT", 6); break;
			case XK_Cancel:    win_key = new_STRING("CANCEL", 6); break;
			case XK_Break:     win_key = new_STRING("BREAK", 5); break;
			case XK_KP_Enter:  win_key = new_STRING("ENTER", 5); break;
			case XK_F1:        win_key = new_STRING("F1", 2); break;
			case XK_F2:        win_key = new_STRING("F2", 2); break;
			case XK_F3:        win_key = new_STRING("F3", 2); break;
			case XK_F4:        win_key = new_STRING("F4", 2); break;
			case XK_F5:        win_key = new_STRING("F5", 2); break;
			case XK_F6:        win_key = new_STRING("F6", 2); break;
			case XK_F7:        win_key = new_STRING("F7", 2); break;
			case XK_F8:        win_key = new_STRING("F8", 2); break;
			case XK_F9:        win_key = new_STRING("F9", 2); break;
			case XK_F10:       win_key = new_STRING("F10", 3); break;
			case XK_F11:       win_key = new_STRING("F11", 3); break;
			case XK_F12:       win_key = new_STRING("F12", 3); break;
			default:
				if( count == 1 ){
					win_key = new_STRING(xlat, 1);
				} else {
					win_key = NULL;
				}
			}
		}
		return win_KeyPress;

	case MappingNotify:
		/* cfr Barkakati 190 */
		XRefreshKeyboardMapping(&theEvent.xmapping);
		return win_MappingNotify;

	case ConfigureNotify:
		/* cfr Barkakati 218 */
		HandleConfigureNotifyEvent(theEvent);
		return win_ConfigureNotify;

	case ClientMessage:
		if( theEvent.xclient.message_type == ATOM_WM_PROTOCOLS ){
			if( theEvent.xclient.data.l[0] == ATOM_WM_DELETE_WINDOW ){
				return win_CloseRequest;
			}
		}
		return win_Null;

	default: return win_Null;
	}
	return 0;
}


int win_PendingEvent()
{
	return ( XPending(display) > 0 );
}

// END

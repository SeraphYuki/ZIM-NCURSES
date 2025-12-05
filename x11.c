#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "utils.h"
#include "x11.h"

static Display *display;
static Window window, ourWindow, root, *children, parent;
static GC gc;
static int x11Init = 0;

void X11_Init(){
    display = XOpenDisplay(0);
    char *id;
    int revert;
    unsigned int nchildren;

    if((id = getenv("WINDOWID")) != NULL){
        window = (Window)atoi(id);
    } else {
        XGetInputFocus(display, &window, &revert);
    }

    if(!window) return;

    XWindowAttributes attr;
    XGetWindowAttributes(display, window, &attr);

    int width = 0, height = 0;

    while(1){
        Window p_window;
        XQueryTree(display, window, &root, &parent, &children, &nchildren);
        p_window = window;
        int i;
        for(i = 0; i < nchildren; i++){
            XGetWindowAttributes(display, children[i], &attr);
            if(attr.width > width && attr.height > height){
                width = attr.width;
                height = attr.height;
                window = children[i];
            }
        }

        if(p_window == window) break;
    }

    if(width == 1 && height == 1)
        window = parent;

    unsigned long windowMask;
    XSetWindowAttributes winAttrib; 
            
    windowMask = CWBackPixel | CWBorderPixel;   
    winAttrib.border_pixel = BlackPixel (display, 0);
    winAttrib.background_pixel = BlackPixel (display, 0);
    winAttrib.override_redirect = 0;

    ourWindow = XCreateWindow(display, window, attr.x, attr.y, attr.width, attr.height, attr.border_width, attr.depth, attr.class, 
        attr.visual, windowMask, &winAttrib );

    gc = XCreateGC(display, ourWindow, 0, NULL);

	XSelectInput(display, window, KeyPress | KeyRelease);
    x11Init = 1;
}
void X11_NextEvent(XEvent *ev){
	XWindowEvent(display,window, KeyPress | KeyRelease,  ev);
}
void X11_Close(){
    if(!x11Init) return;
    XFreeGC(display,gc);
    XCloseDisplay(display);
    x11Init = 0;
}

int X11_GetGlobalKeys(int *ret){
    char keys[32];
    XQueryKeymap(display, keys);
    int k, returnOne = 0;
    for(k = 0; k < 32; k++){ 
        if(keys[k]) {
            int f;
            for(f = 0; f < 8; f++){
                if(keys[k] & 0x01 << f){
                    XKeyEvent keyEvent;
                    keyEvent.keycode = (k*8)+f;
                    keyEvent.display = display;
                    char *str = XKeysymToString(XLookupKeysym(&keyEvent, 0));
                    if(strlen(str) == 1) ret[(k*8)+f] = (int)str[0];
                    else if(strcmp(str, "equal") == 0) ret[(k*8)+f] = '=';
                    else if(strcmp(str, "minus") == 0) ret[(k*8)+f] = '-';
                    else if(strcmp(str, "Control_L") == 0) ret[(k*8)+f] = X11_CONTROL_KEY;
                    else if(strcmp(str, "Control_R") == 0) ret[(k*8)+f] = X11_CONTROL_KEY;
                    else if(strcmp(str, "Alt_L") == 0) ret[(k*8)+f] = X11_ALT_KEY;
                    else if(strcmp(str, "Alt_R") == 0) ret[(k*8)+f] = X11_ALT_KEY;
                    else if(strcmp(str, "Super_L") == 0) ret[(k*8)+f] = X11_SUPER_KEY;
                    else if(strcmp(str, "Super_R") == 0) ret[(k*8)+f] = X11_SUPER_KEY;
                    returnOne = 1;
                }
            }
        }
    }
    return returnOne;
}

void X11_WithdrawWindow(){
    if(!x11Init) return;
    XWithdrawWindow(display, ourWindow, DefaultScreen(display));
    XSync(display,False);
}
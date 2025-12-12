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
static Window window, root, *children, parent;
static XIC ic;
static XIM xim;
static Atom targets_atom;
static Atom text_atom;
static Atom UTF8;
static int x11Init = 0;

XIC X11_GetIC(){ return ic;}

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

    xim = XOpenIM(display,NULL,NULL,NULL);
    ic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, 
        XNClientWindow, window, NULL);

	XSelectInput(display, parent,  FocusChangeMask);
	XSelectInput(display, window, KeyPress | KeyRelease | FocusChangeMask);
	XSetInputFocus(display, window, RevertToPointerRoot, CurrentTime);

	targets_atom = XInternAtom(display, "Targets", 0);
	text_atom = XInternAtom(display, "TEXT", 0);
	UTF8 = XInternAtom(display, "UTF8_STRING",1);
	if(UTF8 == None) UTF8 = 31;


    x11Init = 1;
}

void X11_ResetFocus(){
}
void X11_NextEvent(XEvent *ev){
	XNextEvent(display,ev);
	if(ev->type == FocusIn && ev->xfocus.window == parent){
		XSetInputFocus(display, window, RevertToPointerRoot, CurrentTime);
		XSetICFocus(ic);
		XSelectInput(display, window, KeyPress | KeyRelease | FocusChangeMask);
	}
}

void X11_Close(){
    if(!x11Init) return;
	XSetInputFocus(display, parent, RevertToParent, CurrentTime);
    XDestroyIC(ic);
    XCloseIM(xim);
    XCloseDisplay(display);
    x11Init = 0;
}

void X11_WithdrawWindow(){
    if(!x11Init) return;
    XSync(display,False);
}
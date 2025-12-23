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
static Window window, root, *children, parent, clipboardWindow;
static XIC ic;
static XIM xim;
static Atom targets_atom, selection;
static Atom text_atom;
static Atom UTF8, XSEL_DATA;
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

    unsigned long windowMask;
    XSetWindowAttributes winAttrib; 
            
    windowMask = CWBackPixel | CWBorderPixel ;
    winAttrib.border_pixel = BlackPixel (display, 0);
    winAttrib.background_pixel = BlackPixel (display, 0);
    winAttrib.override_redirect = 0;

    clipboardWindow = XCreateWindow(display, window, attr.x, attr.y, attr.width, attr.height, 
	attr.border_width, attr.depth, attr.class, 
        attr.visual, windowMask, &winAttrib );

//    gc = XCreateGC(display, ourWindow, 0, NULL);

    xim = XOpenIM(display,NULL,NULL,NULL);
    ic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, 
        XNClientWindow, window, NULL);

	XSelectInput(display, parent,  FocusChangeMask);
    XSelectInput(display, window, KeyPress | KeyRelease);
	XSetInputFocus(display, window, RevertToPointerRoot, CurrentTime);


	targets_atom = XInternAtom(display, "TARGETS", 0);
	text_atom = XInternAtom(display, "TEXT", 0);
    UTF8 = XInternAtom(display, "UTF8_STRING",0);
    XSEL_DATA = XInternAtom(display, "XSEL_DATA",0);
    selection = XInternAtom(display, "CLIPBOARD",0);
	if(UTF8 == None) UTF8 = 31;


    x11Init = 1;
}

void X11_Copy(char **clipboard){
	XSetSelectionOwner(display,selection,window,0);

}

void X11_Paste(char **clipboard){

	XConvertSelection(display,selection,UTF8,XSEL_DATA,clipboardWindow,CurrentTime),
	XSync(display,0);
	XEvent ev;

	XNextEvent(display,&ev);

	if(ev.type == SelectionNotify){
		if(ev.xselection.selection == selection && ev.xselection.property){
			Atom target;
			int format;
			unsigned long N, size;
			char *data;
	
			XGetWindowProperty(ev.xselection.display, ev.xselection.requestor,
				ev.xselection.property,0L,(~0L),0,AnyPropertyType,
					&target,&format,&size,&N,(unsigned char **)&data);
	
	
			if(target == UTF8 || target == 31){
				if(*clipboard) free(*clipboard);
				*clipboard = malloc(size+1);
				memcpy(*clipboard,data,size);
				(*clipboard)[size]=0;
				XFree(data);
			}
			XDeleteProperty(ev.xselection.display,ev.xselection.requestor,ev.xselection.property);
		}
	}
	XSetInputFocus(display, window, RevertToPointerRoot, CurrentTime);
	XSetICFocus(ic);
	XSelectInput(display, window, KeyPress | KeyRelease | FocusChangeMask);
}

void X11_NextEvent(XEvent *ev,char *clipboard){
	XNextEvent(display,ev);

	if(ev->type == FocusIn && ev->xfocus.window == parent){
		XSetInputFocus(display, window, RevertToPointerRoot, CurrentTime);
		XSetICFocus(ic);
		XSelectInput(display, window, KeyPress | KeyRelease | FocusChangeMask);
    } else if(ev->type == SelectionRequest){
		
		if(ev->xselectionrequest.selection == selection){
			XSelectionRequestEvent *xsr = &ev->xselectionrequest;
			XSelectionEvent event = {0}; 
			event.type = SelectionNotify, event.display = xsr->display;
			event.requestor = xsr->requestor, event.selection = xsr->selection, event.time = xsr->time;
			event.target = xsr->target, event.property = xsr->property;
			if(event.target == targets_atom){
				XChangeProperty(event.display,event.requestor,event.property,
					4,32, PropModeReplace,(unsigned char*)&UTF8,1);
			} else if (event.target == 31 || event.target == text_atom){
				XChangeProperty(event.display,event.requestor,event.property,
					31,8,PropModeReplace,(unsigned char*)clipboard, strlen(clipboard));
			} else if(event.target == UTF8){
				XChangeProperty(event.display,event.requestor,event.property,
					UTF8,8,PropModeReplace,(unsigned char*)clipboard,strlen(clipboard));
			}
			XSendEvent(display,event.requestor,0,0,(XEvent*)&event);
		}
	}
}

void X11_Close(){
    if(!x11Init) return;
	XSetInputFocus(display, parent, RevertToParent, CurrentTime);
    XDestroyIC(ic);
    XCloseIM(xim);
	XDestroyWindow(display,clipboardWindow);
    XCloseDisplay(display);
    x11Init = 0;
}

void X11_WithdrawWindow(){
    if(!x11Init) return;
    XSync(display,False);
}
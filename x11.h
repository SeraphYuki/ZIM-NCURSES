#ifndef X11_DEF
#define X11_DEF
#include <ncurses.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>

#define X11_CONTROL_KEY KEY_MAX+1
#define X11_ALT_KEY KEY_MAX+2
#define X11_SUPER_KEY KEY_MAX+3
#define X11_NUM_OF_RET_KEYS 32*8

void X11_WithdrawWindow();
XIC X11_GetIC();
Display *X11_GetDisplay();
void X11_Init();
void X11_NextEvent(XEvent *ev, char *clipboard);
void X11_Close();
int X11_GetGlobalKeys(int *ret);
void X11_Copy(char **str);
void X11_Paste(char **str);


#endif
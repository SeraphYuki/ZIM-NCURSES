#ifndef X11_DEF
#define X11_DEF
#include <ncurses.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>

#define X11_CONTROL_KEY KEY_MAX+1
#define X11_ALT_KEY KEY_MAX+2
#define X11_SUPER_KEY KEY_MAX+3
#define X11_NUM_OF_RET_KEYS 32*8

typedef struct {
	XImage *xi;
	char *pixels;
	int width;
	int height;
	int channels;
} Image;

void X11_DestroyImage(Image *img);
int X11_LoadJPEG(FILE *fp, Image *image);
int X11_LoadPNG(FILE *fp, Image *image);
void X11_DrawImage(Image *image,int xPos, int yPos, int drawWidth, int drawHeight);
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
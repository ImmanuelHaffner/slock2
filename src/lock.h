#ifndef LOCK_H
#define LOCK_H


#include <X11/Xlib.h>

#ifndef FONTNAME
#define FONTNAME "-*-helvetica-*-r-*-*-14-*-*-*-*-*-*-*"
#endif


typedef struct {
  bool ok;
  int screen;
  Window root, win;
  Pixmap pmap;
  XColor colorActive;
  XColor colorInactive;
  XColor colorError;
  GC gc;
  XFontStruct *font;
} Lock;


void init( Display * const display, Lock * const lock );

/**
 * Locks the `screen`th screen of the display.
 */
void lock( Display * const display, Lock * const lock );

/**
 * Unlocks the `screen`th screen of the display.
 */
void unlock( Display * const display, Lock * const lock );


#endif

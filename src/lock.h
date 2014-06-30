#ifndef LOCK_H
#define LOCK_H


#include <X11/Xlib.h>


typedef struct {
  bool ok;
  int screen;
  Window root, win;
  Pixmap pmap;
  XColor colorActive;
  XColor colorInactive;
  XColor colorError;
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

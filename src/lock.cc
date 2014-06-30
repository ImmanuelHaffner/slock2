#include "lock.h"


#include <cassert>

#include <X11/keysym.h>
#include <X11/Xutil.h>

#include "logger.h"


void init( Display * const display, Lock * const lock )
{
  assert( display && "display must not be NULL" );
  assert( lock && "lock must not be NULL" );

  /* Obtain the X Root Window for the screen. */
  lock->root = RootWindow( display, lock->screen );

  /* Obtain default color map for the screen. */
  Colormap cmap = DefaultColormap( display, lock->screen );

  /* Obtain the XColors. */
  XColor dummy;
  XAllocNamedColor( display,
      cmap,
      COLOR_INACTIVE,
      &lock->colorInactive,
      &dummy );

  XAllocNamedColor( display,
      cmap,
      COLOR_ACTIVE,
      &lock->colorActive,
      &dummy );

  XAllocNamedColor( display,
      cmap,
      COLOR_ERROR,
      &lock->colorError,
      &dummy );

  Logger::get()->d( "initialized lock for screen ", lock->screen );
}

void lock( Display * const display, Lock * const lock )
{
  assert( display && "display must not be NULL" );
  assert( lock && "lock must not be NULL" );

  lock->ok = false;

  /* Create Window Attributes for a new window. */
  XSetWindowAttributes wa;
  wa.override_redirect = 1;
  wa.background_pixel = lock->colorInactive.pixel;

  /* Create a new window.
   *
   * see http://menehune.opt.wfu.edu/Kokua/Irix_6.5.21_doc_cd/usr/share/Insight/library/SGI_bookshelves/SGI_Developer/books/XLib_PG/sgi_html/ch04.html
   */
  lock->win = XCreateWindow(
      display,      /* the display where to create this window */
      lock->root,   /* the parent of this window */
      0,            /* X-pos */
      0,            /* Y-pos */
      DisplayWidth( display, lock->screen ),  /* width */
      DisplayHeight( display, lock->screen ), /* height */
      0,                                      /* border width */
      DefaultDepth( display, lock->screen ),  /* color depth */
      CopyFromParent,                         /* window class */
      DefaultVisual( display, lock->screen ), /* visual type */
      CWOverrideRedirect | CWBackPixel,       /* value mask */
      &wa                                     /* window attributes */
      );

  /* Create a pixmap for the cursor. */
  char data[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  lock->pmap = XCreateBitmapFromData(
      display,
      lock->win,  /* Drawable */
      data,       /* data */
      8,          /* width */
      8           /* height */
      );

  /* Create an invisible cursor. */
  Cursor cursor = XCreatePixmapCursor(
      display,
      lock->pmap,           /* shape of the source cursor */
      lock->pmap,           /* mask */
      &lock->colorInactive, /* foreground color */
      &lock->colorInactive, /* background color */
      0,                    /* X-Pos */
      0                     /* Y-Pos */
      );

  /* Use the invisible cursor. */
  XDefineCursor( display, lock->win, cursor );

  /* Map the window to the display, and raise it to the top. */
  XMapRaised( display, lock->win );

  int grab = XGrabPointer(
      display,
      lock->root,     /* grab window */
      False,          /* owner events */
      ButtonPressMask | ButtonReleaseMask | PointerMotionMask,/* event mask */
      GrabModeAsync,  /* pointer mode */
      GrabModeAsync,  /* keyboard mode */
      None,           /* window to confine the pointer in */
      cursor,         /* the cursor to dislay */
      CurrentTime );

  if ( GrabSuccess == grab )
    Logger::get()->d( "successfully grabbed the pointer for screen ",
        lock->screen );
  else
  {
    Logger::get()->d( "failed to grab the pointer for screen ", lock->screen );
    unlock( display, lock );
    return;
  }

  grab = XGrabKeyboard(
      display,
      lock->root,     /* grab window */
      True,           /* owner events */
      GrabModeAsync,  /* pointer mode */
      GrabModeAsync,  /* keyboard mode */
      CurrentTime );

  if ( GrabSuccess == grab )
    Logger::get()->d( "successfully grabbed the keyboard for screen ",
        lock->screen );
  else
  {
    Logger::get()->d( "failed to grab the keyboard for screen ", lock->screen );
    unlock( display, lock );
    return;
  }

  XSelectInput( display, lock->root, SubstructureNotifyMask );

  lock->ok = true;
}

void unlock( Display * const display, Lock * const lock )
{
  assert( display && "display must not be NULL" );
  assert( lock && "lock must not be NULL" );

  /* Return the pointer to all windows. */
  XUngrabPointer( display, CurrentTime );

  unsigned long pixels[] = {
    lock->colorInactive.pixel,
    lock->colorActive.pixel,
    lock->colorError.pixel };

  XFreeColors(
      display,
      DefaultColormap( display, lock->screen ),
      pixels,
      sizeof( pixels ) / sizeof( pixels[0] ),
      0 );

  /* Cause the X server to free the pixmap storage, as soon as there are no more
   * references to it.
   */
  XFreePixmap( display, lock->pmap );
  /* Cause the X server to destroy the specified window. */
  XDestroyWindow( display, lock->win );

  delete lock;
}

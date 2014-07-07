#ifndef EVENT_H
#define EVENT_H


#ifndef EVENT_HANDLER
#define EVENT_HANDLER "/usr/bin/slock2-events.sh"
#endif


/**
 * Called when login succeeds.
 */
void loginSuccess();

/**
 * Called when login fails.
 */
void loginFail();

/**
 * Invokes the slock2 shell script, and passes the event as argument.
 */
void raiseEvent( char const * const name );


#endif

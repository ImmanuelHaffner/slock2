slock2 - Simple X Screen Locker
===============================
**slock2** is a simple screen locker utility for X.
It is based on **slock** from http://tools.suckless.org/slock/ .

Features
--------
* **slock2** reimplements **slock** in C++11
* more transparent and better documented code
* a simple but powerful logger
* some command-line arguments
* event-handling with a shell script
* TODO: udev


#### Logger

**slock2** contains a simple and lightweight logging utility.  The
logger features the different log levels `ERROR`, `WARNING`, `NORMAL`, `VERBOSE`
and `DEBUG`.  In `VERBOSE`-mode the logger mirrors all log output to `stdout`.
`DEBUG` subsumes `VERBOSE`, and adds further log output relevant for debugging.


#### Command-line arguments

**slock2** provides some command-line arguments to modify it's behaviour:


| Short | Long              | Usage                                                                |
|-------|-------------------|----------------------------------------------------------------------|
| -h    | --help            | Prints a help message.                                               |
| -b    | --bell            | Rings the XBell on failed login attempt.                             |
| -v    | --verbose         | Sets the log level to `VERBOSE`.                                     |
|       | --debug           | Sets the log level to `DEBUG`.                                       |
|       | --logfile <FILE>  | Sets the used log file (by default `/var/log/slock.log`).            |
|       | --version         | Prints the version, the release date, and the author.                |


Requirements
------------
In order to build **slock2** you need the `Xlib` header files.


Build
-----

To build **slock2** issue the command

    $ make

To configure the build process, you can create the file `default.cfg` in the
root directory of this project.  Some available settings are:

| Flag                  | Example                                                        |
|-----------------------|----------------------------------------------------------------|
| CXX                   | CXX = clang++                                                  |
| DEBUG                 | DEBUG = 1, DEBUG = 0 (default)                                 |
| VERBOSE               | VERBOSE = 1, VERBOSE = 0 (default)                             |
| COLOR_INACTIVE        | COLOR_INACTIVE = \"black\", COLOR_INACTIVE = \"#005577¸"       |
| COLOR_ACTIVE          | COLOR_ACTIVE   = \"black\", COLOR_ACTIVE   = \"#005577¸"       |
| COLOR_ERROR           | COLOR_ERROR    = \"black\", COLOR_ERROR    = \"#005577¸"       |


Installation
------------

Edit the `install.cfg` to match your local setup.

Afterwards enter the following command to build and install **slock2**

    $ make install


#### IMPORTANT: Root privileges

**slock2** needs root privileges to disable the Out-Of-Memory Killer in Linux,
and to access the password file.  Therefore, you need to make *root* the owner
of **slock2**, and set the *SetUID* bit.
(This is already done by `make install`.)


Running slock
-------------

To run **slock2**, enter the command

    $ slock2

and to get out of it, enter your password.

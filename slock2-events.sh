#!/bin/sh

case $1 in
  "start")
    ;;

  "loginSuccess")
    ;;

  "loginFail")
    ;;

  "exit")
    ;;

  *)
    echo "unknown event $1"
    ;;
esac

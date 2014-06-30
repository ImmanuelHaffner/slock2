#!/bin/sh

case $1 in
  "loginSuccess")
    ;;

  "loginFail")
    ;;

  *)
    echo "unknown event $1"
    ;;
esac

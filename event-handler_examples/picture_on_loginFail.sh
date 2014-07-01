#!/bin/bash

DIR=~/Images/slock2

# Create/Clear picture directory

case $1 in
  "start")
    if [ ! -d "${DIR}" ];
    then
      mkdir -p "${DIR}"
    fi

    for FILE in "${DIR}/"*.png; do
      if [ -f "${FILE}" ];
      then
        rm "${FILE}"
      fi
    done
    ;;

  "loginSuccess")
    ;;

  "loginFail")
    IMAGE="${DIR}/loginFail$(ls -l "${DIR}/"*.png 2>/dev/null | wc -l).png"
    fswebcam -r 1920x1080 "${IMAGE}" > /dev/null 2>&1
    echo "created image '${IMAGE}'"
    ;;

  "exit")
    COUNT=$(ls -l "${DIR}/"*.png 2>/dev/null | wc -l)

    if [ ! 0 -eq ${COUNT} ];
    then
      sxiv "${DIR}/"*.png
    fi
    ;;

  *)
    echo "unknown event '$1'"
    ;;
esac

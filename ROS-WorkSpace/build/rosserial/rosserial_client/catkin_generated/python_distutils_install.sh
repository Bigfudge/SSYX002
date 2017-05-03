#!/bin/sh

if [ -n "$DESTDIR" ] ; then
    case $DESTDIR in
        /*) # ok
            ;;
        *)
            /bin/echo "DESTDIR argument must be absolute... "
            /bin/echo "otherwise python's distutils will bork things."
            exit 1
    esac
    DESTDIR_ARG="--root=$DESTDIR"
fi

echo_and_run() { echo "+ $@" ; "$@" ; }

echo_and_run cd "/home/simonpersson/ROS-WorkSpace/src/rosserial/rosserial_client"

# snsure that Python install destination exists
echo_and_run mkdir -p "$DESTDIR/home/simonpersson/ROS-WorkSpace/install/lib/python2.7/dist-packages"

# Note that PYTHONPATH is pulled from the environment to support installing
# into one location when some dependencies were installed in another
# location, #123.
echo_and_run /usr/bin/env \
    PYTHONPATH="/home/simonpersson/ROS-WorkSpace/install/lib/python2.7/dist-packages:/home/simonpersson/ROS-WorkSpace/build/lib/python2.7/dist-packages:$PYTHONPATH" \
    CATKIN_BINARY_DIR="/home/simonpersson/ROS-WorkSpace/build" \
    "/usr/bin/python" \
    "/home/simonpersson/ROS-WorkSpace/src/rosserial/rosserial_client/setup.py" \
    build --build-base "/home/simonpersson/ROS-WorkSpace/build/rosserial/rosserial_client" \
    install \
    $DESTDIR_ARG \
    --install-layout=deb --prefix="/home/simonpersson/ROS-WorkSpace/install" --install-scripts="/home/simonpersson/ROS-WorkSpace/install/bin"

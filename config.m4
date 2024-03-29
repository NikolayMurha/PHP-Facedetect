PHP_ARG_WITH(facedetect, for facedetect support, [  --with-facedetect     Enable facedetect support])

if test "$PHP_FACEDETECT" != "no"; then
  SEARCH_PATH="/usr/local /usr /opt/local"
  SEARCH_FOR="/include/opencv/cv.h"

  if test -r $PHP_FACEDETECT/$SEARCH_FOR; then
    FACEDETECT_DIR=$PHP_FACEDETECT
  else
    AC_MSG_CHECKING([for facedetect in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        FACEDETECT_DIR=$i
        AC_MSG_RESULT(found in $i)
	break
      fi
    done
  fi

  if test -z "$FACEDETECT_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the OpenCV distribution])
  fi

  PHP_ADD_INCLUDE($FACEDETECT_DIR/include)

  AC_CHECK_HEADER([$FACEDETECT_DIR/include/opencv/cv.h], [], AC_MSG_ERROR('opencv/cv.h' header not found))
  AC_CHECK_HEADER([$FACEDETECT_DIR/include/opencv/highgui.h], [], AC_MSG_ERROR('opencv/highgui.h' header not found))

  PHP_CHECK_LIBRARY(opencv_objdetect, cvLoad,
  [
    PHP_ADD_LIBRARY_WITH_PATH(opencv_objdetect, $FACEDETECT_DIR/lib, FACEDETECT_SHARED_LIBADD)
    AC_DEFINE(HAVE_FACEDETECT, 1, [ ])
  ],[
    AC_MSG_ERROR([wrong OpenCV version or OpenCV not found])
  ],[
  ])

  PHP_REQUIRE_CXX()
  PHP_SUBST(FACEDETECT_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, FACEDETECT_SHARED_LIBADD)
  PHP_NEW_EXTENSION(facedetect, facedetect.cpp, $ext_shared)

  #PHP_NEW_EXTENSION(vehicles, vehicles.cc car.cc, $ext_shared)
  AC_DEFINE(HAVE_FACEDETECT, 1, [ ])
fi


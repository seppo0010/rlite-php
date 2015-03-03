PHP_ARG_ENABLE(rlite, whether to enable rlite support,
[ --enable-rlite   Enable rlite support])

PHP_ARG_WITH(rlite-dir, for rlite library,
[  --with-rlite-dir[=DIR]   Set the path to rlite install prefix.], yes)

if test "$PHP_RLITE" = "yes"; then


  AC_MSG_CHECKING([for rlite installation])

  #
  # Caller wants to check this path specifically
  #
  if test "x$PHP_RLITE_DIR" != "xno" && test "x$PHP_RLITE_DIR" != "xyes"; then
    if test -r "$PHP_RLITE_DIR/include/hirlite/hirlite.h"; then
      RLITE_DIR=$PHP_RLITE_DIR
      break
    fi
  else
    for i in /usr/local /usr /opt /opt/local; do
      if test -r "$i/include/hirlite/hirlite.h"; then
        RLITE_DIR=$i
        break
      fi
    done
  fi

  if test "x$RLITE_DIR" = "x"; then
    AC_MSG_ERROR([not found])
  fi

  AC_MSG_RESULT([found in $RLITE_DIR])

  PHP_ADD_LIBRARY_WITH_PATH(hirlite, [$RLITE_DIR/$PHP_LIBDIR], RLITE_SHARED_LIBADD)
  PHP_ADD_INCLUDE([$RLITE_DIR/include])

  AC_DEFINE(HAVE_RLITE, 1, [Whether you have rlite])
  PHP_SUBST(RLITE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(rlite, rlite.c, $ext_shared)
fi

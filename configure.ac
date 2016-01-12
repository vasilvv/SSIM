#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ssim], [1.0], [vasilvv@mit.edu])
AM_INIT_AUTOMAKE([foreign -Wall])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/RawFrame.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
PKG_CHECK_MODULES([LIBAVUTIL], [libavutil])
PKG_CHECK_MODULES([LIBAVFORMAT], [libavformat])
PKG_CHECK_MODULES([LIBAVCODEC], [libavcodec])
PKG_CHECK_MODULES([LIBSWSCALE], [libswscale])
PKG_CHECK_MODULES([X264], [x264])
PKG_CHECK_MODULES([ZLIB], [zlib])
AC_SUBST([LIBAVUTIL_CFLAGS])
AC_SUBST([LIBAVFORMAT_CFLAGS])
AC_SUBST([LIBAVCODEC_CFLAGS])
AC_SUBST([LIBSWSCALE_CFLAGS])
AC_SUBST([ZLIB_CFLAGS])
AC_SUBST([LIBAVUTIL_LIBS])
AC_SUBST([LIBAVFORMAT_LIBS])
AC_SUBST([LIBAVCODEC_LIBS])
AC_SUBST([LIBSWSCALE_LIBS])
AC_SUBST([ZLIB_LIBS])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

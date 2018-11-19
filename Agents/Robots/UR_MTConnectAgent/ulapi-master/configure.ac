AC_INIT([ulapi], [1.6])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([src/ulapi.h])
AC_CONFIG_HEADERS([config/config.h])
AM_INIT_AUTOMAKE([subdir-objects])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_RANLIB

# Checks for things ulapi needs
ACX_PRE_ULAPI

# Configures Doxygen.
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([ulapi],[Doxyfile],[doxygen-doc])

AC_CONFIG_FILES([Makefile src/Makefile lib/Makefile bin/Makefile])

AC_OUTPUT

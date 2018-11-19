SUBDIRS = src lib bin

EXTRA_DIST = CMakeLists.txt Makefile.am Makefile.local acinclude.m4 configure.ac autoconf.sh config/config.guess config/config.sub package.xml ulapi.m4 win32
nobase_data_DATA = ulapi.m4

# uncomment these lines if you want to include the Doxygen documentation
# in the distribution and in the install
# EXTRA_DIST += @DX_CONFIG@ @DX_DOCDIR@ @DX_DOCDIR@/html
# nobase_data_DATA += @DX_DOCDIR@/html/*

include doxygen.am

if HAVE_RTAI

EXTRA_DIST +=  rtlib/README rtlib/Makefile rtlib/SETUP

all-local:
	make -f Makefile.local modules

clean-local:
	make -f Makefile.local modules_clean

install-exec-hook:
	$(INSTALL_DATA) rtlib/rtapi_hal_mod.*o $(DESTDIR)$(libdir)

endif

MOSTLYCLEANFILES = @DX_CLEANFILES@


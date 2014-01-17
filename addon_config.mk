# All variables and this file are optional, if they are not present the PG and the
# makefiles will try to parse the correct values from the file system.
#
# Variables that specify exclusions can use % as a wildcard to specify that anything in
# that position will match. A partial path can also be specified to, for example, exclude
# a whole folder from the parsed paths from the file system
#
# Variables can be specified using = or +=
# = will clear the contents of that variable both specified from the file or the ones parsed
# from the file system
# += will add the values to the previous ones in the file or the ones parsed from the file 
# system
# 
# The PG can be used to detect errors in this file, just create a new project with this addon 
# and the PG will write to the console the kind of error and in which line it is

meta:
	ADDON_NAME = ofxNice
	ADDON_DESCRIPTION = Addon for NAT transversal using libnice
	ADDON_AUTHOR = Arturo Castro
	ADDON_TAGS = "ICE" "NAT" "network"
	ADDON_URL = http://github.com/arturoc/ofxNice

common:
	# dependencies with other addons, a list of them separated by spaces 
	# or use += in several lines
	# ADDON_DEPENDENCIES =
	
	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=
	# ADDON_INCLUDES =
	
	# any special flag that should be passed to the compiler when using this
	# addon
	# ADDON_CFLAGS =
	
	# any special flag that should be passed to the linker when using this
	# addon, also used for system libraries with -lname
	# ADDON_LDFLAGS =
	
	# linux only, any library that should be included in the project using
	# pkg-config
	ADDON_PKG_CONFIG_LIBRARIES = nice
	
	# osx/iOS only, any framework that should be included in the project
	# ADDON_FRAMEWORKS =
	
	# source files, these will be usually parsed from the file system looking
	# in the src folders in libs and the root of the addon. if your addon needs
	# to include files in different places or a different set of files per platform
	# they can be specified here
	# ADDON_SOURCES =
	
	# some addons need resources to be copied to the bin/data folder of the project
	# specify here any files that need to be copied, you can use wildcards like * and ?
	# ADDON_DATA = 
	
	# when parsing the file system looking for libraries exclude this for all or
	# a specific platform
	# ADDON_LIBS_EXCLUDE =
	
	# when parsing the file system looking for sources exclude this for all or
	# a specific platform
	# ADDON_SOURCES_EXCLUDE = 
	
	# when parsing the file system looking for include paths exclude this for all or
	# a specific platform
    # ADDON_INCLUDES_EXCLUDE =

linux:
	ADDON_INCLUDES_EXCLUDE = libs/libnice/%
	ADDON_LDFLAGS = -lresolv
	
linux64:
	ADDON_INCLUDES_EXCLUDE = libs/libnice/%
	ADDON_LDFLAGS = -lresolv
	
linuxarmv6l:
	ADDON_INCLUDES_EXCLUDE = libs/libnice/%
	ADDON_LDFLAGS = -lresolv
	
linuxarmv7l:
	ADDON_INCLUDES_EXCLUDE = libs/libnice/%
	ADDON_LDFLAGS = -lresolv
	
win_cb:
	ADDON_LDFLAGS = -lIphlpapi -L$(GSTREAMER_1_0_ROOT_X86)lib -lgstreamer-1.0 -lgstapp-1.0 -lgstvideo-1.0 -lgstbase-1.0 -lgstnet-1.0 -lgstaudio-1.0 -lgio-2.0 -lpangowin32-1.0 -lgdi32 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lffi -lws2_32 -lole32 -lwinmm -lshlwapi  -lintl -lglib-2.0
	
vs:
	ADDON_LIBS = libs/libnice/lib/vs/libnice.lib
	ADDON_LIBS += libs/libnice/lib/vs/gstnice.lib
	ADDON_DLLS_TO_COPY = libs/libnice/lib/vs/libnice.dll
	ADDON_LIBS += Iphlpapi.lib
	
android/armeabi:	
	ADDON_LDFLAGS = -lresolv
	
android/armeabi-v7a:	
	ADDON_LDFLAGS = -lresolv

osx:
	ADDON_LDFLAGS = -lresolv
    
ios:
	ADDON_LDFLAGS = -lresolv



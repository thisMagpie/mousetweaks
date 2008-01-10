INCLUDES =			\
	$(GLADE_CFLAGS)		\
	$(PANEL_CFLAGS)		\
	$(CSPI_CFLAGS)		\
	$(DBUS_CFLAGS)		\
	$(LIBDAEMON_CFLAGS)

AM_CFLAGS =                            \
	-DGLADE_PATH=\"$(gladedir)\"   \
	-DLOCALEDIR=\"$(localedir)\"   \
	-Wall -export-dynamic

if BUILD_PCA
pca_bin = pointer-capture-applet
endif

if BUILD_CTA 
cta_bin = dwell-click-applet
endif

bin_PROGRAMS = mousetweaks mousetweaks-preferences $(pca_bin) $(cta_bin)
mousetweaks_SOURCES =		\
	mt-main.c		\
	mt-common.c		\
	mt-session.c		\
	mt-dbus.c		\
	mt-ctw.c

mousetweaks_LDFLAGS =		\
	$(GLADE_LIBS)		\
	$(CSPI_LIBS)		\
	$(DBUS_LIBS)		\
	$(GNOMEUI_LIBS)     	\
	$(LIBDAEMON_LIBS)

mousetweaks_preferences_SOURCES =	\
	preferences.c			\
	mt-common.c

mousetweaks_preferences_LDFLAGS = \
	$(GLADE_LIBS)		  \
	$(GNOMEUI_LIBS)

dwell_click_applet_SOURCES = dwell-click-applet.c
dwell_click_applet_LDFLAGS = \
	$(GLADE_LIBS)	     \
	$(PANEL_LIBS)	     \
	$(DBUS_LIBS)

pointer_capture_applet_SOURCES = pointer-capture-applet.c
pointer_capture_applet_LDFLAGS = \
	$(GLADE_LIBS)		 \
	$(PANEL_LIBS)

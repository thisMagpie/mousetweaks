if BUILD_PCA
pca_schemas = pointer-capture-applet.schemas
pca_glade   = pointer-capture-applet.glade
pca_server  = PointerCapture_Factory.server.in
endif

if BUILD_CTA
cta_glade  = dwell-click-applet.glade
cta_server = DwellClick_Factory.server.in
endif

serverdir = $(libdir)/bonobo/servers
server_in_files = $(pca_server)	$(cta_server)
server_DATA = $(server_in_files:.server.in=.server)

if BUILD_PCA
$(pca_server): $(pca_server).in Makefile
	sed -e "s|\@bindir\@|$(bindir)|g" < $< > $@
endif

if BUILD_CTA
$(cta_server): $(cta_server).in Makefile
	sed -e "s|\@bindir\@|$(bindir)|g" < $< > $@
endif

@INTLTOOL_SERVER_RULE@

xmldir = $(gladedir)
xml_DATA = mousetweaks.glade ctw.glade $(pca_glade) $(cta_glade)

desktopdir = $(deskdir)
desktop_in_files = mousetweaks.desktop.in mousetweaks-preferences.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

autostartdir = /usr/share/gnome/autostart
autostart_DATA = mousetweaks.desktop

schemasdir = $(GCONF_SCHEMA_FILE_DIR)
schemas_DATA = mousetweaks.schemas $(pca_schemas)

if GCONF_SCHEMAS_INSTALL
install-data-local: install-schemas
install-schemas:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
	$(GCONFTOOL) --makefile-install-rule mousetweaks.schemas
if BUILD_PCA
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
	$(GCONFTOOL) --makefile-install-rule $(pca_schemas)
endif

uninstall-local: uninstall-schemas
uninstall-schemas:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
	$(GCONFTOOL) --makefile-uninstall-rule mousetweaks.schemas

if BUILD_PCA
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
	$(GCONFTOOL) --makefile-uninstall-rule $(pca_schemas)
endif

else
install-data-local:
uninstall-local:
endif

EXTRA_DIST =		      \
	$(xml_DATA)	      \
	$(schemas_DATA)       \
	$(desktop_in_files)   \
	$(autostart_DATA)     \
	$(server_in_files).in

DISTCLEANFILES =           \
	$(server_DATA)     \
	$(server_in_files) \
	$(desktop_DATA)

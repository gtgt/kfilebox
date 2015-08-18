
# Install hicolor icons to standard locations depending on filename
# (e.g. $prefix/share/icons/hicolor/128x128/apps/kfilebox.png)
ecm_install_icons(ICONS
	128-apps-kfilebox.png
	16-apps-kfilebox.png
	22-apps-kfilebox.png
	24-apps-kfilebox.png
	32-apps-kfilebox.png
	48-apps-kfilebox.png
	64-apps-kfilebox.png
	DESTINATION ${ICON_INSTALL_DIR}
	THEME hicolor
)

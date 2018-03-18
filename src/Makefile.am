## Process this file with automake to produce Makefile.in

## Created by Anjuta



AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GTK_NEMO_COLUMNS_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gtk_nemo_columns

gtk_nemo_columns_SOURCES = \
	main.c \
	gtk-nemo-columns.h \
	gtk-nemo-columns.c	


gtk_nemo_columns_LDFLAGS = \
	-Wl,--export-dynamic

gtk_nemo_columns_LDADD = $(GTK_NEMO_COLUMNS_LIBS)





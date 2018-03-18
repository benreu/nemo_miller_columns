/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gtk-nemo-columns.h
 * Copyright (C) 2018 reuben <reuben@reuben-house>
 * 
 * gtk-nemo-columns is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gtk-nemo-columns is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GTK_NEMO_COLUMNS_
#define _GTK_NEMO_COLUMNS_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_NEMO_COLUMNS_TYPE_APPLICATION             (gtk_nemo_columns_get_type ())
#define GTK_NEMO_COLUMNS_APPLICATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_NEMO_COLUMNS_TYPE_APPLICATION, Gtknemocolumns))
#define GTK_NEMO_COLUMNS_APPLICATION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_NEMO_COLUMNS_TYPE_APPLICATION, GtknemocolumnsClass))
#define GTK_NEMO_COLUMNS_IS_APPLICATION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_NEMO_COLUMNS_TYPE_APPLICATION))
#define GTK_NEMO_COLUMNS_IS_APPLICATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_NEMO_COLUMNS_TYPE_APPLICATION))
#define GTK_NEMO_COLUMNS_APPLICATION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_NEMO_COLUMNS_TYPE_APPLICATION, GtknemocolumnsClass))

typedef struct _GtknemocolumnsClass GtknemocolumnsClass;
typedef struct _Gtknemocolumns Gtknemocolumns;


struct _GtknemocolumnsClass
{
	GtkApplicationClass parent_class;
};

struct _Gtknemocolumns
{
	GtkApplication parent_instance;

};


GType gtk_nemo_columns_get_type (void) G_GNUC_CONST;
Gtknemocolumns *gtk_nemo_columns_new (void);

/* Callbacks */

G_END_DECLS

#endif /* _APPLICATION_H_ */


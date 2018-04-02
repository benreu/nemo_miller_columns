/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gtk-nemo-columns.c
 * Copyright (C) 2018 reuben
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
#include <limits.h>       //For PATH_MAX
#include "gtk-nemo-columns.h"
#include <glib/gi18n.h>
#include <dirent.h> 
#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


static GtkWidget *h_scrolled_window;
static void treeview_selection_changed (GtkTreeView *view, 
                                    GtkTreeModel *model, 
                                    GtkPaned *parent_pane);
static void preview_size_changed (GtkPaned *paned);
static void render_preview_image ();
static GtkWidget *preview_label;
static GtkWidget *preview_image;
static int preview_size = 12;
char dir_str[PATH_MAX + 1];


G_DEFINE_TYPE (Gtknemocolumns, gtk_nemo_columns, GTK_TYPE_APPLICATION);

/* ANJUTA: Macro GTK_NEMO_COLUMNS_APPLICATION gets Gtknemocolumns - DO NOT REMOVE */

/* Create a new window  */
static GtkListStore *create_and_fill_model (char* directory){
	GtkListStore *model;
	DIR *d;
	GtkTreeIter iter;
	
	//g_print ("file is: %s\n", fullPath);
	model = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
	struct dirent *dir;
	//char *realpath;
	d = opendir(directory);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			char buf[PATH_MAX + 1];
			realpath(directory, buf);
			gtk_list_store_append (model, &iter);
			gtk_list_store_set (model, &iter, 0, dir->d_name, 1, buf, -1);
		}
	closedir(d);
	}
	return GTK_LIST_STORE(model);
}

static void scroll_to_right (GtkAdjustment *h_adjustment){
	int size = gtk_adjustment_get_upper (h_adjustment);
	size -= gtk_adjustment_get_page_size (h_adjustment);
	gtk_adjustment_set_value (GTK_ADJUSTMENT(h_adjustment), size);
}

static void create_new_browser_pane (char *dir_str, GtkPaned *parent_pane){
	
	GtkWidget *pane;
	GtkWidget *treeview;
	GtkWidget *previous_treeview;
	GtkListStore *new_model;
	GtkCellRenderer *renderer;
	GtkWidget *new_s_window;
	GtkAdjustment *h_adjustment;
	DIR *d;
	
	d = opendir(dir_str);
	if (d){
		//remove the folder previously displayed, if exists
		previous_treeview = gtk_paned_get_child2 (GTK_PANED(parent_pane));
		if (previous_treeview){
			gtk_container_remove (GTK_CONTAINER(parent_pane),
			                    gtk_paned_get_child2 (GTK_PANED(parent_pane)));
		}
		//create new pane
		pane = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
		gtk_paned_set_position (GTK_PANED(pane), 150);
		gtk_paned_pack2 (GTK_PANED (parent_pane), pane, FALSE, FALSE);
		//create new treeview
		new_model = create_and_fill_model (dir_str);
		treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(new_model));
		g_object_unref (new_model);
		gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(treeview), FALSE);
		renderer = gtk_cell_renderer_text_new ();
		//g_object_set(renderer, "ellipsize-set", FALSE);
		g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_MIDDLE, NULL);
		gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
			                                       -1, "",  renderer,
			                                       "text", 0, NULL);
		//create new scrolled window
		new_s_window = gtk_scrolled_window_new (NULL, NULL);
		gtk_widget_set_size_request (GTK_WIDGET(new_s_window), 150, 150);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(new_s_window), 
				                    	GTK_POLICY_NEVER, 
		                                GTK_POLICY_AUTOMATIC);
		//add the treeview to the scrolled window
		gtk_container_add(GTK_CONTAINER (new_s_window), treeview);
		gtk_paned_pack1 (GTK_PANED (pane), new_s_window, FALSE, FALSE);
		//connect the signal for when selecting a file
		g_signal_connect(G_OBJECT(treeview), "cursor-changed", 
		                 G_CALLBACK(treeview_selection_changed), pane);
		//show everything we added
		gtk_widget_show_all (GTK_WIDGET (parent_pane));
		//scroll to the far right
		h_adjustment = gtk_scrolled_window_get_hadjustment 
						(GTK_SCROLLED_WINDOW(h_scrolled_window));
		g_idle_add((GSourceFunc )scroll_to_right, h_adjustment);
	}
	closedir(d);
}
	


static void treeview_selection_changed (GtkTreeView *view, 
                                    GtkTreeModel *model, 
                                    GtkPaned *parent_pane){
	GtkTreeSelection *selection;
	GtkTreeIter       iter;
	GdkPixbuf *pixbuf;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
		gchar *name;
		gchar *path;
		gtk_tree_model_get (model, &iter, 0, &name, -1);
		gtk_tree_model_get (model, &iter, 1, &path, -1);
		strcpy(dir_str, path);
		strcat(dir_str, "/");
		strcat(dir_str, name);
		struct stat path_stat;
		stat(dir_str, &path_stat);
		if (S_ISDIR(path_stat.st_mode)){
			create_new_browser_pane(dir_str, parent_pane);
			pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 1, 1);
			gtk_image_set_from_pixbuf (GTK_IMAGE(preview_image), pixbuf);
		}
		else if (S_ISREG(path_stat.st_mode)){
			render_preview_image ();
			//FIXME
		}
		gtk_label_set_label (GTK_LABEL(preview_label),name);
		g_free(name);
		g_free(path);
	}
}

static void limit_preview_size (GtkPaned *paned){
	int position = gtk_widget_get_allocated_width(GTK_WIDGET(paned)) - 101;
	gtk_paned_set_position (GTK_PANED(paned), position);
}

static void preview_size_changed (GtkPaned *paned){
	//offset of 5 when calculating reverse position
	preview_size = gtk_widget_get_allocated_width(GTK_WIDGET(paned)) - 
					gtk_paned_get_position (GTK_PANED(paned)) - 5;
	if (preview_size < 96){
		g_idle_add ((GSourceFunc)limit_preview_size, paned);
		preview_size = 96;
	}
	render_preview_image ();
}

static void render_preview_image (){
	GdkPixbuf *pixbuf;

	pixbuf = gdk_pixbuf_new_from_file_at_scale (dir_str, 
			                                    preview_size, 
			                                    preview_size, 
			                                    TRUE, NULL);
	gtk_image_set_from_pixbuf (GTK_IMAGE(preview_image), pixbuf);
	gtk_widget_set_size_request (GTK_WIDGET(preview_label), preview_size, -1);
}

/* Create a new window  */

static void
gtk_nemo_columns_new_window (GApplication *app)
{
	GtkWidget *window;
	GtkWidget *preview_box;
	GtkWidget *pane;
	GtkWidget *s_window;
	GtkWidget *treeview;
	GtkListStore *model;
	GtkCellRenderer *renderer;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "gtk-nemo-columns");
	
	gtk_window_set_application (GTK_WINDOW (window), GTK_APPLICATION (app));
	//create a box for file preview purposes
	preview_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	preview_label = gtk_label_new ("");
	gtk_label_set_line_wrap (GTK_LABEL(preview_label), TRUE);
	gtk_widget_set_size_request (GTK_WIDGET(preview_box), 12, 12);
	preview_image = gtk_image_new ();
	gtk_box_pack_start (GTK_BOX (preview_box), 
	                  GTK_WIDGET(preview_image), FALSE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX (preview_box), 
	                  GTK_WIDGET(preview_label), FALSE, FALSE, 5);
	//create pane to pack the side scrolling window and the preview box into
	pane = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_container_add(GTK_CONTAINER (window), pane);
	g_signal_connect(G_OBJECT(pane), "size-allocate", 
	                 G_CALLBACK(preview_size_changed), NULL);
	g_idle_add ((GSourceFunc)limit_preview_size, pane);
	//add the preview box to the pane
	gtk_paned_pack2 (GTK_PANED (pane), preview_box, TRUE, TRUE);
	//create a side scrolling window, add to pane
	h_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(h_scrolled_window), 
	                            	GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
	gtk_paned_pack1 (GTK_PANED (pane), h_scrolled_window, FALSE, FALSE);
	//create a horizontal pane, add to horizontal scrolled window
	pane = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_paned_set_position (GTK_PANED(pane), 150);
	gtk_container_add(GTK_CONTAINER (h_scrolled_window), pane);
	//add a scrolled window
	s_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(s_window), 
	                            	GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request (GTK_WIDGET(s_window), 150, 150);
	//create treeview with the primary folder
	char dir[] = "..";
	model = create_and_fill_model (dir);
	treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(model));
	g_object_unref (model);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(treeview), FALSE);
	renderer = gtk_cell_renderer_text_new ();
	g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_MIDDLE, NULL);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                               -1, "", renderer,"text", 0, NULL);
	
	gtk_container_add(GTK_CONTAINER (s_window), treeview);
	gtk_paned_pack1 (GTK_PANED (pane), s_window, FALSE, FALSE);
	//connect the signal for when selecting a file
	g_signal_connect(G_OBJECT(treeview), "cursor-changed", 
	                 G_CALLBACK(treeview_selection_changed), pane);
	//set an initial size to the window and show it
	gtk_window_set_default_size (GTK_WINDOW(window), 600, 400);
	gtk_widget_show_all (GTK_WIDGET (window));
}

/* GApplication implementation */
static void
gtk_nemo_columns_activate (GApplication *application)
{
	gtk_nemo_columns_new_window (application);
}

static void
gtk_nemo_columns_open (GApplication  *application,
                     GFile        **files,
                     gint           n_files,
                     const gchar   *hint)
{
	gtk_nemo_columns_new_window (application);
}

static void
gtk_nemo_columns_init (Gtknemocolumns *object)
{
}

static void
gtk_nemo_columns_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtk_nemo_columns_parent_class)->finalize (object);
}

static void
gtk_nemo_columns_class_init (GtknemocolumnsClass *klass)
{
	G_APPLICATION_CLASS (klass)->activate = gtk_nemo_columns_activate;
	G_APPLICATION_CLASS (klass)->open = gtk_nemo_columns_open;

	G_OBJECT_CLASS (klass)->finalize = gtk_nemo_columns_finalize;
}

Gtknemocolumns *
gtk_nemo_columns_new (void)
{
	return g_object_new (gtk_nemo_columns_get_type (),
	                     "application-id", "org.gnome.gtk_nemo_columns",
	                     "flags", G_APPLICATION_HANDLES_OPEN,
	                     NULL);
}


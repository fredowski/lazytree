#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "exampleapp.h"


struct _ExampleApp
{
  GtkApplication parent;
};

struct _ExampleAppClass
{
  GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(ExampleApp, example_app, GTK_TYPE_APPLICATION);
const int nr_of_columns = 20;
const int nr_of_rows = 300;

static GtkTreeModel *
create_model (void)
{
  gint i = 0;
  GtkListStore *store;
  GtkTreeIter iter;
  GType typearray[nr_of_columns];
  gchar string[100];

  for(i=0;i<nr_of_columns;i++)
    {
      typearray[i]=G_TYPE_STRING;
    }

  /* create list store */
  store = gtk_list_store_newv (nr_of_columns,
                               typearray);

  /* add data to the list store */
  for (i = 0; i < nr_of_rows; i++)
    {
      gtk_list_store_append (store, &iter);
      for(int c = 0;c < nr_of_columns;c++)
        {
          GValue value = G_VALUE_INIT;
          g_value_init(&value, G_TYPE_STRING);
          g_sprintf(string,"Row: %d, Column: %d",i,c);
          g_value_set_string (&value, string);
          gtk_list_store_set_value(store, &iter, c, &value);
        }
    }

  return GTK_TREE_MODEL (store);
}

static void
add_columns(GtkTreeView *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  for (int c = 0;c < nr_of_columns;c++){
    renderer = gtk_cell_renderer_text_new ();
    g_object_set (renderer,
                  "editable", TRUE,
                  NULL);
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title (column, "Fred");
    gtk_tree_view_column_pack_start (column, renderer, FALSE);
    gtk_tree_view_column_add_attribute (column,
                                        renderer,
                                        "text",
                                        c);
    gtk_tree_view_append_column (treeview, column);

  }
}

static void
example_app_init (ExampleApp *app)
{
}

static void
example_app_activate (GApplication *app)
{
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkTreeModel *model;

  printf("%s\n",__FUNCTION__);
  
  window = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_default_size ( GTK_WINDOW (window), 800, 480);

  model = create_model();


  treeview = gtk_tree_view_new_with_model (model);
  add_columns (GTK_TREE_VIEW (treeview));
  sw = gtk_scrolled_window_new(NULL,NULL);
  gtk_container_add (GTK_CONTAINER (sw), treeview);
  
  label = gtk_label_new ("Hallo!!!!");
  gtk_container_add (GTK_CONTAINER (window), sw);
  gtk_widget_show_all (sw);
  gtk_window_present (GTK_WINDOW (window));
}

static void
example_app_class_init (ExampleAppClass *class)
{
  G_APPLICATION_CLASS (class)->activate = example_app_activate;
}

ExampleApp *
example_app_new (void)
{
  return g_object_new (EXAMPLE_APP_TYPE,
                       "application-id", "org.gtk.exampleapp",
                       NULL);
}

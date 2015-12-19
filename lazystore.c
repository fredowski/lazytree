/* lazytree - a lazy treeview
   Copyright (C) 2015 Friedrich Beckmann

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/* This lazystore is an example for a treestore which does produce
   data on demand. No data is actually stored here. The main purpose
   is to provide a GktTreeModel Interface for the treeview to access
   the data*/

#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include "lazystore.h"

struct _LazyStore
{
  GObject parent;

  /* private */
  guint n_columns;
  guint n_rows;
  guint stamp;
};


/* GtkTreeModel Interface */
static void         lazy_store_tree_model_init (GtkTreeModelIface *iface);
static GtkTreeModelFlags lazy_store_get_flags  (GtkTreeModel      *tree_model);
static gint         lazy_store_get_n_columns   (GtkTreeModel      *tree_model);
static GType        lazy_store_get_column_type (GtkTreeModel      *tree_model,
                                                gint               index);
static gboolean     lazy_store_get_iter        (GtkTreeModel      *tree_model,
                                                GtkTreeIter       *iter,
                                                GtkTreePath       *path);
static GtkTreePath *lazy_store_get_path        (GtkTreeModel      *tree_model,
                                                GtkTreeIter       *iter);
static void         lazy_store_get_value       (GtkTreeModel      *tree_model,
                                                GtkTreeIter       *iter,
                                                gint               column,
                                                GValue            *value);
static gboolean     lazy_store_iter_next       (GtkTreeModel      *tree_model,
                                                GtkTreeIter       *iter);
static gboolean     lazy_store_iter_previous   (GtkTreeModel      *tree_model,
                                                GtkTreeIter       *iter);
static gboolean     lazy_store_iter_children   (GtkTreeModel      *tree_model,
                                                GtkTreeIter       *iter,
                                                GtkTreeIter       *parent);
static gboolean     lazy_store_iter_has_child  (GtkTreeModel      *tree_model,
                                                GtkTreeIter       *iter);
static gint         lazy_store_iter_n_children (GtkTreeModel      *tree_model,
                                                GtkTreeIter       *iter);
static gboolean     lazy_store_iter_nth_child  (GtkTreeModel      *tree_model,
                                                GtkTreeIter       *iter,
                                                GtkTreeIter       *parent,
                                                gint               n);
static gboolean     lazy_store_iter_parent     (GtkTreeModel      *tree_model,
                                                GtkTreeIter       *iter,
                                                GtkTreeIter       *child);

G_DEFINE_TYPE_WITH_CODE (LazyStore, lazy_store, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL,
                                                lazy_store_tree_model_init))


static void
lazy_store_class_init (LazyStoreClass *class)
{

}

static void
lazy_store_tree_model_init (GtkTreeModelIface *iface)
{
  iface->get_flags = lazy_store_get_flags;
  iface->get_n_columns = lazy_store_get_n_columns;
  iface->get_column_type = lazy_store_get_column_type;
  iface->get_iter = lazy_store_get_iter;
  iface->get_path = lazy_store_get_path;
  iface->get_value = lazy_store_get_value;
  iface->iter_next = lazy_store_iter_next;
  iface->iter_previous = lazy_store_iter_previous;
  iface->iter_children = lazy_store_iter_children;
  iface->iter_has_child = lazy_store_iter_has_child;
  iface->iter_n_children = lazy_store_iter_n_children;
  iface->iter_nth_child = lazy_store_iter_nth_child;
  iface->iter_parent = lazy_store_iter_parent;
}

static void
lazy_store_init (LazyStore *lazy_store)
{
  lazy_store->n_columns = 30000;
  lazy_store->n_rows = 100;
  lazy_store->stamp = g_random_int ();
}


LazyStore *
lazy_store_new (void)
{
  return g_object_new (TYPE_LAZY_STORE, NULL);
}


/* Fulfill the GtkTreeModel requirements */
static GtkTreeModelFlags
lazy_store_get_flags (GtkTreeModel *tree_model)
{
  return GTK_TREE_MODEL_ITERS_PERSIST | GTK_TREE_MODEL_LIST_ONLY;
}

static gint
lazy_store_get_n_columns (GtkTreeModel *tree_model)
{
  LazyStore *lazy_store = LAZY_STORE (tree_model);

  return lazy_store->n_columns;
}

static GType
lazy_store_get_column_type (GtkTreeModel *tree_model,
                            gint          index)
{
  LazyStore *lazy_store = LAZY_STORE (tree_model);

  g_return_val_if_fail (index < lazy_store->n_columns, G_TYPE_INVALID);

  return G_TYPE_STRING;
}

static gboolean
lazy_store_get_iter (GtkTreeModel *tree_model,
                     GtkTreeIter  *iter,
                     GtkTreePath  *path)
{
  LazyStore *lazy_store = LAZY_STORE (tree_model);
  gint i;

  i = gtk_tree_path_get_indices (path)[0];

  if (i >= lazy_store->n_rows)
    {
      return FALSE;
    }

  iter->stamp = lazy_store->stamp;
  iter->user_data = (gpointer)(intptr_t)i;

  return TRUE;
}

static GtkTreePath *
lazy_store_get_path (GtkTreeModel *tree_model,
                     GtkTreeIter  *iter)
{
  LazyStore *lazy_store = LAZY_STORE (tree_model);

  GtkTreePath *path;

  if ((guint)(intptr_t)iter->user_data >= lazy_store->n_rows)
    return NULL;
  path = gtk_tree_path_new ();
  gtk_tree_path_append_index (path, (gulong)iter->user_data);
  return path;
}

static void
lazy_store_get_value (GtkTreeModel *tree_model,
                      GtkTreeIter  *iter,
                      gint          column,
                      GValue       *value)
{
  LazyStore *lazy_store = LAZY_STORE (tree_model);
  gchar string[100];

  g_return_if_fail (column < lazy_store->n_columns);
  //printf("%s - row: %d, col: %d\n", __FUNCTION__,(gint) iter->user_data,column);
  g_return_if_fail ((guint)(intptr_t)iter->user_data < lazy_store->n_rows);

  g_value_init (value, G_TYPE_STRING);
  g_sprintf(string,"Row: %d, Column: %d",(gint)(intptr_t)iter->user_data,column);
  g_value_set_string (value, string);
}

static gboolean
lazy_store_iter_next (GtkTreeModel  *tree_model,
                      GtkTreeIter   *iter)
{
  LazyStore *lazy_store = LAZY_STORE (tree_model);

  iter->user_data++;

  if ((guint)(intptr_t)iter->user_data >= lazy_store->n_rows)
    {
      iter->stamp = 0;
      return FALSE;
    }
  return TRUE;
}

static gboolean
lazy_store_iter_previous (GtkTreeModel *tree_model,
                          GtkTreeIter  *iter)
{
  LazyStore *lazy_store = LAZY_STORE (tree_model);

  g_return_val_if_fail (lazy_store->stamp == iter->stamp, FALSE);

  if (iter->user_data == 0)
    {
      iter->stamp = 0;
      return FALSE;
    }

  iter->user_data--;

  return TRUE;
}

static gboolean
lazy_store_iter_children (GtkTreeModel *tree_model,
                          GtkTreeIter  *iter,
                          GtkTreeIter  *parent)
{
  LazyStore *lazy_store = (LazyStore *) tree_model;

  /* this is a list, nodes have no children */
  if (parent)
    {
      iter->stamp = 0;
      return FALSE;
    }

  if (lazy_store->n_rows > 0)
    {
      iter->stamp = lazy_store->stamp;
      iter->user_data = 0;
      return TRUE;
    }
  else
    {
      iter->stamp = 0;
      return FALSE;
    }
}

static gboolean
lazy_store_iter_has_child (GtkTreeModel *tree_model,
                           GtkTreeIter  *iter)
{
  return FALSE;
}

static gint
lazy_store_iter_n_children (GtkTreeModel *tree_model,
                            GtkTreeIter  *iter)
{
  LazyStore *lazy_store = LAZY_STORE (tree_model);

  if (iter == NULL)
    return lazy_store->n_rows;

  g_return_val_if_fail (lazy_store->stamp == iter->stamp, -1);

  return 0;
}

static gboolean
lazy_store_iter_nth_child (GtkTreeModel *tree_model,
                           GtkTreeIter  *iter,
                           GtkTreeIter  *parent,
                           gint          n)
{
  LazyStore *lazy_store = LAZY_STORE (tree_model);

  iter->stamp = 0;

  if (parent)
    return FALSE;

  if (n >= lazy_store->n_rows)
    return FALSE;

  iter->stamp = lazy_store->stamp;
  iter->user_data = (gpointer)(intptr_t)n;

  return TRUE;
}

static gboolean
lazy_store_iter_parent (GtkTreeModel *tree_model,
                        GtkTreeIter  *iter,
                        GtkTreeIter  *child)
{
  iter->stamp = 0;
  return FALSE;
}


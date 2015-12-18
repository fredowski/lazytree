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

#include <gtk/gtk.h>

#include "lazytreeview.h"

/* Properties */
enum {
  PROP_0,
  PROP_FRED,
  LAST_PROP,
  /* overridden */
  PROP_HADJUSTMENT = LAST_PROP,
  PROP_VADJUSTMENT,
  PROP_HSCROLL_POLICY,
  PROP_VSCROLL_POLICY,
};

struct _LazyTreeView
{
  GtkTreeView parent;
  /* Adjustments */
  GtkAdjustment *hadjustment;
  GtkAdjustment *vadjustment;
  /* GtkScrollablePolicy needs to be checked when
   * driving the scrollable adjustment values */
  guint hscroll_policy : 1;
  guint vscroll_policy : 1;
  
};

struct _LazyTreeViewClass
{
  GtkTreeViewClass parent_class;
};

G_DEFINE_TYPE(LazyTreeView, lazy_tree_view, GTK_TYPE_TREE_VIEW);


static void
print_adj(const char *name, GtkAdjustment *adj)
{
  if (adj == NULL)
    printf("%s, adj is NULL\n", name);
  else
    printf("%s, value: %lf, lower: %lf, upper: %lf\n",
           name,
           gtk_adjustment_get_value (adj),
           gtk_adjustment_get_lower (adj),
           gtk_adjustment_get_upper (adj));
  
}
/* Callbacks */
static void
lazy_tree_view_hadjustment_changed (GtkAdjustment *adjustment,
                                   LazyTreeView   *tree_view)
{

  GtkTreeModel *model;
  guint ncol_in_tree_view, ncol_in_model, col_offset = 0;

  ncol_in_tree_view = gtk_tree_view_get_n_columns (GTK_TREE_VIEW (tree_view));
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
  ncol_in_model = gtk_tree_model_get_n_columns (model);
  if (gtk_adjustment_get_upper (adjustment) > 0.0)
    col_offset = (ncol_in_model - ncol_in_tree_view) *
      (gtk_adjustment_get_value (adjustment) / gtk_adjustment_get_upper (adjustment));
    
  
  for(int ctree = 0;ctree < ncol_in_tree_view;ctree++)
    {
      GtkTreeViewColumn *column;
      GtkCellRenderer *renderer;
      GList *list;
      column = gtk_tree_view_get_column (GTK_TREE_VIEW (tree_view),ctree);
      list = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (column));
      renderer = GTK_CELL_RENDERER (list->data);
      gtk_tree_view_column_set_attributes (column, renderer,
                                           "text", ctree + col_offset, NULL);
      gtk_tree_view_column_queue_resize (column);
      
    }
  
  print_adj (__FUNCTION__, adjustment);

  
}

static void
lazy_tree_view_vadjustment_changed (GtkAdjustment *adjustment,
                                   LazyTreeView   *tree_view)
{
  print_adj (__FUNCTION__, adjustment);
}

static GtkAdjustment *
lazy_tree_view_do_get_hadjustment (LazyTreeView *tree_view)
{
  return tree_view->hadjustment;
}

static void
lazy_tree_view_do_set_hadjustment (LazyTreeView   *tree_view,
                                   GtkAdjustment *adjustment)
{
  print_adj (__FUNCTION__, adjustment);
  if (adjustment && tree_view->hadjustment == adjustment)
    return;

  if (tree_view->hadjustment != NULL)
    {
      g_signal_handlers_disconnect_by_func (tree_view->hadjustment,
                                            lazy_tree_view_hadjustment_changed,
                                            tree_view);
      g_object_unref (tree_view->hadjustment);
    }

  if (adjustment == NULL)
    adjustment = gtk_adjustment_new (0.0, 0.0, 1.0,
                                     0.1, 0.2, 0.0);

  g_signal_connect (adjustment, "value-changed",
                    G_CALLBACK (lazy_tree_view_hadjustment_changed), tree_view);
  tree_view->hadjustment = g_object_ref_sink (adjustment);
}

static GtkAdjustment *
lazy_tree_view_do_get_vadjustment (LazyTreeView *tree_view)
{
  return tree_view->vadjustment;
}

static void
lazy_tree_view_do_set_vadjustment (LazyTreeView   *tree_view,
                                   GtkAdjustment *adjustment)
{
  print_adj (__FUNCTION__, adjustment);
  if (adjustment && tree_view->vadjustment == adjustment)
    return;

  if (tree_view->vadjustment != NULL)
    {
      g_signal_handlers_disconnect_by_func (tree_view->vadjustment,
                                            lazy_tree_view_vadjustment_changed,
                                            tree_view);
      g_object_unref (tree_view->vadjustment);
    }

  if (adjustment == NULL)
    adjustment = gtk_adjustment_new (0.1, 0.0, 2000.0,
                                     0.1, 0.2, 0.0);

  g_signal_connect (adjustment, "value-changed",
                    G_CALLBACK (lazy_tree_view_vadjustment_changed), tree_view);
  tree_view->vadjustment = g_object_ref_sink (adjustment);
}


/* GObject Methods
 */

static void
lazy_tree_view_set_property (GObject         *object,
                             guint            prop_id,
                             const GValue    *value,
                             GParamSpec      *pspec)
{
  LazyTreeView *tree_view;

  tree_view = LAZY_TREE_VIEW (object);
  printf("%s - prop_id %d\n", __FUNCTION__, prop_id);
  switch (prop_id)
    {
    case PROP_HADJUSTMENT:
      lazy_tree_view_do_set_hadjustment (tree_view, g_value_get_object (value));
      break;
    case PROP_VADJUSTMENT:
      lazy_tree_view_do_set_vadjustment (tree_view, g_value_get_object (value));
      break;
    case PROP_HSCROLL_POLICY:
      if (tree_view->hscroll_policy != g_value_get_enum (value))
        {
          tree_view->hscroll_policy = g_value_get_enum (value);
          g_object_notify_by_pspec (object, pspec);
        }
      break;
    case PROP_VSCROLL_POLICY:
      if (tree_view->vscroll_policy != g_value_get_enum (value))
        {
          tree_view->vscroll_policy = g_value_get_enum (value);
          g_object_notify_by_pspec (object, pspec);
        }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
lazy_tree_view_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  LazyTreeView *tree_view;

  tree_view = LAZY_TREE_VIEW (object);
  printf("%s - prop_id %d\n", __FUNCTION__, prop_id);
  switch (prop_id)
    {
    case PROP_HADJUSTMENT:
      g_value_set_object (value, tree_view->hadjustment);
      break;
    case PROP_VADJUSTMENT:
      g_value_set_object (value, tree_view->vadjustment);
      break;
    case PROP_HSCROLL_POLICY:
      g_value_set_enum (value, tree_view->hscroll_policy);
      break;
    case PROP_VSCROLL_POLICY:
      g_value_set_enum (value, tree_view->vscroll_policy);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
lazy_tree_view_init (LazyTreeView *treeview)
{

  printf("%s\n",__FUNCTION__);
  lazy_tree_view_do_set_vadjustment (treeview, NULL);
  lazy_tree_view_do_set_hadjustment (treeview, NULL);
}

static void
lazy_tree_view_class_init (LazyTreeViewClass *class)
{
  GObjectClass *o_class;

  o_class = (GObjectClass *) class;

  /* GObject signals */
  o_class->set_property = lazy_tree_view_set_property;
  o_class->get_property = lazy_tree_view_get_property;
  /* Properties */
  g_object_class_override_property (o_class, PROP_HADJUSTMENT,    "hadjustment");
  g_object_class_override_property (o_class, PROP_VADJUSTMENT,    "vadjustment");
  g_object_class_override_property (o_class, PROP_HSCROLL_POLICY, "hscroll-policy");
  g_object_class_override_property (o_class, PROP_VSCROLL_POLICY, "vscroll-policy");
  
  printf("%s\n",__FUNCTION__);
}

GtkWidget *
lazy_tree_view_new (void)
{
  printf("%s\n",__FUNCTION__);
  return g_object_new (TYPE_LAZY_TREE_VIEW, NULL);
}


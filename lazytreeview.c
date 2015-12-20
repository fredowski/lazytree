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
  /* Offsets derived from scrollers */
  gint col_offset;
  gint row_offset;
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
  col_offset = gtk_adjustment_get_value (adjustment);
  tree_view->col_offset = col_offset;

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
print_allocation (const char *msg, GtkAllocation *allocation)
{
  if (allocation)
    printf("%s - x: %d, y: %d, width: %d, height: %d\n",
           msg,
           allocation->x,
           allocation->y,
           allocation->width,
           allocation->height);
  else
    printf("%s - allocation is NULL\n",msg);
}

static void
lazy_tree_view_size_allocate (GtkWidget     *widget,
                              GtkAllocation *allocation)
{
  LazyTreeView *tree_view = LAZY_TREE_VIEW (widget);
  GtkTreeModel *model;
  GtkAdjustment *adj;
  guint ncol_in_tree_view, ncol_in_model, col_offset = 0;
  print_allocation(__FUNCTION__, allocation);

  /* Adapt the horizontal adjustments to the number of columns in
     the treeview and in the model */
  ncol_in_tree_view = gtk_tree_view_get_n_columns (GTK_TREE_VIEW (tree_view));
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
  ncol_in_model = gtk_tree_model_get_n_columns (model);
  adj = gtk_scrollable_get_hadjustment (GTK_SCROLLABLE (widget));
  gtk_adjustment_set_upper(adj, ncol_in_model);
  gtk_adjustment_set_page_size(adj, ncol_in_tree_view);
  gtk_adjustment_set_page_increment(adj, ncol_in_tree_view);
  gtk_adjustment_set_step_increment(adj, 1.0);
  gtk_scrollable_set_hadjustment ( GTK_SCROLLABLE (widget), adj);
  /* Chain up */
  GTK_WIDGET_CLASS(lazy_tree_view_parent_class)->size_allocate(widget, allocation);
  print_allocation(__FUNCTION__, allocation);
}

static gboolean
lazy_tree_view_move_cursor (GtkTreeView       *tree_view,
                            GtkMovementStep    step,
                            gint               count)
{
  LazyTreeView *lazy_tree_view = LAZY_TREE_VIEW (tree_view);
  GtkTreePath *path;
  GtkTreeViewColumn *column;
  GtkTreeModel *model;
  gint row_in_treeview = 0;
  gint col_in_treeview = 0;
  guint ncol_in_model = 0;
  guint ncol_in_treeview = 0;
  GList *list;

  printf("%s - step %d, count %d\n", __FUNCTION__, (int) step, count);

  gtk_tree_view_get_cursor (GTK_TREE_VIEW (tree_view), &path, &column);
  row_in_treeview = gtk_tree_path_get_indices (path)[0];

  list = gtk_tree_view_get_columns (tree_view);
  col_in_treeview = g_list_position(list, g_list_find(list, column));

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
  ncol_in_model = gtk_tree_model_get_n_columns (model);
  ncol_in_treeview = gtk_tree_view_get_n_columns (GTK_TREE_VIEW (tree_view));

  printf("row: %d, col: %d, col_offset: %d\n", row_in_treeview, col_in_treeview, lazy_tree_view->col_offset);

  /* Move the last or the first column */
  if (col_in_treeview == 0 && lazy_tree_view->col_offset > 0 &&
      step == GTK_MOVEMENT_VISUAL_POSITIONS &&
      count < 0)
    {
      gtk_tree_view_move_column_after (tree_view,
                                       g_list_last (list)->data, NULL);
      lazy_tree_view->col_offset--;
      gtk_adjustment_set_value (lazy_tree_view->hadjustment,
                                lazy_tree_view->col_offset);
    }
  else if (col_in_treeview == ncol_in_treeview-1 &&
           lazy_tree_view->col_offset + ncol_in_treeview < ncol_in_model &&
           step == GTK_MOVEMENT_VISUAL_POSITIONS &&
           count > 0)
    {
      gtk_tree_view_move_column_after (tree_view,
                                       g_list_first (list)->data,
                                       g_list_last  (list)->data);
      lazy_tree_view->col_offset++;
      gtk_adjustment_set_value (lazy_tree_view->hadjustment,
                                lazy_tree_view->col_offset);
    }
  /* Chain up to GtkTreeView */
  return GTK_TREE_VIEW_CLASS(lazy_tree_view_parent_class)->move_cursor(tree_view, step, count);
}

static void
lazy_tree_view_class_init (LazyTreeViewClass *class)
{
  GObjectClass *o_class;
  GtkWidgetClass *widget_class;
  GtkTreeViewClass *gtktreeview_class;

  o_class = (GObjectClass *) class;
  widget_class = (GtkWidgetClass*) class;
  gtktreeview_class = (GtkTreeViewClass*) class;

  /* GObject signals */
  o_class->set_property = lazy_tree_view_set_property;
  o_class->get_property = lazy_tree_view_get_property;

  /* widget */
  widget_class->size_allocate = lazy_tree_view_size_allocate;

  /* GtkTreeView class */
  gtktreeview_class->move_cursor = lazy_tree_view_move_cursor;

  /* Properties */
  g_object_class_override_property (o_class, PROP_HADJUSTMENT,    "hadjustment");
  //g_object_class_override_property (o_class, PROP_VADJUSTMENT,    "vadjustment");
  g_object_class_override_property (o_class, PROP_HSCROLL_POLICY, "hscroll-policy");
  //g_object_class_override_property (o_class, PROP_VSCROLL_POLICY, "vscroll-policy");

  printf("%s\n",__FUNCTION__);
}

GtkWidget *
lazy_tree_view_new (void)
{
  printf("%s\n",__FUNCTION__);
  return g_object_new (TYPE_LAZY_TREE_VIEW, NULL);
}


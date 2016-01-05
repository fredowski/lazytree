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
};

struct _LazyTreeView
{
  GtkLayout parent;

  /* Gestures */
  GtkGesture *gesture;

  /* Red Rectangle */
  gint red_x;
  gint red_y;
  gint red_width;
  gint red_height;
  gboolean red_move;

  /* The Tree Model */
  GtkTreeModel *model;

  /* Render Data */
  gint col_width;
  gint row_height;
  GtkCellRenderer *renderer;

  /* Offsets derived from scrollers */
  gint col_offset;
  gint row_offset;
};

struct _LazyTreeViewClass
{
  GtkLayoutClass parent_class;
};

G_DEFINE_TYPE (LazyTreeView, lazy_tree_view, GTK_TYPE_LAYOUT)

static void
print_adj(const char *name, GtkAdjustment *adj)
{
  if (adj == NULL)
    printf("%s, adj is NULL\n", name);
  else
    printf("%s, value: %lf, lower: %lf, upper: %lf, page size: %lf, page incr: %lf, step incr %lf\n",
           name,
           gtk_adjustment_get_value (adj),
           gtk_adjustment_get_lower (adj),
           gtk_adjustment_get_upper (adj),
           gtk_adjustment_get_page_size (adj),
           gtk_adjustment_get_page_increment (adj),
           gtk_adjustment_get_step_increment (adj));
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
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

void
drag_begin_cb (GtkGestureDrag *gesture,
               gdouble         start_x,
               gdouble         start_y,
               LazyTreeView    *treeview)
{
  GtkAdjustment *hadj, *vadj;
  gdouble hadj_value, vadj_value;

  hadj = gtk_scrollable_get_hadjustment (GTK_SCROLLABLE (treeview));
  hadj_value = gtk_adjustment_get_value(hadj);
  vadj = gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (treeview));
  vadj_value = gtk_adjustment_get_value(vadj);

  printf("%s - x: %lf, y: %lf, rx: %d, ry: %d, hadj: %lf, vadj: %lf\n",__FUNCTION__,
         start_x, start_y,
         treeview->red_x, treeview->red_y,
         hadj_value, vadj_value);

  if (ABS(start_x + hadj_value - treeview->red_x ) < 5 &&
      ABS(start_y + vadj_value - treeview->red_y ) < 5)
    treeview->red_move = TRUE;
}

void
drag_update_cb (GtkGestureDrag *gesture,
               gdouble         offset_x,
               gdouble         offset_y,
               LazyTreeView    *treeview)
{
  GtkAdjustment *hadj, *vadj;
  gint hadj_value, vadj_value;

  hadj = gtk_scrollable_get_hadjustment (GTK_SCROLLABLE (treeview));
  hadj_value = gtk_adjustment_get_value(hadj);
  vadj = gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (treeview));
  vadj_value = gtk_adjustment_get_value(vadj);

  if (treeview->red_move)
    {
      gdouble startx, starty;
      gtk_gesture_drag_get_start_point (gesture, &startx, &starty);
      printf("%s - x: %lf, y: %lf, ox: %lf, oy: %lf, hadj: %d, vadj: %d\n",__FUNCTION__,
             startx, starty,
             offset_x, offset_y,
             hadj_value, vadj_value);
      treeview->red_x = startx + offset_x + hadj_value;
      treeview->red_y = starty + offset_y + vadj_value;
      gtk_widget_queue_draw (GTK_WIDGET (treeview));
    }
}

void
drag_end_cb (GtkGestureDrag *gesture,
             gdouble         offset_x,
             gdouble         offset_y,
             LazyTreeView    *treeview)
{
  printf("%s - x: %d, y: %d\n",__FUNCTION__, treeview->red_x, treeview->red_y);

  if (treeview->red_move)
    {
      GtkWidget *button = gtk_button_new_with_label ("Moved the red rectangle");
      gtk_layout_put ( GTK_LAYOUT (treeview), button, treeview->red_x, treeview->red_y);
      gtk_widget_show (button);

      //treeview->red_x += offset_x;
      //treeview->red_y += offset_y;
    }
  treeview->red_move = FALSE;
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
print_widget (const char *msg, GtkWidget *widget)
{
  printf("%s - realized: %d, visible: %d, mapped: %d, has-screen: %d, is-drawable: %d\n", msg,
         gtk_widget_get_realized (widget),
         gtk_widget_is_visible (widget),
         gtk_widget_get_mapped (widget),
         gtk_widget_has_screen (widget),
         gtk_widget_is_drawable (widget));
}

static void
print_window (const char *msg, GdkWindow *window)
{
  printf("%s - visible: %d, viewable: %d\n", msg,
         gdk_window_is_visible (window),
         gdk_window_is_viewable (window));
}

static gboolean
lazy_tree_view_draw (GtkWidget *widget,
                     cairo_t   *cr)
{
  LazyTreeView *tree_view = LAZY_TREE_VIEW (widget);
  GtkWidget   *button;
  GtkStyleContext *context;
  GdkWindow *window;
  GtkAdjustment *hadj = gtk_scrollable_get_hadjustment (GTK_SCROLLABLE (widget));
  GtkAdjustment *vadj = gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (widget));
  gdouble hadj_value = 0.0;
  gdouble vadj_value = 0.0;

  if (hadj)
    hadj_value = gtk_adjustment_get_value(hadj);
  if (vadj)
    vadj_value = gtk_adjustment_get_value(vadj);

  context = gtk_widget_get_style_context (widget);
  gtk_style_context_save (context);
  cairo_save (cr);
  cairo_set_source_rgba (cr, 0.99, 0.0, 0.5, 0.8);
  //cairo_paint (cr);
  cairo_set_source_rgb (cr, 0.0, 0.9, 0.9);
  for (int i = -100; i < 300; i+=50)
    cairo_rectangle (cr,
                     i - hadj_value, i - vadj_value,
                     100,
                     100);
  cairo_stroke (cr);

  /* The red rectangle */
  cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
  cairo_rectangle (cr,
                   tree_view->red_x - hadj_value, tree_view->red_y - vadj_value,
                   tree_view->red_width,
                   tree_view->red_height   );
  cairo_stroke (cr);

  cairo_restore (cr);
  gtk_style_context_restore (context);

  /* Here we go */
  {
    GtkTreeIter iter;

    guint total_width, total_height;
    guint row;
    gint y;
    gboolean valid;
    gtk_layout_get_size( GTK_LAYOUT (tree_view), &total_width, &total_height);

    row = vadj_value / total_height * gtk_tree_model_iter_n_children (tree_view->model, NULL);

    valid = gtk_tree_model_iter_nth_child (tree_view->model, &iter, NULL, row);
    y = -vadj_value + row * tree_view->row_height;
    while (valid && y < gtk_adjustment_get_page_size (vadj))
      {
        guint col = hadj_value / total_width * gtk_tree_model_get_n_columns(tree_view->model);
        gint x = -hadj_value + col * tree_view->col_width;
        for(;col < gtk_tree_model_get_n_columns(tree_view->model) && x < gtk_adjustment_get_page_size (hadj);col++, x+=tree_view->col_width)
          {
            GdkRectangle rect;
            GValue val = G_VALUE_INIT;
            rect.x = x;
            rect.y = y;
            rect.width  = tree_view->col_width;
            rect.height = tree_view->row_height;

            gtk_tree_model_get_value (tree_view->model, &iter, col, &val);
            g_object_set ( G_OBJECT (tree_view->renderer),
                           "text", g_value_get_string (&val), NULL);
            gtk_cell_renderer_render (tree_view->renderer, cr, widget,
                                      &rect, &rect,0);
            g_value_unset (&val);
          }
        valid = gtk_tree_model_iter_next (tree_view->model, &iter);
        y += tree_view->row_height;
      }
  }

  /* Chain up */
  GTK_WIDGET_CLASS (lazy_tree_view_parent_class)->draw (widget, cr);

  return FALSE;
}

static void
lazy_tree_view_init (LazyTreeView *treeview)
{
  printf("%s\n",__FUNCTION__);

  GtkWidget *button = gtk_button_new_with_label ("Move red rectangle with "
                                                 "upper left corner");
  gtk_layout_put ( GTK_LAYOUT (treeview), button, 200, 300);

  /* Red Rectangle */
  treeview->red_x = 80;
  treeview->red_y = 120;
  treeview->red_width = 80;
  treeview->red_height = 120;
  treeview->red_move = FALSE;

  /* Tree Model */
  treeview->model = NULL;

  /* Render Data */
  treeview->col_width = 200;
  treeview->row_height = 50;

  treeview->renderer = gtk_cell_renderer_text_new ();

  treeview->gesture = gtk_gesture_drag_new (GTK_WIDGET (treeview));
  gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (treeview->gesture),
                                              GTK_PHASE_CAPTURE);
  g_signal_connect (treeview->gesture, "drag-begin",
                    G_CALLBACK (drag_begin_cb), treeview);
  g_signal_connect (treeview->gesture, "drag-update",
                    G_CALLBACK (drag_update_cb), treeview);
  g_signal_connect (treeview->gesture, "drag-end",
                    G_CALLBACK (drag_end_cb), treeview);
}

static void
lazy_tree_view_class_init (LazyTreeViewClass *class)
{
  GObjectClass *o_class;
  GtkWidgetClass *widget_class;

  o_class = (GObjectClass *) class;
  widget_class = (GtkWidgetClass*) class;

  /* GObject signals */
  //o_class->set_property = lazy_tree_view_set_property;
  //o_class->get_property = lazy_tree_view_get_property;

  /* widget */
  //widget_class->map = lazy_tree_view_map;
  //widget_class->size_allocate = lazy_tree_view_size_allocate;
  widget_class->draw = lazy_tree_view_draw;
  //widget_class->realize = lazy_tree_view_realize;
  //widget_class->get_preferred_width = lazy_tree_view_get_preferred_width;
  //widget_class->get_preferred_height = lazy_tree_view_get_preferred_height;

  printf("%s\n",__FUNCTION__);
}

GtkWidget *
lazy_tree_view_new (void)
{
  printf("%s\n",__FUNCTION__);
  return g_object_new (TYPE_LAZY_TREE_VIEW, NULL);
}


static void
estimate_new_size (LazyTreeView *tree_view)
{
  gint n_cols = gtk_tree_model_get_n_columns(tree_view->model);
  gint n_rows = gtk_tree_model_iter_n_children (tree_view->model, NULL);

  gtk_layout_set_size (GTK_LAYOUT (tree_view),
                       n_cols  * tree_view->col_width,
                       n_rows  * tree_view->row_height);
}

void lazy_tree_view_set_model (LazyTreeView *tree_view,
                               GtkTreeModel *model)
{
  if (tree_view->model == model)
    return;
  tree_view->model = model;
  estimate_new_size (tree_view);
}


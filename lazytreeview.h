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

#ifndef __LAZY_TREE_VIEW_H
#define __LAZY_TREE_VIEW_H

#include <gtk/gtk.h>

#define TYPE_LAZY_TREE_VIEW            (lazy_tree_view_get_type ())
#define LAZY_TREE_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_LAZY_TREE_VIEW, LazyTreeView))
#define LAZY_TREE_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_LAZY_TREE_VIEW, LazyTreeViewClass))
#define IS_LAZY_TREE_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_LAZY_TREE_VIEW))
#define IS_LAZY_TREE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_LAZY_TREE_VIEW_CLASS))
#define LAZY_TREE_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_LAZY_TREE_VIEW, LazyTreeViewClass))

typedef struct _LazyTreeView         LazyTreeView;
typedef struct _LazyTreeViewClass    LazyTreeViewClass;

GType                   lazy_tree_view_get_type     (void);
GtkWidget              *lazy_tree_view_new          (void);

#endif /* __LAZY_TREE_VIEW_H */

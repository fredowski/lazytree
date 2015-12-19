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

#ifndef __LAZY_STORE_H__
#define __LAZY_STORE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TYPE_LAZY_STORE                 (lazy_store_get_type ())
#define LAZY_STORE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_LAZY_STORE, LazyStore))
#define LAZY_STORE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_LAZY_STORE, LazyStoreClass))
#define IS_LAZY_STORE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_LAZY_STORE))
#define IS_LAZY_STORE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_LAZY_STORE))
#define LAZY_STORE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_LAZY_STORE, LazyStoreClass))

typedef struct _LazyStore              LazyStore;
typedef struct _LazyStoreClass         LazyStoreClass;

struct _LazyStoreClass
{
  GObjectClass parent_class;

};

GType         lazy_store_get_type         (void) G_GNUC_CONST;

LazyStore    *lazy_store_new              (void);

G_END_DECLS


#endif /* __LAZY_STORE_H__ */

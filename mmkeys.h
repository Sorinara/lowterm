/*
 * Copyright (C) 2004 Lee Willis <lee@leewillis.co.uk>
 *    Borrowed heavily from code by Jan Arne Petersen <jpetersen@uni-bonn.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <X11/Xlib.h>
#include <X11/XF86keysym.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <stdio.h>
#include <gtk/gtktogglebutton.h>

#ifndef __KEYS_H
#define __KEYS_H

#define TYPE_KEYS            (key_get_type ())
#define KEYS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_KEYS, BindKey))
#define KEYS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_KEYS, BindKeyClass))
#define IS_KEYS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_KEYS))
#define IS_KEYS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_KEYS))
#define KEYS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_KEYS, BindKeyClass))

typedef struct _BindKey      BindKey;
typedef struct _BindKeyClass BindKeyClass;

/* arguments add! */
struct _BindKey
{
	GObject parent;
    int keycode;
    guint signal;
};

struct _BindKeyClass
{
	GObjectClass parent_class;
};

GType   key_get_type (void);

BindKey *key_new      (char *keysym);

#endif /* __KEYS_H */

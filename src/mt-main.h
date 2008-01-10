/*
 * Copyright © 2007 Gerd Kohlberger <lowfi@chello.at>
 *
 * This file is part of Mousetweaks.
 *
 * Mousetweaks is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mousetweaks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MT_MAIN_H__
#define __MT_MAIN_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dbus/dbus-glib-lowlevel.h>
#include "mt-common.h"

G_BEGIN_DECLS

typedef enum {
    BUTTON_STYLE_TEXT = 0,
    BUTTON_STYLE_ICON,
    BUTTON_STYLE_BOTH
} ButtonStyle;

typedef struct _MTClosure MTClosure;
struct _MTClosure {
    DBusConnection *conn;
    GConfClient *client;

    GtkWidget *about;

    GTimer *delay_timer;
    guint  delay_tid;
    GTimer *dwell_timer;
    guint  dwell_tid;

    gint dwell_cct;
    gboolean dwell_drag_started;
    gboolean dwell_gesture_started;
    gboolean context_menu_opened;

    GestureDirection direction;

    gint pointer_x;
    gint pointer_y;

    /* options */
    gint threshold;
    ButtonStyle style;
    gboolean delay_enabled;
    gdouble  delay_time;
    gboolean dwell_enabled;
    gdouble  dwell_time;
    gboolean dwell_show_ctw;
    DwellMode dwell_mode;
    GestureDirection dwell_dirs[4];
};

void spi_shutdown (void);

G_END_DECLS

#endif /* __MT_MAIN_H__ */

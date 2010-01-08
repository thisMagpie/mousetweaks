/*
 * Copyright © 2007-2009 Gerd Kohlberger <lowfi@chello.at>
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

#include <dbus/dbus-glib-lowlevel.h>

#include "mt-listener.h"

#define SPI_EVENT_MOUSE_IFACE "org.freedesktop.atspi.Event.Mouse"
#define SPI_SIGNAL_BUTTON     "Button"
#define SPI_SIGNAL_ABS        "Abs"

#define SPI_EVENT_FOCUS_IFACE "org.freedesktop.atspi.Event.Focus"
#define SPI_SIGNAL_FOCUS      "Focus"

#define SPI_ACCESSIBLE_IFACE  "org.freedesktop.atspi.Accessible"

struct _MtListenerPrivate {
    DBusGConnection *connection;
    DBusGProxy      *focus;
    guint            track_focus : 1;
};

enum {
    MOTION_EVENT,
    BUTTON_EVENT,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (MtListener, mt_listener, G_TYPE_OBJECT)

static void
mt_listener_init (MtListener *listener)
{
    listener->priv = G_TYPE_INSTANCE_GET_PRIVATE (listener,
						  MT_TYPE_LISTENER,
						  MtListenerPrivate);
}

static void
mt_listener_dispose (GObject *object)
{
    MtListenerPrivate *priv = MT_LISTENER (object)->priv;

    if (priv->connection) {
	dbus_g_connection_unref (priv->connection);
	priv->connection = NULL;
    }
    if (priv->focus) {
	g_object_unref (priv->focus);
	priv->focus = NULL;
    }
    G_OBJECT_CLASS (mt_listener_parent_class)->dispose (object);
}

static void
mt_listener_class_init (MtListenerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = mt_listener_dispose;

    signals[MOTION_EVENT] =
	g_signal_new (g_intern_static_string ("motion_event"),
		      G_OBJECT_CLASS_TYPE (klass),
		      G_SIGNAL_RUN_LAST,
		      0, NULL, NULL,
		      g_cclosure_marshal_VOID__BOXED, G_TYPE_NONE,
		      1, MT_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);
    signals[BUTTON_EVENT] =
	g_signal_new (g_intern_static_string ("button_event"),
		      G_OBJECT_CLASS_TYPE (klass),
		      G_SIGNAL_RUN_LAST,
		      0, NULL, NULL,
		      g_cclosure_marshal_VOID__BOXED, G_TYPE_NONE,
		      1, MT_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_type_class_add_private (klass, sizeof (MtListenerPrivate));
}

static gboolean
mt_listener_msg_to_event (MtListener  *listener,
                          DBusMessage *msg,
                          MtEvent     *event)
{
    DBusMessageIter iter;
    dbus_uint32_t u;
    char *s;
    int type, arg = 1;

    dbus_message_iter_init (msg, &iter);
    while (dbus_message_iter_has_next (&iter))
    {
	type = dbus_message_iter_get_arg_type (&iter);
	if (type == DBUS_TYPE_STRING) {
	    dbus_message_iter_get_basic (&iter, &s);
	    if (!s)
		return FALSE;
	    if (*s == '\0') {
		event->button = 0;
		event->type = EV_MOTION;
	    }
	    else if (s[0] && s[1]) {
		event->button = (guint) g_ascii_strtod (s, NULL);
		event->type = s[1] == 'p' ? EV_BUTTON_PRESS : EV_BUTTON_RELEASE;
	    }
	    else
		return FALSE;
	}
	else if (type == DBUS_TYPE_UINT32) {
	    dbus_message_iter_get_basic (&iter, &u);
	    if (arg++ == 1)
		event->x = u;
	    else
		event->y = u;
	}
	dbus_message_iter_next (&iter);
    }
    return TRUE;
}

static DBusHandlerResult
mt_listener_dispatch (DBusConnection *bus,
		      DBusMessage    *msg,
		      gpointer        data)
{
    MtListener *listener = data;
    MtEvent ev;

    if (dbus_message_has_sender (msg, DBUS_SERVICE_DBUS))
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    if (dbus_message_has_member (msg, SPI_SIGNAL_ABS))
    {
	if (mt_listener_msg_to_event (listener, msg, &ev))
	    g_signal_emit (data, signals[MOTION_EVENT], 0, &ev);
    }
    else if (dbus_message_has_member (msg, SPI_SIGNAL_BUTTON))
    {
	if (mt_listener_msg_to_event (listener, msg, &ev))
	    g_signal_emit (data, signals[BUTTON_EVENT], 0, &ev);
    }
    else if (listener->priv->track_focus &&
	     dbus_message_has_member (msg, SPI_SIGNAL_FOCUS))
    {
	if (listener->priv->focus)
	    g_object_unref (listener->priv->focus);

	listener->priv->focus =
	    dbus_g_proxy_new_for_name (listener->priv->connection,
				       dbus_message_get_sender (msg),
				       dbus_message_get_path (msg),
				       SPI_ACCESSIBLE_IFACE);
    }
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static void
mt_listener_setup_filter (MtListener *listener)
{
    DBusConnection *bus;

    bus = dbus_g_connection_get_connection (listener->priv->connection);
    dbus_bus_add_match (bus,
			"type='signal',"
			"interface='" SPI_EVENT_MOUSE_IFACE "',"
			"member='" SPI_SIGNAL_ABS "'",
			NULL);
    dbus_bus_add_match (bus,
			"type='signal',"
			"interface='" SPI_EVENT_MOUSE_IFACE "',"
			"member='" SPI_SIGNAL_BUTTON "'",
			NULL);
    dbus_bus_add_match (bus,
			"type='signal',"
			"interface='" SPI_EVENT_FOCUS_IFACE "'",
			NULL);
    dbus_connection_add_filter (bus, mt_listener_dispatch, listener, NULL);
}

MtListener *
mt_listener_new (DBusGConnection *connection)
{
    MtListener *listener;

    g_return_val_if_fail (connection != NULL, NULL);

    listener = g_object_new (MT_TYPE_LISTENER, NULL);
    listener->priv->connection = dbus_g_connection_ref (connection);
    mt_listener_setup_filter (listener);

    return listener;
}

DBusGProxy *
mt_listener_current_focus (MtListener *listener)
{
    g_return_val_if_fail (MT_IS_LISTENER (listener), NULL);

    if (listener->priv->track_focus && listener->priv->focus)
	return g_object_ref (listener->priv->focus);
    else
	return NULL;
}

void
mt_listener_track_focus (MtListener *listener,
                         gboolean    track)
{
    MtListenerPrivate *priv;

    g_return_if_fail (MT_IS_LISTENER (listener));

    priv = listener->priv;
    priv->track_focus = track;
    if (!track && priv->focus) {
	g_object_unref (priv->focus);
	priv->focus = NULL;
    }
}

GType
mt_event_get_type (void)
{
    static GType event = 0;

    if (G_UNLIKELY (event == 0))
	event = g_boxed_type_register_static (g_intern_static_string ("MtEvent"),
					      (GBoxedCopyFunc) mt_event_copy,
					      (GBoxedFreeFunc) mt_event_free);
    return event;
}

MtEvent *
mt_event_copy (const MtEvent *event)
{
    return g_memdup (event, sizeof (MtEvent));
}

void
mt_event_free (MtEvent *event)
{
    g_free (event);
}

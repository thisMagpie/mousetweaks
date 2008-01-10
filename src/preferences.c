/*
 * Copyright © 2007 Gerd Kohlberger <lowfi@chello.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <glade/glade.h>

#include "mt-common.h"

static GladeXML    *xml    = NULL;
static GConfClient *client = NULL;

static void
update_mode_sensitivity (gint mode)
{
    GtkWidget *w;

    w = glade_xml_get_widget (xml, "box_ctw");
    gtk_widget_set_sensitive (w, mode);
    w = glade_xml_get_widget (xml, "box_gesture");
    gtk_widget_set_sensitive (w, !mode);
}

static gboolean
verfiy_setting (gint value, gint type)
{
    gint i, c[N_CLICK_TYPES];

    c[DWELL_CLICK_TYPE_SINGLE] =
	gconf_client_get_int (client, OPT_G_SINGLE, NULL);
    c[DWELL_CLICK_TYPE_DOUBLE] =
	gconf_client_get_int (client, OPT_G_DOUBLE, NULL);
    c[DWELL_CLICK_TYPE_DRAG] =
	gconf_client_get_int (client, OPT_G_DRAG, NULL);
    c[DWELL_CLICK_TYPE_RIGHT] =
	gconf_client_get_int (client, OPT_G_RIGHT, NULL);

    for (i = 0; i < N_CLICK_TYPES; i++) {
	if (i == type)
	    continue;
	if (c[i] == value)
	    return FALSE;
    }

    return TRUE;
}

void
threshold_changed (GtkRange *range, gpointer data)
{
    gconf_client_set_int (client, OPT_THRESHOLD,
			  gtk_range_get_value (range), NULL);
}

void
delay_enable (GtkToggleButton *button, gpointer data)
{
    gboolean active, dwell;
    gint ret;

    active = gtk_toggle_button_get_active (button);
    dwell = gconf_client_get_bool (client, OPT_DWELL, NULL);

    if (active && dwell) {
	ret = mt_show_dialog (_("Enable Delay Click"),
			      _("Dwell Click is currently active and will "
			      "be disabled."),
			       GTK_MESSAGE_QUESTION);

	if (ret == GTK_RESPONSE_NO || ret == GTK_RESPONSE_DELETE_EVENT) {
	    gtk_toggle_button_set_active (button, FALSE);
	    return;
	}
	else {
	    GtkWidget *w;

	    w = glade_xml_get_widget (xml, "dwell_enable");
	    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), FALSE);
	}
    }

    gconf_client_set_bool (client, OPT_DELAY, active, NULL);
}

void
delay_time (GtkRange *range, gpointer data)
{
    gconf_client_set_float (client, OPT_DELAY_T,
			    gtk_range_get_value (range), NULL);
}

void
dwell_enable (GtkToggleButton *button, gpointer data)
{
    gboolean active, delay;
    gint ret;

    active = gtk_toggle_button_get_active (button);
    delay = gconf_client_get_bool (client, OPT_DELAY, NULL);

    if (active && delay) {
	ret = mt_show_dialog (_("Enable Dwell Click"),
			      _("Delay Click is currently active and will "
			      "be disabled."),
			      GTK_MESSAGE_QUESTION);

	if (ret == GTK_RESPONSE_NO || ret == GTK_RESPONSE_DELETE_EVENT) {
	    gtk_toggle_button_set_active (button, FALSE);
	    return;
	}
	else {
	    GtkWidget *w;

	    w = glade_xml_get_widget (xml, "delay_enable");
	    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), FALSE);
	}
    }

    gconf_client_set_bool (client, OPT_DWELL, active, NULL);
}

void
dwell_time (GtkRange *range, gpointer data)
{
    gconf_client_set_float (client, OPT_DWELL_T,
			    gtk_range_get_value (range), NULL);
}

void
show_ctw (GtkToggleButton *button, gpointer data)
{
    gconf_client_set_bool (client, OPT_CTW,
			   gtk_toggle_button_get_active (button), NULL);
}

void
mode_changed (GtkToggleButton *button, gpointer data)
{
    GSList *group;
    gint mode;

    if (!gtk_toggle_button_get_active (button))
	return;

    group = gtk_radio_button_get_group (GTK_RADIO_BUTTON(button));
    mode = g_slist_index (group, (gconstpointer) button);
    gconf_client_set_int (client, OPT_MODE, mode, NULL);

    update_mode_sensitivity (mode);
}

void
gesture_single (GtkComboBox *combo, gpointer data)
{
    gint active;

    active = gtk_combo_box_get_active (combo);

    if (!verfiy_setting (active, DWELL_CLICK_TYPE_SINGLE))
	gtk_combo_box_set_active (combo, DIRECTION_DISABLE);
    else
	gconf_client_set_int (client, OPT_G_SINGLE, active, NULL);
}

void
gesture_double (GtkComboBox *combo, gpointer data)
{
    gint active;

    active = gtk_combo_box_get_active (combo);

    if (!verfiy_setting (active, DWELL_CLICK_TYPE_DOUBLE))
	gtk_combo_box_set_active (combo, DIRECTION_DISABLE);
    else
	gconf_client_set_int (client, OPT_G_DOUBLE, active, NULL);
}

void
gesture_drag (GtkComboBox *combo, gpointer data)
{
    gint active;

    active = gtk_combo_box_get_active (combo);

    if (!verfiy_setting (active, DWELL_CLICK_TYPE_DRAG))
	gtk_combo_box_set_active (combo, DIRECTION_DISABLE);
    else
	gconf_client_set_int (client, OPT_G_DRAG, active, NULL);
}

void
gesture_right (GtkComboBox *combo, gpointer data)
{
    gint active;

    active = gtk_combo_box_get_active (combo);

    if (!verfiy_setting (active, DWELL_CLICK_TYPE_RIGHT))
	gtk_combo_box_set_active (combo, DIRECTION_DISABLE);
    else
	gconf_client_set_int (client, OPT_G_RIGHT, active, NULL);
}

int
main (int argc, char **argv)
{
    GtkWidget *w;
    gint mode;

    gtk_init (&argc, &argv);
    gtk_window_set_default_icon_name (MT_ICON_NAME);

    xml = glade_xml_new (GLADE_PATH "/mousetweaks.glade", NULL, NULL);
    if (!xml) {
	mt_show_dialog (_("Internal Error"),
			_("Couldn't load glade interface file."),
			GTK_MESSAGE_ERROR);
	return 1;
    }

    client = gconf_client_get_default ();

    /* general tab */
    w = glade_xml_get_widget (xml, "threshold");
    gtk_range_set_value (GTK_RANGE(w),
			 gconf_client_get_int (client, OPT_THRESHOLD, NULL));

    /* delay tab */
    w = glade_xml_get_widget (xml, "delay_enable");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w),
				  gconf_client_get_bool (client,
							 OPT_DELAY, NULL));
    w = glade_xml_get_widget (xml, "delay_time");
    gtk_range_set_value (GTK_RANGE(w),
			 gconf_client_get_float (client, OPT_DELAY_T, NULL));

    /* dwell tab */
    w = glade_xml_get_widget (xml, "dwell_enable");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w),
				  gconf_client_get_bool (client,
							 OPT_DWELL, NULL));
    w = glade_xml_get_widget (xml, "dwell_time");
    gtk_range_set_value (GTK_RANGE(w),
			 gconf_client_get_float (client, OPT_DWELL_T, NULL));
    w = glade_xml_get_widget (xml, "dwell_show_ctw");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w),
				  gconf_client_get_bool (client,
							 OPT_CTW, NULL));

    mode = gconf_client_get_int (client, OPT_MODE, NULL);
    w = glade_xml_get_widget (xml, "dwell_mode_ctw");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), mode);
    w = glade_xml_get_widget (xml, "dwell_mode_gesture");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), !mode);
    update_mode_sensitivity (mode);

    w = glade_xml_get_widget (xml, "dwell_gest_single");
    gtk_combo_box_set_active (GTK_COMBO_BOX(w),
			      gconf_client_get_int (client,
						    OPT_G_SINGLE, NULL));
    w = glade_xml_get_widget (xml, "dwell_gest_double");
    gtk_combo_box_set_active (GTK_COMBO_BOX(w),
			      gconf_client_get_int (client,
						    OPT_G_DOUBLE, NULL));
    w = glade_xml_get_widget (xml, "dwell_gest_drag");
    gtk_combo_box_set_active (GTK_COMBO_BOX(w),
			      gconf_client_get_int (client,
						    OPT_G_DRAG, NULL));
    w = glade_xml_get_widget (xml, "dwell_gest_right");
    gtk_combo_box_set_active (GTK_COMBO_BOX(w),
			      gconf_client_get_int (client,
						    OPT_G_RIGHT, NULL));

    glade_xml_signal_autoconnect (xml);
    gtk_main ();

    g_object_unref (client);

    return 0;
}

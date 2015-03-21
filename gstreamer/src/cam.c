/*
 * cam.c - XOverlay using GStreamer 0.10.36
 * Copyright (c) 2014 Mandar Joshi (emailmandar at gmail)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
 

/* gcc -o cam cam.c `pkg-config --cflags --libs gstreamer-interfaces-0.10 gtk+-2.0 gstreamer-0.10` */
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <gdk/gdkx.h>

typedef struct _CustomData {
	GstElement *playbin2;	
	GtkWidget *cam_window;
	GtkWidget *still_image_window;
	GtkWidget *message_box;
	GtkWidget *filename_entry;
}CustomData;

guintptr cam_window_handle;

static GstBusSyncReply bus_sync_handler (GstBus * bus, GstMessage * message, gpointer data)
{
	// ignore anything but 'prepare-xwindow-id' element messages
	if (GST_MESSAGE_TYPE (message) != GST_MESSAGE_ELEMENT)
		return GST_BUS_PASS;
	if (!gst_structure_has_name (message->structure, "prepare-xwindow-id"))
		return GST_BUS_PASS;

	if (cam_window_handle != 0) {
		GstXOverlay *xoverlay;
		// GST_MESSAGE_SRC (message) will be the video sink element
		xoverlay = GST_X_OVERLAY (GST_MESSAGE_SRC (message));
		gst_x_overlay_set_window_handle (xoverlay, cam_window_handle);
	} else {
		g_warning ("Should have obtained video_window_xid by now!");
	}

	gst_message_unref (message);
	return GST_BUS_DROP;
}

static void realize_cb (GtkWidget *widget, CustomData *data) {
	GdkWindow *window = gtk_widget_get_window (widget);
	
	g_print ("In cam_window realize_cb\n");
	
	if (!gdk_window_ensure_native (window))
		g_error ("Couldn't create native window needed for GstXOverlay!");
	
	cam_window_handle = GDK_WINDOW_XID (window);	

	//gst_x_overlay_set_window_handle (GST_X_OVERLAY (data->playbin2), cam_window_handle);	
}

static void start_cb (GtkButton *button, CustomData *data) {
	g_print ("Starting Capture\n");
	gst_element_set_state (data->playbin2, GST_STATE_PLAYING);
}

static void stop_cb (GtkButton *button, CustomData *data) {
	g_print ("Stopping Capture\n");
	gst_element_set_state (data->playbin2, GST_STATE_PAUSED);
}

static void take_snap_cb (GtkButton *button, CustomData *data) {
	g_print ("Taking Snap\n");
	GdkPixbuf *pb;
	pb = gdk_pixbuf_get_from_drawable(NULL, GDK_DRAWABLE(data->cam_window->window), NULL, 0, 0, 0, 0, 640, 480);
	gdk_draw_pixbuf(data->still_image_window->window, NULL, pb, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);

}

static void save_snap_cb (GtkButton *button, CustomData *data) {
	const gchar *filename = gtk_entry_get_text (GTK_ENTRY (data->filename_entry));
	g_print ("Saving Snap to %s\n", filename);
	GdkPixbuf *pb;
	pb = gdk_pixbuf_get_from_drawable(NULL, GDK_DRAWABLE(data->still_image_window->window), NULL, 0, 0, 0, 0, 640, 480);
	gdk_pixbuf_save(pb, filename, "png", NULL, "compression", "9", NULL);

}

static void delete_event_cb (GtkWidget *widget, GdkEvent *event, CustomData *data) {
	stop_cb(NULL, data);
	gtk_main_quit();
}

static void create_ui (CustomData *data) {
	GtkWidget *main_window;
	GtkWidget *button_vbox;
	GtkWidget *main_hbox;
	GtkWidget *main_vbox;
	GtkWidget *start_button, *take_snap_button, *stop_button, *save_snap_button;
	
	main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (G_OBJECT (main_window), "delete-event", G_CALLBACK (delete_event_cb), data);
	
	data->cam_window = gtk_drawing_area_new ();
	gtk_widget_set_usize (data->cam_window, 640, 480);
	gtk_widget_set_double_buffered (data->cam_window, FALSE);
	//gtk_widget_realize (data->cam_window);
	g_signal_connect (data->cam_window, "realize", G_CALLBACK (realize_cb), data);
	
	
	data->still_image_window = gtk_drawing_area_new ();
	gtk_widget_set_usize (data->still_image_window, 640, 480);
	
	start_button = gtk_button_new_with_label ("Start Capture");
	g_signal_connect (G_OBJECT (start_button), "clicked", G_CALLBACK (start_cb), data);
		
	take_snap_button = gtk_button_new_with_label ("Take Snap");
	g_signal_connect (G_OBJECT (take_snap_button), "clicked", G_CALLBACK (take_snap_cb), data);
	
	stop_button = gtk_button_new_with_label ("Stop Capture");
	g_signal_connect (G_OBJECT (stop_button), "clicked", G_CALLBACK (stop_cb), data);
	
	data->filename_entry = gtk_entry_new();
	gtk_entry_set_text (GTK_ENTRY (data->filename_entry), "snap.png");
		
	save_snap_button = gtk_button_new_with_label ("Save Snap");
	g_signal_connect (G_OBJECT (save_snap_button), "clicked", G_CALLBACK (save_snap_cb), data);
	
	button_vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (button_vbox), start_button, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (button_vbox), take_snap_button, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (button_vbox), stop_button, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (button_vbox), data->filename_entry, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (button_vbox), save_snap_button, FALSE, TRUE, 0);
	
	main_hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (main_hbox), data->cam_window, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (main_hbox), button_vbox, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (main_hbox), data->still_image_window, FALSE, FALSE, 0);
	
	
	data->message_box = gtk_text_view_new ();
	gtk_text_view_set_editable (GTK_TEXT_VIEW (data->message_box), FALSE);
	gtk_widget_set_usize (data->message_box, 1024, 200);
	
	main_vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (main_vbox), main_hbox, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (main_vbox), data->message_box, TRUE, TRUE, 0);
	
	
	gtk_container_add (GTK_CONTAINER (main_window), main_vbox);
	/*gtk_window_set_default_size (GTK_WINDOW (main_window), 1024, 768);*/
	
	gtk_widget_show_all (main_window);	
	gtk_widget_realize (data->cam_window);
}

int main (int argc, char *argv[]) {
	CustomData data;
	GstBus *bus;
	
	gst_init (&argc, &argv);
	gtk_init (&argc, &argv);	

	data.playbin2 = gst_parse_launch ("v4l2src ! video/x-raw-yuv,width=640,height=480 ! xvimagesink", NULL);
	/*data.playbin2 = gst_element_factory_make ("playbin2", "playbin2");
	g_object_set (data.playbin2, "uri", "file:///disks/sda7/youtube/sintel_trailer-480p.webm", NULL);	*/
	create_ui(&data);

	bus = gst_pipeline_get_bus (GST_PIPELINE (data.playbin2));
  	gst_bus_set_sync_handler (bus, (GstBusSyncHandler) bus_sync_handler, NULL);
  	gst_object_unref (bus);
	//gst_element_set_state (data.playbin2, GST_STATE_PLAYING);


	gtk_main();
		
	gst_object_unref (data.playbin2);
	return 0;
}


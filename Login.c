#include "header.h"

gboolean clear_status_label(gpointer user_data)
{
	if (!user_data)
	{
		g_printerr("clear_status_label: user_data is NULL\n");
		return G_SOURCE_REMOVE;
	}
	GtkWidget** widget = (GtkWidget**)user_data;
	if (!widget[0] || !widget[1] || !widget[2])
	{
		g_printerr("clear_status_label: Invalid widgets array\n");
		return G_SOURCE_REMOVE;
	}
	gtk_entry_set_text(GTK_ENTRY(widget[0]), "");
	gtk_entry_set_text(GTK_ENTRY(widget[1]), "");
	gtk_label_set_text(GTK_LABEL(widget[2]), "");
	return G_SOURCE_REMOVE;
}

// Credential verification function
gboolean verify_credentials(const gchar* username, const gchar* password)
{
	GKeyFile* keyfile = g_key_file_new();
	GError* error = NULL;

	if (!g_key_file_load_from_file(keyfile, "./_resourse/credentials.ini", G_KEY_FILE_NONE, &error))
	{
		g_printerr("Error loading credentials: %s\n", error->message);
		g_error_free(error);
		return FALSE;
	}

	// This checks BOTH username existence AND password match
	gchar* stored_pass = g_key_file_get_string(keyfile, "Users", username, &error);

	gboolean result = (stored_pass && g_strcmp0(password, stored_pass) == 0);

	g_free(stored_pass);
	g_key_file_free(keyfile);
	return result;
}

gboolean check_login(GtkWidget* entry_user, GtkWidget* entry_pass, GtkWidget* status_label)
{
	printf("going to check login\n");

	if (!entry_user || !entry_pass || !status_label)
	{
		g_printerr("check_login: Invalid widget pointers\n");
		return FALSE;
	}

	const gchar* username = gtk_entry_get_text(GTK_ENTRY(entry_user));
	const gchar* password = gtk_entry_get_text(GTK_ENTRY(entry_pass));

	GtkStyleContext* context = gtk_widget_get_style_context(status_label);

	if (g_strcmp0(username, "") == 0 || g_strcmp0(password, "") == 0)
	{
		gtk_label_set_text(GTK_LABEL(status_label), "Please fill all fields.");
		gtk_style_context_remove_class(context, "success-label");
		gtk_style_context_add_class(context, "error-label");
		return FALSE;
	}
	if (verify_credentials(username, password))
	{
		gtk_label_set_text(GTK_LABEL(status_label), "Login Successful!");
		gtk_style_context_remove_class(context, "error-label");
		gtk_style_context_add_class(context, "success-label");
		return TRUE;
	}
	gtk_label_set_text(GTK_LABEL(status_label), "Invalid username or password");
	gtk_style_context_remove_class(context, "success-label");
	gtk_style_context_add_class(context, "error-label");
	return FALSE;
}

static void on_login_clicked(GtkButton* button, gpointer user_data)
{
	printf("Login Button Clicked\n");
	if (!user_data)
	{
		g_printerr("on_login_clicked: user_data is NULL\n");
		return;
	}
	gpointer* widgets = (gpointer*)user_data;
	if (!widgets[0] || !widgets[1] || !widgets[2])
	{
		g_printerr("on_login_clicked: Invalid widgets array\n");
		return;
	}
	GtkWidget* entry_user = widgets[0];
	GtkWidget* entry_pass = widgets[1];
	GtkWidget* status_label = widgets[2];

	const gchar* username = gtk_entry_get_text(GTK_ENTRY(entry_user));
	const gchar* password = gtk_entry_get_text(GTK_ENTRY(entry_pass));
	gboolean success = check_login(entry_user, entry_pass, status_label);
	printf("going to store status\n");


	g_print("Username: %s\n", username);
	g_print("Password: %s\n", password);
	if (success)
	{
		g_timeout_add(500, redirect_to_homepage, button);
	}
	else
	{
		g_timeout_add(500, clear_status_label, widgets);
	}
}

static void on_login_destroy(GtkWidget* widget, gpointer data)
{
	g_free(data);
}

GtkWidget* login(void)
{
	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

	GtkWidget* grid = gtk_grid_new();
	gtk_widget_set_name(box, "login-box");
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

	GtkWidget* title_label = gtk_label_new("Vector India - Login");
	gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
	gtk_style_context_add_class(gtk_widget_get_style_context(title_label), "title");
	gtk_box_pack_start(GTK_BOX(box), title_label, FALSE, FALSE, 0);

	GtkWidget* username_label = gtk_label_new("Username:");
	GtkWidget* password_label = gtk_label_new("Password:");
	gtk_widget_set_margin_top(grid, 20);
	gtk_widget_set_margin_bottom(grid, 20);
	gtk_widget_set_margin_start(grid, 20);
	gtk_widget_set_margin_end(grid, 20);

	GtkWidget* username_entry = gtk_entry_new();
	GtkWidget* password_entry = gtk_entry_new();/*
	gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Enter username");
	gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter password");*/
	gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
	gtk_entry_set_invisible_char(GTK_ENTRY(password_entry), '*');

	GtkWidget* login_button = gtk_button_new_with_label("Login");
	gtk_widget_set_name(login_button, "login-button");
	GtkWidget* status_label = gtk_label_new("");

	gpointer* widgets = g_new(gpointer, 3);
	widgets[0] = username_entry;
	widgets[1] = password_entry;
	widgets[2] = status_label;

	gtk_grid_attach(GTK_GRID(grid), username_label, 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), username_entry, 1, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), password_label, 0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), password_entry, 1, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), login_button, 0, 4, 2, 1);
	// gtk_grid_attach(GTK_GRID(grid), status_label, 0, 5, 2, 1);
	gtk_box_pack_end(GTK_BOX(box), status_label, FALSE, FALSE, 10);

	g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_clicked), widgets);
	g_signal_connect(box, "destroy", G_CALLBACK(on_login_destroy), widgets);
	gtk_container_add(GTK_CONTAINER(box), grid);

	return box;
}
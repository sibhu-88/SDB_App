#include "header.h"

SDB *list = NULL;
GtkWidget* create_header_bar()
{
	GtkWidget* header = gtk_header_bar_new();

	GtkWidget* logo_image = gtk_image_new_from_file("./_resourse/logo.png");
	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), logo_image);

	const char* button_icons[] = {
		"document-save-symbolic",
		"view-sort-ascending-symbolic",
		"document-edit-symbolic",
		"user-trash-symbolic",
		"view-list-symbolic",
		"list-add-symbolic" };

	const char* button_labels[] = { "Save", "Sort", "Modify", "Delete", "List", "Add" };
	GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	for (int i = 0; i < 6; i++)
	{
		GtkWidget* btn = gtk_button_new_from_icon_name(button_icons[i], GTK_ICON_SIZE_BUTTON);

		gtk_widget_set_tooltip_text(btn, button_labels[i]);
		g_signal_connect(btn, "clicked", G_CALLBACK(on_menu_button_clicked), (gpointer)button_labels[i]);

		gtk_box_pack_end(GTK_BOX(button_box), btn, FALSE, FALSE, 0);
	}

	GtkWidget* profile_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_set_margin_start(profile_box, 15);

	GtkWidget* profile_btn = gtk_button_new_from_icon_name("avatar-default-symbolic", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_tooltip_text(profile_btn, "User Profile");
	gtk_box_pack_end(GTK_BOX(profile_box), profile_btn, FALSE, FALSE, 0);

	g_signal_connect(profile_btn, "clicked", G_CALLBACK(on_profile_clicked), NULL);

	gtk_header_bar_pack_end(GTK_HEADER_BAR(header), profile_box);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(header), button_box);

	return header;
}

void on_menu_button_clicked(GtkButton* button, gpointer user_data)
{
	const char* button_label = (const char*)user_data;
	g_print("Clicked: %s\n", button_label);

	if (strcmp(button_label, "Add") == 0)
	{
		// Handle Add action
		AddRecord(button, &list);
	}
	else if (strcmp(button_label, "List") == 0)
	{
		// Handle List action
		SortRecordRollno(&list);
		ListRecord(button, list);
	}
	else if (strcmp(button_label, "Delete") == 0)
	{
		// Handle delete action
		DeleteRecord(button, &list);
	}
	else if (strcmp(button_label, "Modify") == 0)
	{
		// Handle Modify action
		ModifyRecord(button, &list);
	}
	else if (strcmp(button_label, "Sort") == 0) {
		g_print("Sort button clicked!\n");

		if (list == NULL) {
			GtkWidget* dialog = gtk_dialog_new_with_buttons("No Records",
				GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
				GTK_DIALOG_MODAL, "_OK", GTK_RESPONSE_OK, NULL);
			GtkWidget* message_label = gtk_label_new("No student records available.");
			gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), message_label, TRUE, TRUE, 0);
			gtk_widget_show_all(dialog);
			g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
			return;
		}

		GtkWidget* menu = gtk_menu_new();



		GtkWidget* menu_item1 = gtk_menu_item_new_with_label("Sort by Name");
		GtkWidget* menu_item2 = gtk_menu_item_new_with_label("Sort by Mark");


		g_object_set_data(G_OBJECT(menu_item1), "button", button);
		g_object_set_data(G_OBJECT(menu_item2), "button", button);

		// Pass button and list to callbacks
		SortData* sort_data = g_new(SortData, 1);
		sort_data->button = button;
		sort_data->list = &list; // Pointer to the list pointer

		g_signal_connect(menu_item1, "activate", G_CALLBACK(sort_by_name), sort_data);
		g_signal_connect(menu_item2, "activate", G_CALLBACK(sort_by_mark), sort_data);

		// Free sort_data when menu is destroyed
		g_signal_connect(menu, "selection-done", G_CALLBACK(gtk_widget_destroy), NULL);
		g_signal_connect_swapped(menu, "selection-done", G_CALLBACK(g_free), sort_data);

		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item1);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item2);

		gtk_widget_show_all(menu);

		gtk_menu_popup_at_widget(
			GTK_MENU(menu),
			GTK_WIDGET(button),
			GDK_GRAVITY_SOUTH,
			GDK_GRAVITY_NORTH,
			NULL);
	}
	else if (strcmp(button_label, "Save") == 0)
	{
		// Handle Save action
		SaveRecord(button, list);
	}
}

// Sorting callbacks
void sort_by_name(GtkMenuItem* item, gpointer data)
{
	g_print("Sort By Name selected\n");
	// SDB **list = (SDB **)data;
	SortRecordName(&list);
	GtkWidget* button = g_object_get_data(G_OBJECT(item), "button");
	if (button)
	{
		g_print("Calling SortRecord for name\n");
		SortRecord(GTK_BUTTON(button), &list);
	}
	else
	{
		g_printerr("Error: No button data in sort_by_name\n");
	}
}

void sort_by_mark(GtkMenuItem* item, gpointer data)
{
	g_print("Sort By Mark selected\n");
	// SDB **list = (SDB **)data;
	SortRecordMark(&list);
	GtkWidget* button = g_object_get_data(G_OBJECT(item), "button");
	if (button)
	{
		g_print("Calling SortRecord for mark\n");
		SortRecord(GTK_BUTTON(button), &list);
	}
	else
	{
		g_printerr("Error: No button data in sort_by_mark\n");
	}
}

// Profile Click Handler (New Function)
void on_profile_clicked(GtkButton* button, gpointer user_data)
{
	g_print("Profile button clicked!\n");

	// Create profile menu
	GtkWidget* menu = gtk_menu_new();

	GtkWidget* menu_item1 = gtk_menu_item_new_with_label("Edit Profile");
	GtkWidget* menu_item2 = gtk_menu_item_new_with_label("Change Password");
	GtkWidget* menu_item3 = gtk_menu_item_new_with_label("Logout");

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item2);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item3);

	// Connect menu actions
	g_signal_connect(menu_item1, "activate", G_CALLBACK(edit_profile), NULL);
	g_signal_connect(menu_item2, "activate", G_CALLBACK(change_password), NULL);
	g_signal_connect(menu_item3, "activate", G_CALLBACK(logout_user), NULL);

	gtk_widget_show_all(menu);

	// Show menu at button position
	gtk_menu_popup_at_widget(
		GTK_MENU(menu),
		GTK_WIDGET(button),
		GDK_GRAVITY_SOUTH_WEST,
		GDK_GRAVITY_NORTH_WEST,
		NULL);
}

// Add these stubs for menu actions
void edit_profile(GtkMenuItem* item, gpointer data)
{
	g_print("Edit Profile selected\n");
}

void change_password(GtkMenuItem* item, gpointer data)
{
	g_print("Change Password selected\n");
}

void logout_user(GtkMenuItem* item, gpointer user_data)
{
	g_print("Logout selected\n");

	// Free the student list to prevent memory leaks
	SDB* current = list;
	while (current)
	{
		SDB* next = current->next;
		free(current);
		current = next;
	}
	list = NULL;

	// Use the global main_window
	extern GtkWidget* window;
	if (!window || !GTK_IS_WINDOW(window))
	{
		g_printerr("Error: main_window is not a valid window\n");
		return;
	}

	// Remove all children from the window
	GList* children = gtk_container_get_children(GTK_CONTAINER(window));
	for (GList* iter = children; iter; iter = g_list_next(iter))
	{
		if (GTK_IS_WIDGET(iter->data))
		{
			gtk_widget_destroy(GTK_WIDGET(iter->data));
		}
		else
		{
			g_printerr("Warning: Skipping invalid widget in window children\n");
		}
	}
	g_list_free(children);

	// Load the login screen
	GtkWidget* login_window = login();
	gtk_container_add(GTK_CONTAINER(window), login_window);
	gtk_widget_show_all(window);
}

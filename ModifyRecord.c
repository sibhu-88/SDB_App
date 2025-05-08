#include "header.h"

void ModifyRecord(GtkButton *button, SDB **node)
{
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    if (*node == NULL)
    {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("No Records",
                                                        GTK_WINDOW(window),
                                                        GTK_DIALOG_MODAL,
                                                        "_OK", GTK_RESPONSE_OK,
                                                        NULL);
        GtkWidget *message_label = gtk_label_new("No student records available.");
        gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                           message_label, TRUE, TRUE, 0);
        gtk_widget_show_all(dialog);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    GList *children = gtk_container_get_children(GTK_CONTAINER(window));
    for (GList *iter = children; iter; iter = g_list_next(iter))
    {
        GtkWidget *child = GTK_WIDGET(iter->data);
        gtk_container_remove(GTK_CONTAINER(window), child);
        gtk_widget_destroy(child); // Ensure widget is destroyed
    }
    g_list_free(children);

    GtkWidget *modify_record = ModifyRecordTable(node);
    gtk_container_add(GTK_CONTAINER(window), modify_record);
    gtk_widget_show_all(window);
}

SDB *check_data(GtkButton *button, SDB *node, int rollno)
{
    SDB *found = NULL;
    while (node)
    {
        if (node->rollno == rollno)
        {
            found = node;
            break;
        }
        node = node->next;
    }

    if (!found)
    {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("No Records",
                                                        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                                                        GTK_DIALOG_MODAL,
                                                        "_OK", GTK_RESPONSE_OK,
                                                        NULL);
        GtkWidget *message_label = gtk_label_new("No records found.");
        GtkWidget *ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
        gtk_style_context_add_class(gtk_widget_get_style_context(ok_button), "ok-button");
        gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                           message_label, TRUE, TRUE, 0);
        gtk_widget_show_all(dialog);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    return found;
}

void on_name_change_clicked(GtkButton *button, gpointer user_data)
{
    gpointer *widgets = (gpointer *)user_data;
    GtkEntry *rollno_entry = GTK_ENTRY(widgets[0]);
    SDB **node = (SDB **)widgets[1];

    const char *rollno_text = gtk_entry_get_text(GTK_ENTRY(rollno_entry));
    if (!rollno_text || !*rollno_text)
    {
        g_print("Error: Roll number is empty\n");
        return;
    }
    char *endptr;
    long rollno = strtol(rollno_text, &endptr, 10);
    if (*endptr != '\0' || rollno < 0 || rollno > INT_MAX)
    {
        g_print("Error: Invalid roll number\n");
        return;
    }

    SDB *temp = check_data(button, *node, rollno);
    if (temp)
    {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Change Name",
                                                        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                                                        GTK_DIALOG_MODAL,
                                                        "_OK", GTK_RESPONSE_OK,
                                                        "_Cancel", GTK_RESPONSE_CANCEL,
                                                        NULL);
        GtkWidget *entry = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(entry), temp->name);
        GtkWidget *ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
        GtkWidget *cancel_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
        gtk_style_context_add_class(gtk_widget_get_style_context(ok_button), "yes-button");
        gtk_style_context_add_class(gtk_widget_get_style_context(cancel_button), "no-button");
        gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                           entry, TRUE, TRUE, 0);
        gtk_widget_show_all(dialog);

        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        // Disconnect signal handler to prevent further clicks
        g_signal_handlers_disconnect_by_func(button, G_CALLBACK(on_name_change_clicked), widgets);
        if (response == GTK_RESPONSE_OK)
        {
            const char *new_name = gtk_entry_get_text(GTK_ENTRY(entry));
            strncpy(temp->name, new_name, SDB_NAME_MAX - 1);
            temp->name[SDB_NAME_MAX - 1] = '\0';
            g_print("Updated name for rollno %d to %s\n", temp->rollno, temp->name);
            gtk_entry_set_text(rollno_entry, "");
        }
        gtk_widget_destroy(dialog);
        // g_timeout_add(500, redirect_to_homepage, button);
    }
}

void on_mark_change_clicked(GtkButton *button, gpointer user_data)
{
    gpointer *widgets = (gpointer *)user_data;
    GtkEntry *rollno_entry = GTK_ENTRY(widgets[0]);
    SDB **node = (SDB **)widgets[1];

    const char *rollno_text = gtk_entry_get_text(GTK_ENTRY(rollno_entry));
    if (!rollno_text || !*rollno_text)
    {
        g_print("Error: Roll number is empty\n");
        return;
    }
    char *endptr;
    long rollno = strtol(rollno_text, &endptr, 10);
    if (*endptr != '\0' || rollno < 0 || rollno > INT_MAX)
    {
        g_print("Error: Invalid roll number\n");
        return;
    }

    SDB *temp = check_data(button, *node, rollno);
    if (temp)
    {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Change Mark",
                                                        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                                                        GTK_DIALOG_MODAL,
                                                        "_OK", GTK_RESPONSE_OK,
                                                        "_Cancel", GTK_RESPONSE_CANCEL,
                                                        NULL);
        GtkWidget *entry = gtk_entry_new();
        char mark_str[16];
        snprintf(mark_str, sizeof(mark_str), "%d", temp->mark);
        gtk_entry_set_text(GTK_ENTRY(entry), mark_str);
        GtkWidget *ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
        GtkWidget *cancel_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
        gtk_style_context_add_class(gtk_widget_get_style_context(ok_button), "yes-button");
        gtk_style_context_add_class(gtk_widget_get_style_context(cancel_button), "no-button");
        gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                           entry, TRUE, TRUE, 0);
        gtk_widget_show_all(dialog);

        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        // Disconnect signal handler to prevent further clicks
        g_signal_handlers_disconnect_by_func(button, G_CALLBACK(on_mark_change_clicked), widgets);
        if (response == GTK_RESPONSE_OK)
        {
            const char *mark_text = gtk_entry_get_text(GTK_ENTRY(entry));
            long new_mark = strtol(mark_text, &endptr, 10);
            if (*endptr != '\0' || new_mark < 0 || new_mark > 100)
            {
                g_print("Error: Invalid mark (must be 0-100)\n");
            }
            else
            {
                temp->mark = new_mark;
                g_print("Updated mark for rollno %d to %d\n", temp->rollno, temp->mark);
                g_timeout_add(500, redirect_to_homepage, button);
            }
            gtk_entry_set_text(rollno_entry, "");
        }
        gtk_widget_destroy(dialog);
        g_timeout_add(500, redirect_to_homepage, button);
    }
}

void on_widgets_destroy(gpointer user_data)
{
    WidgetData *data = (WidgetData *)user_data;
    g_free(data);
}

GtkWidget *ModifyRecordTable(SDB **node)
{
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *header = create_header_bar();
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);

    GtkWidget *info_label = gtk_label_new("Make sure your roll number is on the list page");
    gtk_widget_set_halign(info_label, GTK_ALIGN_CENTER);
    gtk_style_context_add_class(gtk_widget_get_style_context(info_label), "info");
    gtk_box_pack_start(GTK_BOX(box), info_label, FALSE, FALSE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_name(box, "Modify Record");
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    GtkWidget *rollno_label = gtk_label_new("Roll No:");
    GtkWidget *rollno_entry = gtk_entry_new();
    GtkWidget *name_button = gtk_button_new_with_label("Name change");
    GtkWidget *mark_button = gtk_button_new_with_label("Mark change");

    gtk_style_context_add_class(gtk_widget_get_style_context(name_button), "modify-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(mark_button), "modify-button");

    gpointer *widgets = g_new(gpointer, 2);
    widgets[0] = rollno_entry;
    widgets[1] = node;

    // Store data on box to ensure cleanup
    g_object_set_data(G_OBJECT(box), "widget-data", widgets);

    gtk_grid_attach(GTK_GRID(grid), rollno_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rollno_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_button, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mark_button, 1, 1, 1, 1);

    g_signal_connect(name_button, "clicked", G_CALLBACK(on_name_change_clicked), widgets);
    g_signal_connect(mark_button, "clicked", G_CALLBACK(on_mark_change_clicked), widgets);
    // g_signal_connect(box, "destroy", G_CALLBACK(on_widgets_destroy), widgets);
    gtk_container_add(GTK_CONTAINER(box), grid);

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    gtk_box_set_spacing(GTK_BOX(button_box), 0);

    GtkWidget *back_button = gtk_button_new_with_label("Back");
    gtk_style_context_add_class(gtk_widget_get_style_context(back_button), "back-button");
    gtk_box_pack_start(GTK_BOX(button_box), back_button, FALSE, FALSE, 0);

    g_signal_connect(back_button, "clicked", G_CALLBACK(homepage), NULL);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), box, FALSE, FALSE, 10);
    return main_box;
}
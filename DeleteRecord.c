#include "header.h"

void DeleteRecord(GtkButton *button, SDB **node)
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
        GtkWidget *ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
        gtk_style_context_add_class(gtk_widget_get_style_context(ok_button), "ok-button");
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

    GtkWidget *delete_record = DeleteRecordTable(node);
    gtk_container_add(GTK_CONTAINER(window), delete_record);
    gtk_widget_show_all(window);
}

GtkWidget *DeleteRecordTable(SDB **node)
{
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *header = create_header_bar(node);
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
    gtk_widget_set_name(box, "Delete Record");
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    GtkWidget *rollno_label = gtk_label_new("Roll No:");
    GtkWidget *rollno_entry = gtk_entry_new();
    GtkWidget *status_label = gtk_label_new("");
    GtkWidget *delete_button = gtk_button_new_with_label("Delete");
    gtk_style_context_add_class(gtk_widget_get_style_context(delete_button), "delete-button");

    WidgetData *data = g_new(WidgetData, 1);
    data->rollno_entry = rollno_entry;
    data->status_label = status_label;
    data->node = node;

    gtk_grid_attach(GTK_GRID(grid), rollno_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rollno_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), delete_button, 0, 1, 2, 1);
    gtk_box_pack_end(GTK_BOX(box), status_label, FALSE, FALSE, 10);


    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_clicked), data);
    g_object_set_data_full(G_OBJECT(box), "widget-data", data, g_free);

    gtk_container_add(GTK_CONTAINER(box), grid);
    gtk_box_pack_start(GTK_BOX(main_box), box, FALSE, FALSE, 10);
    return main_box;
}

void on_delete_clicked(GtkButton *button, gpointer user_data)
{
    WidgetData *data = (WidgetData *)user_data;
    GtkWidget *rollno_entry = data->rollno_entry;
    GtkWidget *status_label = data->status_label;
    SDB **node = data->node;
    GtkStyleContext *context = gtk_widget_get_style_context(status_label);

    const char *rollno_text = gtk_entry_get_text(GTK_ENTRY(rollno_entry));
    if (!rollno_text || !*rollno_text)
    {
        gtk_label_set_text(GTK_LABEL(status_label), "Roll number is empty");
        gtk_style_context_add_class(context, "error-label");
        g_print("Error: Roll number is empty\n");
        return;
    }
    char *endptr;
    long rollno = strtol(rollno_text, &endptr, 10);
    if (*endptr != '\0' || rollno < 0 || rollno > INT_MAX)
    {
        gtk_label_set_text(GTK_LABEL(status_label), "Invalid roll number");
        gtk_style_context_add_class(context, "error-label");
        g_print("Error: Invalid roll number\n");
        return;
    }

    SDB *temp = check_data(button, *node, rollno);
    if (temp)
    {
        GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Confirm Deletion",
                                                        GTK_WINDOW(window),
                                                        GTK_DIALOG_MODAL,
                                                        "_OK", GTK_RESPONSE_OK,
                                                        "_Cancel", GTK_RESPONSE_CANCEL,
                                                        NULL);
        GtkWidget *ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
        GtkWidget *cancel_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
        gtk_style_context_add_class(gtk_widget_get_style_context(ok_button), "yes-button");
        gtk_style_context_add_class(gtk_widget_get_style_context(cancel_button), "no-button");

        char message[100];
        snprintf(message, sizeof(message), "Delete record for roll no %ld?", rollno);
        GtkWidget *message_label = gtk_label_new(message);
        gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                           message_label, TRUE, TRUE, 0);
        gtk_widget_show_all(dialog);

        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        if (response == GTK_RESPONSE_OK)
        {
            if (delete_record(node, rollno))
            {
                g_print("Deleted record for rollno %ld\n", rollno);
                g_timeout_add(500, redirect_to_homepage, button);
            }
            else
            {
                dialog = gtk_dialog_new_with_buttons("Error",
                                                     GTK_WINDOW(window),
                                                     GTK_DIALOG_MODAL,
                                                     "_OK", GTK_RESPONSE_OK,
                                                     NULL);
                message_label = gtk_label_new("Failed to delete record.");
                gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                                   message_label, TRUE, TRUE, 0);
                gtk_widget_show_all(dialog);
                gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);
            }
            
        }
    }
}

int delete_record(SDB **ptr, int rollno)
{
    SDB *current = *ptr;
    SDB *previous = NULL;

    while (current != NULL && current->rollno != rollno)
    {
        previous = current;
        current = current->next;
    }

    if (current != NULL)
    {
        if (previous == NULL)
            *ptr = current->next;
        else
            previous->next = current->next;

        free(current);
        return 1; // Success
    }
    return 0; // Not found
}
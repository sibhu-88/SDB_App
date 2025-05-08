#include "header.h"

gboolean dialog_box(gpointer user_data)
{
    GtkButton *button = GTK_BUTTON(user_data);
    if (!button || !GTK_IS_BUTTON(button)) {
        fprintf(stderr, "Error: Invalid or null button in dialog_box\n");
        return FALSE;
    }
    g_print("dialog_box: button=%p\n", (void *)button);

    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (!GTK_IS_WINDOW(toplevel)) {
        fprintf(stderr, "Error: Could not get toplevel window for dialog\n");
        return FALSE;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Save Records",
        GTK_WINDOW(toplevel),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_OK",
        GTK_RESPONSE_OK,
        NULL
    );
    gtk_widget_set_name(dialog, "save-dialog");

    // Style OK button
    GtkWidget *ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    if (ok_button && GTK_IS_BUTTON(ok_button)) {
        gtk_style_context_add_class(gtk_widget_get_style_context(ok_button), "yes-button");
        g_print("Applied yes-button class to OK button: %p\n", (void *)ok_button);
    } else {
        g_print("Warning: Could not get OK button\n");
    }

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new("Records saved successfully!");
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);

    g_print("Showing dialog: save records\n");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    g_timeout_add(500, redirect_to_homepage, button);
    g_print("Dialog closed\n");
    return FALSE;
}

void SaveRecord(GtkButton *button, SDB *list)
{
    if (list == NULL)
    {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("No Records",
                                                        GTK_WINDOW(button),
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

    g_print("SaveRecord: button=%p, list=%p\n", (void *)button, (void *)list);

    FILE *file = fopen("student.xls", "w");
    if (file == NULL) {
        perror("Unable to open file");
        return;
    }

    fprintf(file, "Roll.No\tName\tPercentage\n");

    SDB *current = list;
    while (current != NULL) {
        fprintf(file, "%d\t%s\t%d\n", current->rollno, current->name, current->mark);
        g_print("Saved record: rollno=%d, name=%s, mark=%d\n",
                current->rollno, current->name, current->mark);
        current = current->next;
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Error: Could not close student.csv\n");
    }

    g_timeout_add(500, dialog_box, button);
}
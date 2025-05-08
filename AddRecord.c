#include "header.h"

void AddRecord(GtkButton *button, SDB **node)
{
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    GList *children = gtk_container_get_children(GTK_CONTAINER(window));
    for (GList *iter = children; iter; iter = g_list_next(iter))
    {
        GtkWidget *child = GTK_WIDGET(iter->data);
        gtk_container_remove(GTK_CONTAINER(window), child);
        gtk_widget_destroy(child); // Ensure widget is destroyed
    }
    g_list_free(children);

    GtkWidget *addrecord = AddRecordTable(node);

    gtk_container_add(GTK_CONTAINER(window), addrecord);
    gtk_widget_show_all(window);
}

void AddNewRecord(SDB **list, const char *name, int mark)
{
    SDB *newRecord = (SDB *)malloc(sizeof(SDB));

    strcpy(newRecord->name, name);
    newRecord->mark = mark;

    if (*list == NULL)
    {
        newRecord->rollno = 1;
        newRecord->next = NULL;
        *list = newRecord;
    }
    else
    {
        SDB *last = *list;
        while (last->next != NULL)
        {
            last = last->next;
        }

        newRecord->rollno = last->rollno + 1;
        newRecord->next = NULL;
        last->next = newRecord;
    }
    g_print("Studen Record %s %d\n", newRecord->name, newRecord->mark);
}

GtkWidget *AddRecordTable(SDB **node)
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

    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_name(box, "Add Record");
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    GtkWidget *title_label = gtk_label_new("Vector India - Add Record");
    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_style_context_add_class(gtk_widget_get_style_context(title_label), "title");
    gtk_box_pack_start(GTK_BOX(box), title_label, FALSE, FALSE, 0);

    GtkWidget *name_label = gtk_label_new("Name:");
    GtkWidget *mark_label = gtk_label_new("Mark:");
    GtkWidget *name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Enter name");
    GtkWidget *mark_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(mark_entry), "Enter mark (0-100)");

    GtkWidget *submit_button = gtk_button_new_with_label("Submit");
    gtk_widget_set_name(submit_button, "submit-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(submit_button), "submit-button");

    GtkWidget *status_label = gtk_label_new("");
    gtk_label_set_line_wrap(GTK_LABEL(status_label), TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(status_label), ".default-label");

    gpointer *widgets = g_new(gpointer, 4);
    widgets[0] = name_entry;
    widgets[1] = mark_entry;
    widgets[2] = status_label;
    widgets[3] = node;

    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mark_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mark_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), submit_button, 0, 3, 2, 1);
    // gtk_grid_attach(GTK_GRID(grid), status_label, 0, 4, 2, 1);
    gtk_box_pack_end(GTK_BOX(box), status_label, FALSE, FALSE, 10);

    g_signal_connect(submit_button, "clicked", G_CALLBACK(on_submit_clicked), widgets);
    g_signal_connect(box, "destroy", G_CALLBACK(on_destroy), widgets);

    gtk_container_add(GTK_CONTAINER(box), grid);
    gtk_box_pack_start(GTK_BOX(main_box), box, FALSE, FALSE, 10);
    return main_box;
}

void on_submit_clicked(GtkButton *button, gpointer user_data)
{
    gpointer *widgets = (gpointer *)user_data;
    GtkEntry *name_entry = GTK_ENTRY(widgets[0]);
    GtkEntry *mark_entry = GTK_ENTRY(widgets[1]);
    GtkWidget *status_label = GTK_WIDGET(widgets[2]);
    SDB **node = (SDB **)widgets[3];

    const char *name = gtk_entry_get_text(name_entry);
    const char *mark_str = gtk_entry_get_text(mark_entry);

    if (strlen(name) == 0 || strlen(mark_str) == 0)
    {
        gtk_style_context_remove_class(gtk_widget_get_style_context(status_label), "default-label");
        gtk_style_context_remove_class(gtk_widget_get_style_context(status_label), "success-label");
        gtk_style_context_add_class(gtk_widget_get_style_context(status_label), "error-label");
        gtk_label_set_text(GTK_LABEL(status_label), "Name and Mark cannot be empty!");
        return;
    }

    char *endptr;
    long mark = strtol(mark_str, &endptr, 10);
    if (*endptr != '\0' || mark < 0 || mark > 100)
    {

        gtk_style_context_remove_class(gtk_widget_get_style_context(status_label), "default-label");
        gtk_style_context_remove_class(gtk_widget_get_style_context(status_label), "success-label");
        gtk_style_context_add_class(gtk_widget_get_style_context(status_label), "error-label");
        gtk_label_set_text(GTK_LABEL(status_label), "Mark must be a number between 0 and 100!");
        return;
    }

    AddNewRecord(node, name, mark);

    gtk_style_context_remove_class(gtk_widget_get_style_context(status_label), "default-label");
    gtk_style_context_remove_class(gtk_widget_get_style_context(status_label), "error-label");
    gtk_style_context_add_class(gtk_widget_get_style_context(status_label), "success-label");
    gtk_label_set_text(GTK_LABEL(status_label), "Record added successfully!");
    gtk_entry_set_text(name_entry, "");
    gtk_entry_set_text(mark_entry, "");

    //g_timeout_add(800, clear_status_label, status_label);


    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Add Another Record",
        GTK_WINDOW(toplevel),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Yes",
        GTK_RESPONSE_YES,
        "_No",
        GTK_RESPONSE_NO,
        NULL);
    gtk_widget_set_name(dialog, "add-another-dialog");
    GtkWidget *yes_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_YES);
    GtkWidget *no_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_NO);
    gtk_style_context_add_class(gtk_widget_get_style_context(yes_button), "yes-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(no_button), "no-button");

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new("Would you like to add another record?");
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);

    g_print("Showing dialog: add another record\n");
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    g_print("Dialog response: %d\n", response);

    gtk_widget_destroy(dialog);

    if (response == GTK_RESPONSE_YES)
    {
        gtk_style_context_remove_class(gtk_widget_get_style_context(status_label), "success-label");
        gtk_style_context_remove_class(gtk_widget_get_style_context(status_label), "error-label");
        gtk_style_context_add_class(gtk_widget_get_style_context(status_label), "default-label");
        gtk_label_set_text(GTK_LABEL(status_label), "");
        gtk_entry_set_text(name_entry, "");
        gtk_entry_set_text(mark_entry, "");
        g_print("User chose to add another record\n");
    }
    else
    {
        g_print("Scheduling redirect to homepage\n");
        g_timeout_add(500, redirect_to_homepage, button);
    }
}

void on_destroy(GtkWidget *widget, gpointer user_data)
{
    g_free(user_data);
}

void read_records_from_file(SDB **list)
{
    FILE *file = fopen("student.xls", "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Unable to open the file 'student.xls'.\n");
        *list = NULL;
        return;
    }

    char line[256];

    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file))
    {
        SDB *temp = (SDB *)malloc(sizeof(SDB));
        if (temp == NULL)
        {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            fclose(file);
            return;
        }

        char name[50];
        sscanf(line, "%d\t%[^\t]\t%d", &temp->rollno, name, &temp->mark);
        strcpy(temp->name, name);
        temp->next = NULL;

        if (*list == NULL)
            *list = temp;
        else
        {
            SDB *last = *list;
            while (last->next != NULL)
                last = last->next;
            last->next = temp;
        }
    }

    fclose(file);
}
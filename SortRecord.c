#include "header.h"

void SortRecord(GtkButton *button, SDB **list)
{
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    if (list == NULL || *list == NULL)
    {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("No Records",
                                                        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(window))),
                                                        GTK_DIALOG_MODAL,
                                                        "_OK",
                                                        GTK_RESPONSE_OK,
                                                        NULL);

        GtkWidget *message_label = gtk_label_new("No student records available.");
        GtkWidget *ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
        gtk_style_context_add_class(gtk_widget_get_style_context(ok_button), "ok-button");
        gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), message_label, TRUE, TRUE, 0);

        gtk_widget_show_all(dialog);

        g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
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

    GtkWidget *listrecord = ListRecordTable(*list);

    gtk_container_add(GTK_CONTAINER(window), listrecord);
    gtk_widget_show_all(window);
}
/*
GtkWidget *SortRecordDesk(SDB *node)
{
    g_print("ListRecordTable: node=%p\n", (void *)node);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *list_box = ListRecordTable(node);
    //GtkWidget *header = create_header_bar();
    //gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL , 0);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    gtk_box_set_spacing(GTK_BOX(button_box), 0);

    GtkWidget *name_button = gtk_button_new_with_label("Back");
    //gtk_style_context_add_class(gtk_widget_get_style_context(name_button), "back-button");
    gtk_box_pack_start(GTK_BOX(button_box), name_button, FALSE, FALSE, 0);

    GtkWidget *mark_button = gtk_button_new_with_label("Back");
    //gtk_style_context_add_class(gtk_widget_get_style_context(mark_button), "back-button");
    gtk_box_pack_start(GTK_BOX(button_box), mark_button, FALSE, FALSE, 0);

    g_signal_connect(name_button, "clicked", G_CALLBACK(sort_by_name), NULL);
    g_signal_connect(mark_button, "clicked", G_CALLBACK(sort_by_mark), NULL);
    
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), list_box, FALSE, FALSE, 0);
    return main_box;
}

// Sorting callbacks
void sort_by_name(GtkMenuItem *item, gpointer data)
{
    g_print("Sort By Name selected\n");
    // SDB **list = (SDB **)data;
    // SortRecordName(&list);
    // GtkWidget *button = g_object_get_data(G_OBJECT(item), "button");
    // if (button)
    // {
    //     g_print("Calling SortRecord for name\n");
    //     SortRecord(GTK_BUTTON(button), &list);
    // }
    // else
    // {
    //     g_printerr("Error: No button data in sort_by_name\n");
    // }
}

void sort_by_mark(GtkMenuItem *item, gpointer data)
{
    g_print("Sort By Mark selected\n");
    // SDB **list = (SDB **)data;
    // SortRecordMark(&list);
    // GtkWidget *button = g_object_get_data(G_OBJECT(item), "button");
    // if (button)
    // {
    //     g_print("Calling SortRecord for mark\n");
    //     SortRecord(GTK_BUTTON(button), &list);
    // }
    // else
    // {
    //     g_printerr("Error: No button data in sort_by_mark\n");
    // }
}
*/
// Count nodes in the list
int count(SDB *head)
{
    int c = 0;
    while (head) {
        c++;
        head = head->next;
    }
    return c;
}

// Sort by name (A-Z)
void SortRecordName(SDB **ptr)
{
    int i, j, c = count(*ptr);
    SDB **p, *temp;

    // Allocate array of pointers
    p = (SDB **)malloc(c * sizeof(SDB *));
    if (!p) return;

    // Populate array
    temp = *ptr;
    for (i = 0; i < c; i++) {
       // if (!temp->name) temp->name = ""; // Prevent strcmp crashes
        p[i] = temp;
        temp = temp->next;
    }

    // Bubble sort array by name (ascending)
    for (i = 0; i < c - 1; i++) {
        for (j = 0; j < c - i - 1; j++) {
            if (strcmp(p[j]->name, p[j + 1]->name) > 0) {
                SDB *t = p[j];
                p[j] = p[j + 1];
                p[j + 1] = t;
            }
        }
    }

    // Re-link nodes in ascending order
    if (c > 0) {
        p[c - 1]->next = NULL; // Last node
        for (i = c - 2; i >= 0; i--) {
            p[i]->next = p[i + 1];
        }
        *ptr = p[0]; // New head is first element
    }

    free(p);
}

// Sort by mark (low to high)
void SortRecordMark(SDB **ptr)
{
    int i, j, c = count(*ptr);
    SDB **p, *temp;

    p = (SDB **)malloc(c * sizeof(SDB *));
    if (!p) return;

    temp = *ptr;
    for (i = 0; i < c; i++) {
        p[i] = temp;
        temp = temp->next;
    }

    for (i = 0; i < c - 1; i++) {
        for (j = 0; j < c - i - 1; j++) {
            if (p[j]->mark < p[j + 1]->mark) {
                SDB *t = p[j];
                p[j] = p[j + 1];
                p[j + 1] = t;
            }
        }
    }

    if (c > 0) {
        p[c - 1]->next = NULL;
        for (i = c - 2; i >= 0; i--) {
            p[i]->next = p[i + 1];
        }
        *ptr = p[0];
    }

    free(p);
}

// Sort by roll number (low to high)
void SortRecordRollno(SDB **ptr)
{
    int i, j, c = count(*ptr);
    SDB **p, *temp;

    p = (SDB **)malloc(c * sizeof(SDB *));
    if (!p) return;

    temp = *ptr;
    for (i = 0; i < c; i++) {
        p[i] = temp;
        temp = temp->next;
    }

    for (i = 0; i < c - 1; i++) {
        for (j = 0; j < c - i - 1; j++) {
            if (p[j]->rollno > p[j + 1]->rollno) {
                SDB *t = p[j];
                p[j] = p[j + 1];
                p[j + 1] = t;
            }
        }
    }

    if (c > 0) {
        p[c - 1]->next = NULL;
        for (i = c - 2; i >= 0; i--) {
            p[i]->next = p[i + 1];
        }
        *ptr = p[0];
    }

    free(p);
}
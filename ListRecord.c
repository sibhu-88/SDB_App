#include "header.h"

void ListRecord(GtkButton* button, SDB* list) {
	GtkWidget* window = gtk_widget_get_toplevel(GTK_WIDGET(button));
	if (!gtk_widget_is_toplevel(window)) {
		g_warning("Cannot show records: not a top-level window");
		return;
	}

	// Stop any blinking timeout
	guint timeout_id = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(window), "blink_timeout_id"));
	if (timeout_id) {
		g_source_remove(timeout_id);
		g_object_set_data(G_OBJECT(window), "blink_timeout_id", NULL);
	}

	if (list == NULL) {
		GtkWidget* dialog = gtk_dialog_new_with_buttons("No Records", GTK_WINDOW(window),
			GTK_DIALOG_MODAL, "_OK", GTK_RESPONSE_OK, NULL);
		GtkWidget* message_label = gtk_label_new("No student records available.");
		GtkWidget* ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
		gtk_style_context_add_class(gtk_widget_get_style_context(ok_button), "ok-button");
		gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), message_label, TRUE, TRUE, 0);
		gtk_widget_show_all(dialog);
		g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
		return;
	}

	// Clear existing children
	GList* children = gtk_container_get_children(GTK_CONTAINER(window));
	for (GList* iter = children; iter; iter = g_list_next(iter)) {
		gtk_container_remove(GTK_CONTAINER(window), GTK_WIDGET(iter->data));
	}
	g_list_free(children);

	GtkWidget* listrecord = ListRecordTable(list);
	gtk_container_add(GTK_CONTAINER(window), listrecord);
	gtk_widget_show_all(window);
}

static GtkWidget* create_grid_table(SDB* node) {
	GtkWidget* grid = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
	gtk_style_context_add_class(gtk_widget_get_style_context(grid), "record-table");

	// Headers
	GtkWidget* header_rollno = gtk_label_new(NULL);
	GtkWidget* header_name = gtk_label_new(NULL);
	GtkWidget* header_mark = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(header_rollno), "<b>" COL_ROLLNO "</b>");
	gtk_label_set_markup(GTK_LABEL(header_name), "<b>" COL_NAME "</b>");
	gtk_label_set_markup(GTK_LABEL(header_mark), "<b>" COL_MARK "</b>");
	gtk_widget_set_margin_start(header_rollno, 5);
	gtk_widget_set_margin_end(header_rollno, 5);
	gtk_widget_set_margin_start(header_name, 5);
	gtk_widget_set_margin_end(header_name, 5);
	gtk_widget_set_margin_start(header_mark, 5);
	gtk_widget_set_margin_end(header_mark, 5);
	gtk_grid_attach(GTK_GRID(grid), header_rollno, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), header_name, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), header_mark, 2, 0, 1, 1);

	// Data
	SDB* current = node;
	int row = 1;
	while (current != NULL) {
		char rollno_str[10];
		char mark_str[10];
		snprintf(rollno_str, sizeof(rollno_str), "%d", current->rollno);
		snprintf(mark_str, sizeof(mark_str), "%d", current->mark);

		GtkWidget* rollno_label = gtk_label_new(rollno_str);
		GtkWidget* name_label = gtk_label_new(current->name);
		GtkWidget* mark_label = gtk_label_new(mark_str);

		gtk_label_set_xalign(GTK_LABEL(rollno_label), 0.0);
		gtk_label_set_xalign(GTK_LABEL(name_label), 0.0);
		gtk_label_set_xalign(GTK_LABEL(mark_label), 0.0);
		gtk_widget_set_margin_start(rollno_label, 5);
		gtk_widget_set_margin_end(rollno_label, 5);
		gtk_widget_set_margin_start(name_label, 5);
		gtk_widget_set_margin_end(name_label, 5);
		gtk_widget_set_margin_start(mark_label, 5);
		gtk_widget_set_margin_end(mark_label, 5);

		gtk_grid_attach(GTK_GRID(grid), rollno_label, 0, row, 1, 1);
		gtk_grid_attach(GTK_GRID(grid), name_label, 1, row, 1, 1);
		gtk_grid_attach(GTK_GRID(grid), mark_label, 2, row, 1, 1);

		current = current->next;
		row++;
	}

	return grid;
}

static GtkWidget* create_treeview_table(SDB* node) {
	GtkWidget* tree_view = gtk_tree_view_new();
	gtk_style_context_add_class(gtk_widget_get_style_context(tree_view), "record-table");
	gtk_widget_set_name(tree_view, "student-list");

	// List store
	GtkListStore* store = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT);
	GtkTreeIter iter;
	SDB* current = node;
	while (current) {
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, 0, current->rollno, 1, current->name, 2, current->mark, -1);
		g_print("Added record: rollno=%d, name=%s, mark=%d\n", current->rollno, current->name, current->mark);
		current = current->next;
	}

	// Columns
	GtkCellRenderer* renderer;
	GtkTreeViewColumn* column;

	renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD, "xalign", 0.0, "xpad", 5, "ypad", 5, NULL);
	column = gtk_tree_view_column_new_with_attributes(COL_ROLLNO, renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gtk_tree_view_column_set_min_width(column, 100);

	renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD, "xalign", 0.0, "xpad", 5, "ypad", 5, NULL);
	column = gtk_tree_view_column_new_with_attributes(COL_NAME, renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_column_set_sort_column_id(column, 1);
	gtk_tree_view_column_set_min_width(column, 200);

	renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD, "xalign", 0.0, "xpad", 5, "ypad", 5, NULL);
	column = gtk_tree_view_column_new_with_attributes(COL_MARK, renderer, "text", 2, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_column_set_sort_column_id(column, 2);
	gtk_tree_view_column_set_min_width(column, 100);

	gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));
	g_object_unref(store);

	return tree_view;
}

GtkWidget* ListRecordTable(SDB* node) {
	g_print("ListRecordTable: node=%p\n", (void*)node);

	// Main vertical box
	GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	/*gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
	gtk_widget_set_margin_start(main_box, 20);
	gtk_widget_set_margin_end(main_box, 20);
	gtk_widget_set_margin_top(main_box, 20);
	gtk_widget_set_margin_bottom(main_box, 20);*/

	// Header bar
	GtkWidget* header = create_header_bar();
	if (!GTK_IS_WIDGET(header)) {
		g_warning("Failed to create header bar");
		header = gtk_label_new("Header unavailable");
	}
	gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);

	// Title label
	GtkWidget* title_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(title_label), "<b>" PROJECT_TITLE "</b>");
	gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
	gtk_style_context_add_class(gtk_widget_get_style_context(title_label), "title-label");
	gtk_box_pack_start(GTK_BOX(main_box), title_label, FALSE, FALSE, 10);

	// Inner vertical box
	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

	// Table (Grid or TreeView)
	GtkWidget* table = USE_TREEVIEW ? create_treeview_table(node) : create_grid_table(node);
	gtk_box_pack_start(GTK_BOX(box), table, TRUE, TRUE, 0);


	//// Scrolled window
	//GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	//gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	//gtk_scrolled_window_set_min_content_height(scrolled_window, 300); // Ensure visibility
	//gtk_container_add(GTK_CONTAINER(scrolled_window), table);
	//gtk_box_pack_start(GTK_BOX(box), scrolled_window, TRUE, TRUE, 0);

	// Button box
	GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
	//gtk_widget_set_margin_top(button_box, 10);

	GtkWidget* back_button = gtk_button_new_with_label("Back");
	gtk_style_context_add_class(gtk_widget_get_style_context(back_button), "nav-button");
	gtk_box_pack_start(GTK_BOX(button_box), back_button, FALSE, FALSE, 5);

	g_signal_connect(back_button, "clicked", G_CALLBACK(homepage), back_button);
	//g_signal_connect(sort_button, "clicked", G_CALLBACK(on_button_clicked), node);

	gtk_box_pack_start(GTK_BOX(main_box), box, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);

	return main_box;
}
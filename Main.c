#include "header.h"
GtkWidget* window = NULL;
void apply_css_from_file(const gchar* file_path) {
    GtkCssProvider* provider = gtk_css_provider_new();
    if (!provider) {
        g_printerr("Failed to create CSS provider\n");
        return;
    }
    GError* error = NULL;
    if (!gtk_css_provider_load_from_path(provider, file_path, &error)) {
        g_printerr("Failed to load CSS: %s\n", error->message);
        g_error_free(error);
    }
    else {
        GdkScreen* screen = gdk_screen_get_default();
        if (screen) {
            gtk_style_context_add_provider_for_screen(
                screen,
                GTK_STYLE_PROVIDER(provider),
                GTK_STYLE_PROVIDER_PRIORITY_USER);
        }
        else {
            g_printerr("Failed to get default screen\n");
        }
    }
    g_object_unref(provider);
}

void on_activate(GtkApplication* app, gpointer user_data) {
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Student Database");
    GdkScreen* screen = gdk_screen_get_default();
    int width = gdk_screen_get_width(screen) - 100;
    int height = gdk_screen_get_height(screen) - 100;

    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    apply_css_from_file("style.css");

    extern SDB* list;
    read_records_from_file(&list);

    GtkWidget* login_window = login();
    gtk_container_add(GTK_CONTAINER(window), login_window);

    gtk_widget_show_all(window);
}

int main(int argc, char** argv) {
    // if (!gtk_init_check(&argc, &argv)) {
    //     g_printerr("Failed to initialize GTK\n");
    //     return 1;
    // }
    // GtkApplication *app = gtk_application_new("com.example.GTK3App", G_APPLICATION_DEFAULT_FLAGS);
    // if (!app) {
    //     g_printerr("Failed to create application\n");
    //     return 1;
    // }
    // g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    // int status = g_application_run(G_APPLICATION(app), argc, argv);
    // g_object_unref(app);
    // return status;

    GtkApplication* app = gtk_application_new("com.example.GTK3App", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;

}
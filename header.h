#ifndef HEADER_H
#define HEADER_H

#include <gtk/gtk.h>
#include <pango/pango.h>
#include <glib/gstdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct sdb
{
	int rollno;
	char name[50];
	int mark;
	struct sdb* next;
} SDB;

typedef struct {
	GtkButton* button;
	SDB** list; // Pointer to the list pointer for modification
} SortData;

typedef struct {
	GtkWidget* rollno_entry;
	GtkWidget* status_label;
	SDB** node;
} WidgetData;

#define USE_TREEVIEW 1 // Set to 1 for GtkTreeView, 0 for GtkGrid
#define PROJECT_TITLE "STUDENT DATABASE PROJECT"
#define COL_ROLLNO "Roll No"
#define COL_NAME "Name"
#define COL_MARK "Mark"
#define SDB_NAME_MAX 50 
#define BLINK_INTERVAL 500 // Blink interval in milliseconds

GtkWidget* login();
gboolean check_login(GtkWidget* entry_user, GtkWidget* entry_pass, GtkWidget* status_label);
gboolean verify_credentials(const gchar* username, const gchar* password);
gboolean clear_status_label(gpointer user_data);

void homepage(GtkButton*);
gboolean blink_label(gpointer data);
gboolean redirect_to_homepage(gpointer user_data);
GtkWidget* create_header_bar();
void on_menu_button_clicked(GtkButton* button, gpointer user_data);
void on_profile_clicked(GtkButton* button, gpointer user_data);
void edit_profile(GtkMenuItem* item, gpointer data);
void change_password(GtkMenuItem* item, gpointer data);
void logout_user(GtkMenuItem* item, gpointer data);

void AddRecord(GtkButton* button, SDB** node);
GtkWidget* AddRecordTable(SDB** node);
void AddNewRecord(SDB** ptr, const char* name, int mark);
void on_submit_clicked(GtkButton* button, gpointer user_data);
void on_destroy(GtkWidget* widget, gpointer user_data);
void read_records_from_file(SDB** list);

void ListRecord(GtkButton* button, SDB* list);
GtkWidget* ListRecordTable(SDB* node);

void ModifyRecord(GtkButton* button, SDB** node);
GtkWidget* ModifyRecordTable(SDB** node);
void on_mark_change_clicked(GtkButton* button, gpointer user_data);
void on_name_change_clicked(GtkButton* button, gpointer user_data);
SDB* check_data(GtkButton* button, SDB* node, int rollno);

void DeleteRecord(GtkButton* button, SDB** node);
GtkWidget* DeleteRecordTable(SDB** node);
void on_delete_clicked(GtkButton* button, gpointer user_data);
int delete_record(SDB** ptr, int rollno);

void SortRecord(GtkButton* button, SDB** list);
GtkWidget* SortRecordDesk(SDB* node);
void sort_by_name(GtkMenuItem* item, gpointer data);
void sort_by_mark(GtkMenuItem* item, gpointer data);
void SortRecordName(SDB** list);
void SortRecordMark(SDB** list);
void SortRecordRollno(SDB** list);
int count(SDB* head);

void SaveRecord(GtkButton* button, SDB* list);
gboolean dialog_box(gpointer user_data);
#endif
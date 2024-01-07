#include <gtk/gtk.h>

// Structure to hold user input data
typedef struct {
    GtkWidget *nameEntry;
    GtkWidget *surnameEntry;
    GtkWidget *ageEntry;
    GtkWidget *idEntry;
    GtkWidget *outputLabel;
} UserData;

static void submit_button_clicked(GtkWidget *widget, gpointer data) {
    UserData *userData = (UserData *)data;

    const char *name = gtk_entry_get_tabs(GTK_ENTRY(userData->nameEntry));
    const char *surname = gtk_entry_get_tabs(GTK_ENTRY(userData->surnameEntry));
    const char *age = gtk_entry_get_tabs(GTK_ENTRY(userData->ageEntry));
    const char *id = gtk_entry_get_tabs(GTK_ENTRY(userData->idEntry));

    // You can do something with the user input, for now, let's just display it
    gchar *output = g_strdup_printf("Name: %s\nSurname: %s\nAge: %s\nID: %s", name, surname, age, id);
    gtk_label_set_text(GTK_LABEL(userData->outputLabel), output);
    g_free(output);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *nameEntry;
    GtkWidget *surnameEntry;
    GtkWidget *ageEntry;
    GtkWidget *idEntry;
    GtkWidget *submit_button;
    GtkWidget *outputLabel;

    // Allocate memory for UserData
    UserData *userData = g_new(UserData, 1);

    // Create a new top-level window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Users");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create a grid to layout the UI elements
    grid = gtk_grid_new();
    gtk_icon_view_set_item_width(GTK_CONSTRAINT(grid), 12);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create and add UI elements to the grid
    label = gtk_label_new("Enter User Information:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);

    nameEntry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), nameEntry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Name:"), 0, 1, 1, 1);

    surnameEntry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), surnameEntry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Surname:"), 0, 2, 1, 1);

    ageEntry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), ageEntry, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Age:"), 0, 3, 1, 1);

    idEntry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), idEntry, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("ID:"), 0, 4, 1, 1);

    submit_button = gtk_button_new_with_label("Submit");
    gtk_grid_attach(GTK_GRID(grid), submit_button, 0, 5, 2, 1);

    // Create a label to display output
    outputLabel = gtk_label_new(NULL);
    gtk_grid_attach(GTK_GRID(grid), outputLabel, 0, 6, 2, 1);

    // Assign UI elements to UserData structure
    userData->nameEntry = nameEntry;
    userData->surnameEntry = surnameEntry;
    userData->ageEntry = ageEntry;
    userData->idEntry = idEntry;
    userData->outputLabel = outputLabel;

    // Connect the button click signal to the submit_button_clicked function
    g_signal_connect(submit_button, "clicked", G_CALLBACK(submit_button_clicked), (gpointer)userData);

    gtk_widget_show(window);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    // Initialize GTK
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

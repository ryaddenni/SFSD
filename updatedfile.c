#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 128

// Structure to represent an individual
struct Person {
    char *id;
    char *age;
    char *name;
    char *surname;
    char flag; // Use char for flag
    char *size; // Dynamically allocated for unknown length
    struct Person *next;
};

// Function to dynamically allocate memory for a string input
char *getUserInput(const char *prompt);

// Function to save individual details to a text file
void saveToFile(struct Person *person, FILE *file) {
    fprintf(file, "%s#%c#%s#%s#%s#", person->size, person->flag, person->id, person->age, person->name);
    // Add '%' at the end of the surname
    fprintf(file, "%s%%", person->surname);
}

// Function to finalize the person details
void finalizePerson(struct Person *person);

// Function to calculate the size string
void calculateSize(struct Person *person);

int main() {
    // Declare a pointer to the head of the linked list
    struct Person *head = NULL;

    // Check if the users file is not empty and clear it if needed
    FILE *usersFile = fopen("users.txt", "r+");
    if (usersFile != NULL) {
        fseek(usersFile, 0, SEEK_END);
        long size = ftell(usersFile);

        // Clear the file if it's not empty
        if (size > 0) {
            freopen("users.txt", "w", usersFile);
        } else {
            // Add a newline if the file is empty
            fprintf(usersFile, "\n");
        }
    } else {
        printf("Error opening users file.\n");
        exit(1);
    }

    int numClasses;

    // Prompt the user for the number of classes
    printf("Enter the number of classes: ");
    scanf("%d", &numClasses);

    // Clear the newline character from the input buffer
    while (getchar() != '\n');

    // Loop to add individuals to the linked list
    for (int i = 0; i < numClasses; ++i) {
        // Dynamically allocate memory for the new person
        struct Person *newPerson = (struct Person *)malloc(sizeof(struct Person));
        if (newPerson == NULL) {
            printf("Error: Memory allocation failed.\n");
            exit(1);
        }

        // Input custom ID, age, name, and surname
        newPerson->id = getUserInput("Enter custom ID (a number):");
        newPerson->age = getUserInput("Enter age:");
        newPerson->name = getUserInput("Enter name:");
        newPerson->surname = getUserInput("Enter surname:");
        newPerson->flag = '0';  // Initially set the flag to 0

        // Allocate memory for the size variable
        newPerson->size = (char *)malloc(BUFFER_SIZE);
        if (newPerson->size == NULL) {
            printf("Error: Memory allocation failed.\n");
            exit(1);
        }

        // Calculate and set the size string
        calculateSize(newPerson);

        // Set the flag to 1
        newPerson->flag = '1';

        // Assign the next pointer
        newPerson->next = head;

        // Update the head to point to the new person
        head = newPerson;

        // Save details to the user file
        saveToFile(newPerson, usersFile);
    }

    fclose(usersFile);

    // Display the entered information
    printf("\nDetails of individuals:\n");

    // Traverse the linked list and print the details
    struct Person *current = head;
    int i = 1;
    while (current != NULL) {
        printf("\nIndividual %d:\n", i++);
        printf("Name: %s\n", current->name);
        printf("Surname: %s\n", current->surname);
        printf("Age: %s\n", current->age);
        printf("Custom ID: %s\n", current->id);
        printf("Size: %s\n", current->size);

        // Move to the next person in the list
        struct Person *temp = current;
        current = current->next;

        // Free allocated memory for this person
        free(temp->id);
        free(temp->surname);
        free(temp->age);
        free(temp->name);
        free(temp->size);  // Free dynamically allocated size
        free(temp);
    }

    return 0;
}

// Function to dynamically allocate memory for a string input
char *getUserInput(const char *prompt) {
    printf("%s", prompt);

    char *buffer = NULL;
    size_t bufferSize = 0;

    // Read the input using getline
    if (getline(&buffer, &bufferSize, stdin) == -1) {
        printf("Error reading input.\n");
        exit(1);
    }

    // Remove newline character if present
    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        // Clear the input buffer if necessary
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    return buffer;
}

// Function to calculate the size string
void calculateSize(struct Person *person) {
    // Calculate the size of the data for this person
    int dataSize = strlen(person->id) + strlen(person->age) + strlen(person->name) +
                      strlen(person->surname) + 7;  // +5 accounts for '#' and '%'

    // Calculate the size of the entire string, including the size variable
    int totalSize = dataSize + snprintf(NULL, 0, "%d", dataSize);

    // Convert totalSize to a string and store it in the size variable
    snprintf(person->size, BUFFER_SIZE, "%d", totalSize);
}

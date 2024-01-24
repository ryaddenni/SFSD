#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define FILE_NAME "users.txt"

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

// Function to calculate the size string
void calculateSize(struct Person *person);

// Function to save individual details to a text file
void saveToFile(struct Person *person, FILE *file) {
    fprintf(file, "%s#%c#%s#%s#%s#%s%%", person->size, person->flag, person->id, person->age, person->name, person->surname);
}

// Function to flush the buffer
void flushBuffer(char *buffer) {
    buffer[0] = '\0'; // Reset buffer to empty
}

// Function to display the content of the block buffer
void displayBlockBuffer(const char *block) {
    printf("Block Buffer Content: %s\n", block);
}

// Function to save the content of the buffer to the block buffer
void saveToBlock(char *block, char *buffer) {
    strcat(block, buffer); // Append buffer to block
}

int main() {
    // Declare a buffer to store user data
    char buffer[BUFFER_SIZE];
    buffer[0] = '\0'; // Initialize buffer as empty

    // Declare a block buffer
    char block[BUFFER_SIZE];
    block[0] = '\0'; // Initialize block as empty

    // Check if the users file is not empty and clear it if needed
    FILE *usersFile = fopen(FILE_NAME, "rb+");
    if (usersFile != NULL) {
        fseek(usersFile, 0, SEEK_END);
        long size = ftell(usersFile);

        // Clear the file if it's not empty
        if (size > 0) {
            freopen(FILE_NAME, "wb+", usersFile);
        } else {
            // Add a newline if the file is empty
            fprintf(usersFile, "\n");
        }
    } else {
        perror("Error opening users file");
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
        // Flush the buffer before getting new user input
        flushBuffer(buffer);

        // Input custom ID, age, name, and surname
        struct Person newPerson;
        newPerson.id = getUserInput("Enter custom ID (a number):");
        newPerson.age = getUserInput("Enter age:");
        newPerson.name = getUserInput("Enter name:");
        newPerson.surname = getUserInput("Enter surname:");
        newPerson.flag = '0';  // Initially set the flag to 0

        // Allocate memory for the size variable
        newPerson.size = (char *)malloc(BUFFER_SIZE);
        if (newPerson.size == NULL) {
            printf("Error: Memory allocation failed.\n");
            exit(1);
        }

        // Calculate and set the size string
        calculateSize(&newPerson);

        // Set the flag to 1
        newPerson.flag = '1';

        // Concatenate user data to the buffer
        sprintf(buffer, "%s#%c#%s#%s#%s#%s%%", newPerson.size, newPerson.flag, newPerson.id, newPerson.age, newPerson.name, newPerson.surname);

        // Copy the last block from the users file into the block buffer
        fseek(usersFile, -(BUFFER_SIZE - 1), SEEK_END); // Move to the last block
        fread(block, sizeof(char), BUFFER_SIZE - 1, usersFile); // Read the last block into the block buffer
        block[BUFFER_SIZE - 1] = '\0'; // Null-terminate the block buffer

        // Calculate the remaining empty space in the block
        int emptySpace = BUFFER_SIZE - strlen(block);

        // Compare the empty space with the size of the data in the buffer
        if (emptySpace >= strlen(buffer)) {
            // The buffer content fits into the block
            strcat(block, buffer); // Append buffer content to the block buffer
        } else {
            // Fill the block with the content of the buffer
            strncat(block, buffer, emptySpace);
            // Update the buffer to store the remaining data
            strcpy(buffer, buffer + emptySpace);
        }

        // Write the relevant part of the block buffer to the file
        fwrite(block, sizeof(char), strlen(block), usersFile);

        // Reset buffer
        flushBuffer(buffer);

        // Display the content of the block buffer
        displayBlockBuffer(block);

        // Save individual details to the user file
        saveToFile(&newPerson, usersFile);

        // Free allocated memory for the size variable
        free(newPerson.size);
    }

    // Close the users file
    fclose(usersFile);

    return 0;
}

// Function to dynamically allocate memory for a string input
char *getUserInput(const char *prompt) {
    printf("%s", prompt);

    char *buffer = NULL;
    size_t bufferSize = 0;

    // Read the input using getline
    if (getline(&buffer, &bufferSize, stdin) == -1) {
        perror("Error reading input");
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
    sprintf(person->size, "%d", totalSize);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define BLOCK_SEPARATOR "*"

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

// Function to delete a person from the file (logically)
void deletePersonFromFile(const char *idToDelete);

int main() {
    // Prompt user for the ID to delete
    char *idToDelete = getUserInput("Enter the ID to delete:");

    // Test the delete function
    deletePersonFromFile(idToDelete);

    // Free allocated memory for user input
    free(idToDelete);

    return 0;
}

void deletePersonFromFile(const char *idToDelete) {
    FILE *file = fopen("users.txt", "r+");
    if (file == NULL) {
        printf("Error opening users file.\n");
        exit(1);
    }

    // Create buffers for processing
    char prevBlock[BUFFER_SIZE];
    char currentBlock[BUFFER_SIZE];
    char *idLocation;
    long blockStartPosition = 0;  // Position where the current block starts in the file

    // Initialize the buffers
    memset(prevBlock, 0, sizeof(prevBlock));
    memset(currentBlock, 0, sizeof(currentBlock));

    while (1) {
        // Read the next character
        char bufferChar = fgetc(file);

        // Check for the end of file
        if (bufferChar == EOF) {
            // End of file reached, ID not found
            printf("ID not found in the file.\n");
            fclose(file);
            return;
        }

        // Add the character to the current block
        strncat(currentBlock, &bufferChar, 1);

        // Check if the current block contains the block separator
        if (strstr(currentBlock, BLOCK_SEPARATOR) != NULL) {
            // Move the content of the current block to the previous block
            strcpy(prevBlock, currentBlock);

            // Update blockStartPosition to the start of the next block
            blockStartPosition = ftell(file);

            // Reset the current block
            memset(currentBlock, 0, sizeof(currentBlock));

            // Read the next block into currentBlock
            fread(currentBlock, 1, BUFFER_SIZE, file);
        }

        // Check if ID is found in the current block
        idLocation = strstr(currentBlock, idToDelete);
        if (idLocation != NULL) {
            break;
        }
    }

    // Locate the position before the last '#' before the specified ID in the current block
    long positionBeforeLastHash = blockStartPosition + (idLocation - currentBlock - 2);
    if (positionBeforeLastHash >= 0) {
        // Go back and set the character before the last '#' to '0'
        fseek(file, positionBeforeLastHash, SEEK_SET);
        fputc('0', file);
        fflush(file);  // Flush changes to file
    }

    // Close the file
    fclose(file);

    printf("Person with ID %s logically deleted from the file.\n", idToDelete);
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

// Function to add a person to the buffer
void addPersonToBuffer(struct Person *person, char *buffer, int *bufferIndex, int *remainingBufferSpace) {
    // Calculate the space needed for the person's data
    int spaceNeeded = strlen(person->id) + strlen(person->age) + strlen(person->name) +
                      strlen(person->surname) + strlen(person->size) + 7;  // +5 accounts for '#' and '%'

    // Check if there is enough space in the buffer
    if (spaceNeeded > *remainingBufferSpace) {
        // Not enough space, reset buffer index and space
        *bufferIndex = 0;
        *remainingBufferSpace = BUFFER_SIZE;
    }

    // Add the person's data to the buffer
    sprintf(buffer + *bufferIndex, "%s#%c#%s#%s#%s#%s%%",
            person->size, person->flag, person->id, person->age, person->name, person->surname);

    // Update buffer index and remaining space
    *bufferIndex += spaceNeeded;
    *remainingBufferSpace -= spaceNeeded;
}

// Function to write the buffer to the file
void writeBufferToFile(FILE *file, char *buffer, int *bufferIndex, int *remainingBufferSpace) {
    // Write the buffer to the file if it's not empty
    if (*remainingBufferSpace < BUFFER_SIZE) {
        fwrite(buffer, 1, *bufferIndex, file);
    }
}

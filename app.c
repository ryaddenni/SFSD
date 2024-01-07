#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_MAP_SIZE 100

// Structure to represent an individual
struct Person {
    char *id;
    char *age;
    char *name;
    char *surname;
    struct Person *next;
};

// Structure to represent a node in the hash table
struct HashNode {
    char *id;
    struct Person *person;
    struct HashNode *next;
};

// Function to dynamically allocate memory for a string input
char *getUserInput(const char *prompt);

// Function to save individual details to a text file
void saveToFile(struct Person *person, FILE *file) {
    fprintf(file, "%s$", person->name);
    fprintf(file, "%s$", person->surname);
    fprintf(file, "%s$", person->age);
    fprintf(file, "%s$", person->id);
}

// Function to save hash map details to a text file
void saveMapToFile(struct HashNode *hashTable[], FILE *file) {
    for (int i = 0; i < HASH_MAP_SIZE; ++i) {
        struct HashNode *current = hashTable[i];
        while (current != NULL) {
            fprintf(file, "%s,%p\n", current->id, (void *)current->person);
            current = current->next;
        }
    }
}

// Function to add a new person to the linked list and hash table
void addPerson(struct Person **head, struct HashNode *hashTable[], FILE *usersFile) {
    // Buffer to store user input
    char inputBuffer[2048];
    size_t bufferIndex = 0;

    // Dynamically allocate memory for the new person
    struct Person *newPerson = (struct Person *)malloc(sizeof(struct Person));
    if (newPerson == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(1);
    }

    // Input custom ID, age, name, and surname into the buffer
    strcpy(inputBuffer, getUserInput("Enter custom ID (a number):"));
    strcat(inputBuffer, "$");
    strcpy(inputBuffer + strlen(inputBuffer), getUserInput("Enter age:"));
    strcat(inputBuffer, "$");
    strcpy(inputBuffer + strlen(inputBuffer), getUserInput("Enter name:"));
    strcat(inputBuffer, "$");
    strcpy(inputBuffer + strlen(inputBuffer), getUserInput("Enter surname:"));
    strcat(inputBuffer, "$");

    // Allocate blocks for the user input
    size_t blockSize = 64;
    size_t inputLength = strlen(inputBuffer);

    while (bufferIndex < inputLength) {
        // Dynamically allocate memory for the block
        char *block = (char *)malloc(blockSize);
        if (block == NULL) {
            printf("Error: Memory allocation failed.\n");
            exit(1);
        }

        // Copy data from the buffer to the block
        strncpy(block, inputBuffer + bufferIndex, blockSize - 1);
        block[blockSize - 1] = '\0'; // Null-terminate the block

        // Update the buffer index
        bufferIndex += blockSize - 1;

        // Save details to the user file
        fprintf(usersFile, "%s", block);

        // Free allocated memory for the block
        free(block);

        // If there's more data, allocate a new block
        if (bufferIndex < inputLength) {
            fprintf(usersFile, "$"); // Add the special character between blocks
        }
    }

    // Assign the next pointer
    newPerson->next = *head;

    // Update the head to point to the new person
    *head = newPerson;

    // Map custom ID to the pointer of the current person (hash table)
    int hashIndex = atoi(newPerson->id) % HASH_MAP_SIZE;

    // Create a new hash node
    struct HashNode *newNode = (struct HashNode *)malloc(sizeof(struct HashNode));
    if (newNode == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(1);
    }

    newNode->id = strdup(newPerson->id);
    newNode->person = newPerson;

    // Insert the hash node into the hash table
    newNode->next = hashTable[hashIndex];
    hashTable[hashIndex] = newNode;
}

int main() {
    // Declare a pointer to the head of the linked list
    struct Person *head = NULL;

    // Array to represent the hash table
    struct HashNode *hashTable[HASH_MAP_SIZE] = {NULL};

    // Open the users file for writing (this will clear the file)
    FILE *usersFile = fopen("users.txt", "w");
    if (usersFile == NULL) {
        printf("Error opening users file.\n");
        exit(1);
    }

    // Check if the map file is not empty and clear it if needed
    FILE *mapFile = fopen("map.txt", "r");
    if (mapFile != NULL) {
        fseek(mapFile, 0, SEEK_END);
        long size = ftell(mapFile);

        // Add a newline if the file is not empty
        if (size > 0) {
            fprintf(mapFile, "\n");
        }
    }

    int numClasses;

    // Prompt the user for the number of classes
    printf("Enter the number of classes: ");
    scanf("%d", &numClasses);

    // Clear the newline character from the input buffer
    while (getchar() != '\n');

    // Loop to add individuals to the linked list and hash table
    for (int i = 0; i < numClasses; ++i) {
        addPerson(&head, hashTable, usersFile);
    }

    fclose(usersFile);

    // Save hash map details to the map file
    FILE *mapFile2 = fopen("map.txt", "a");
    if (mapFile2 == NULL) {
        printf("Error opening map file.\n");
        exit(1);
    }

    saveMapToFile(hashTable, mapFile2);

    fclose(mapFile2);

    // Display the entered information
   

    // Free allocated memory for the hash table
    for (int i = 0; i < HASH_MAP_SIZE; ++i) {
        struct HashNode *current = hashTable[i];
        while (current != NULL) {
            struct HashNode *temp = current;
            current = current->next;

            // Free allocated memory for this hash node
            free(temp->id);
            free(temp);
        }
    }

    return 0;
}

//---------------------------------------------------------------------------------------------------------------/
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

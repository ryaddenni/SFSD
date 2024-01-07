#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_MAP_SIZE 100
#define BLOCK_SIZE 64
#define BUFFER_SIZE 2048

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

// Structure to represent a block of memory
struct MemoryBlock {
    char data[BLOCK_SIZE];
    struct MemoryBlock *next;
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
void addPerson(struct Person **head, struct HashNode *hashTable[], struct MemoryBlock **currentBlock, int *blockCounter, FILE *usersFile) {
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

    // Calculate the total size of the data for this person
    size_t totalSize = strlen(newPerson->name) + strlen(newPerson->surname) + strlen(newPerson->age) + strlen(newPerson->id);

    // Allocate a new block for every 64 bytes of data
    while (totalSize > 0) {
        // Allocate a new block
        struct MemoryBlock *newBlock = (struct MemoryBlock *)malloc(sizeof(struct MemoryBlock));
        if (newBlock == NULL) {
            printf("Error: Memory allocation failed.\n");
            exit(1);
        }

        // Update the block counter
        (*blockCounter)++;

        // Link the new block
        newBlock->next = *currentBlock;
        *currentBlock = newBlock;

        // Calculate the amount of data to write to this block
        size_t blockSize = totalSize > BLOCK_SIZE ? BLOCK_SIZE : totalSize;

        // Write the data to the new block
        snprintf((*currentBlock)->data, blockSize + 1, "%s$", newPerson->name);

        // Update the total size
        totalSize -= blockSize;
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

    // Save details to the user file
    saveToFile(newPerson, usersFile);
}

int main() {
    // Declare a pointer to the head of the linked list
    struct Person *head = NULL;

    // Array to represent the hash table
    struct HashNode *hashTable[HASH_MAP_SIZE] = {NULL};

    // Pointer to the current memory block
    struct MemoryBlock *currentBlock = NULL;

    // Counter for the number of allocated blocks
    int blockCounter = 0;

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
        addPerson(&head, hashTable, &currentBlock, &blockCounter, usersFile);
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

        // Move to the next person in the list
        struct Person *temp = current;
        current = current->next;

        // Free allocated memory for this person
        free(temp->name);
        free(temp->surname);
        free(temp->age);
        free(temp->id);
        free(temp);
    }

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

    // Free allocated memory for the memory blocks
    struct MemoryBlock *currentBlockPtr = currentBlock;
    while (currentBlockPtr != NULL) {
        struct MemoryBlock *temp = currentBlockPtr;
        currentBlockPtr = currentBlockPtr->next;
        free(temp);
    }

    printf("Number of 64-byte blocks allocated: %d\n", blockCounter);

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define FILE_NAME "users.txt"
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

FILE *createFile();
void createUserData(FILE *file);

void flushBuffer(char *buffer) {
    buffer[0] = '\0';
}

void displayBlockBuffer(const char *block) {
    printf("Block Buffer Content: %s\n", block);
}

void saveToBlock(char *block, char *buffer) {
    strcat(block, buffer);
}

char *getUserInput(const char *prompt) {
    printf("%s", prompt);

    char *buffer = NULL;
    size_t bufferSize = 0;

    if (getline(&buffer, &bufferSize, stdin) == -1) {
        perror("Error reading input");
        exit(1);
    }

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    return buffer;
}

void calculateSize(struct Person *person) {
    int dataSize = strlen(person->id) + strlen(person->age) + strlen(person->name) + strlen(person->surname) + 7;
    int totalSize = dataSize + snprintf(NULL, 0, "%d", dataSize);
    sprintf(person->size, "%d", totalSize);
}

FILE *createFile() {
    char fileName[256];

    printf("Enter the file name: ");
    scanf("%255s", fileName);

    FILE *file = fopen(fileName, "r");
    if (file != NULL) {
        printf("File '%s' already exists. Exiting program.\n", fileName);
        fclose(file);
        exit(1);
    } else {
        file = fopen(fileName, "w");
        if (file == NULL) {
            printf("Error creating file '%s'. Exiting program.\n", fileName);
            exit(1);
        } else {
            printf("File '%s' created successfully.\n", fileName);
        }
    }

    return file;
}

void createUserData(FILE *file) {
    char buffer[BUFFER_SIZE];
    buffer[0] = '\0';

    char block[BUFFER_SIZE];
    block[0] = '\0';

    int numClasses;

    printf("Enter the number of classes: ");
    scanf("%d", &numClasses);

    while (getchar() != '\n');

    for (int i = 0; i < numClasses; ++i) {
        flushBuffer(buffer);

        struct Person newPerson;
        newPerson.id = getUserInput("Enter custom ID (a number):");
        newPerson.age = getUserInput("Enter age:");
        newPerson.name = getUserInput("Enter name:");
        newPerson.surname = getUserInput("Enter surname:");
        newPerson.flag = '1';

        newPerson.size = (char *)malloc(BUFFER_SIZE);
        if (newPerson.size == NULL) {
            printf("Error: Memory allocation failed.\n");
            exit(1);
        }

        calculateSize(&newPerson);

        sprintf(buffer, "%s#%c#%s#%s#%s#%s%%", newPerson.size, newPerson.flag, newPerson.id, newPerson.age, newPerson.name, newPerson.surname);

        fseek(file, -(BUFFER_SIZE - 1), SEEK_END);
        fread(block, sizeof(char), BUFFER_SIZE - 1, file);
        block[BUFFER_SIZE - 1] = '\0';

        int emptySpace = BUFFER_SIZE - strlen(block);

        if (emptySpace >= strlen(buffer)) {
            strcat(block, buffer);
        } else {
            strncat(block, buffer, emptySpace);
            strcpy(buffer, buffer + emptySpace);
        }

        fseek(file, 0, SEEK_END);
        fwrite(block, sizeof(char), strlen(block), file);

        flushBuffer(buffer);
        displayBlockBuffer(block);
        flushBuffer(block);

        free(newPerson.size);
    }
}


void createtxt();
void deletePerson();
void searchPerson();

int main() {
    int choice;
     FILE *usersFile;

    do {
        printf("\nOptions:\n");
        printf("1. Create \n");
        printf("2. Delete Person\n");
        printf("3. Search Person\n");
        printf("4. Exit\n");
        printf("Enter your choice (1-4): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createtxt();
                break;
            case 2:
                deletePerson();
                break;
            case 3:
                searchPerson();
                break;
            case 4:
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 4.\n");
                break;
        }

    } while (choice != 4);

    return 0;
}


void createtxt() {
        FILE *usersFile = createFile();
        createUserData(usersFile);
    fclose(usersFile);
    printf("Created successfully.\n");
}

void deletePersonFromFile(const char *fileName, const char *idToDelete) {
    FILE *file = fopen(fileName, "r+");
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





void deletePerson() {
      int c;
    while ((c = getchar()) != '\n' && c != EOF);

       // Prompt user for the file name
    char *fileName = getUserInput("Enter the file name:");

    // Try opening the file to check if it exists
    FILE *fileCheck = fopen(fileName, "r");
    if (fileCheck == NULL) {
        printf("Error: File '%s' not found.\n", fileName);
        free(fileName);
        return;
        fclose(fileCheck);
    }  else {  int c;
    while ((c = getchar()) != '\n' && c != EOF);

        // Prompt user for the ID to delete
    char *idToDelete = getUserInput("Enter the ID to delete:");

    // Test the delete function
    deletePersonFromFile(fileName, idToDelete);
    free(idToDelete);
    printf("Person deleted successfully.\n"); };
    
    // Free allocated memory for user input
    free(fileName);

}


int isIdValid(const char *buffer, int position) {
    // Check the character before the '#' before the specified ID
    if (position > 0 && buffer[position - 1] == '1') {
        return 1;  // ID is valid
    } else {
        return 0;  // ID is not valid
    }
}

void printPersonDetails(const char *buffer, const char *idToSearch) {
    // Find the position of the specified ID
    int position = strstr(buffer, idToSearch) - buffer;

    // Extract the ID
    printf("Your ID: %s\n", idToSearch);

    // Extract the age
    char *ageStart = strchr(buffer + position, '#') + 1;  // Move to the character after the first #
    char *ageEnd = strchr(ageStart, '#');  // Find the next #
    *ageEnd = '\0';  // Null-terminate to print only the age
    printf("Your age: %s\n", ageStart);

    // Extract the name
    char *nameStart = ageEnd + 1;  // Move to the character after the second #
    char *nameEnd = strchr(nameStart, '#');  // Find the next #
    *nameEnd = '\0';  // Null-terminate to print only the name
    printf("Your name: %s\n", nameStart);

    // Extract the surname
    char *surnameStart = nameEnd + 1;  // Move to the character after the third #
    char *surnameEnd = strchr(surnameStart, '%');  // Find the next %
    *surnameEnd = '\0';  // Null-terminate to print only the surname
    printf("Your surname: %s\n", surnameStart);
}

void searchPersonById(const char *fileName, const char *idToSearch) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Error opening users file.\n");
        exit(1);
    }

    // Create buffer for processing
    char buffer[BUFFER_SIZE];

    // Initialize the buffer
    memset(buffer, 0, sizeof(buffer));

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Check if ID is found in the current block
        char *idLocation = strstr(buffer, idToSearch);
        if (idLocation != NULL) {
            // Check if the ID is valid (flag is '1')
            if (isIdValid(buffer, idLocation - buffer - 1)) {
                printPersonDetails(buffer, idToSearch);
                fclose(file);
                return;
            } else {
                // ID exists, but flag is '0'
                printf("This ID doesn't exist.\n");
                fclose(file);
                return;
            }
        }
    }

    // ID not found
    printf("ID not found in the file.\n");

    // Close the file
    fclose(file);
}




void searchPerson() {

        int c;
    while ((c = getchar()) != '\n' && c != EOF);
           // Prompt user for the file name
    char *fileName = getUserInput("Enter the file name:");

    // Try opening the file to check if it exists
    FILE *fileCheck = fopen(fileName, "r");
    if (fileCheck == NULL) {
        printf("Error: File '%s' not found.\n", fileName);
        free(fileName);
        return;
        fclose(fileCheck);
    }  else {  int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // Prompt user for the ID to search
    char idToSearch[BUFFER_SIZE];
    printf("Enter the ID to search: ");
    fgets(idToSearch, sizeof(idToSearch), stdin);
    idToSearch[strcspn(idToSearch, "\n")] = '\0';  // Remove newline character

    // Call the search function
    searchPersonById(fileName, idToSearch);

    printf("Search completed.\n"); };
}



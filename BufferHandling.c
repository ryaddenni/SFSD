#include <stdio.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_PERSON_SIZE 256

typedef struct {
    char Size[4];  // Assuming size is less than 1000 characters
    char Flag;
    char ID[20];
    char Age[4];   // Assuming age is less than 100
    char Name[50];
    char Surname[50];
} Person;

void printPerson(const Person *person) {
    printf("Size: %s, Flag: %c, ID: %s, Age: %s, Name: %s, Surname: %s\n",
           person->Size, person->Flag, person->ID, person->Age, person->Name, person->Surname);
}

void addPersonToFile(const char *filename, const Person *newPerson) {
    FILE *file = fopen(filename, "rb+");
    if (file == NULL) {
        printf("Error opening the file.\n");
        return;
    }

    Person currentPerson;
    fread(&currentPerson, sizeof(Person), 1, file);

    // Copy the existing person to the buffer
    char buffer[MAX_BUFFER_SIZE];
    memcpy(buffer, &currentPerson, sizeof(Person));

    // Check if there is enough space in the buffer for the new person
    int remainingSpace = MAX_BUFFER_SIZE - sizeof(Person);
    int newPersonSize = atoi(newPerson->Size);
    if (remainingSpace < newPersonSize) {
        // Not enough space, use a second buffer
        char secondBuffer[MAX_BUFFER_SIZE];
        memcpy(secondBuffer, newPerson, sizeof(Person));

        // Copy the first part of the new person to the first buffer
        memcpy(buffer + sizeof(Person), secondBuffer, remainingSpace);

        // Paste the content of the first buffer back to the file
        fseek(file, 0, SEEK_SET);
        fwrite(buffer, sizeof(char), sizeof(buffer), file);

        // Flush the first buffer
        memset(buffer, 0, sizeof(buffer));

        // Copy the remaining part of the new person to the first buffer
        memcpy(buffer, secondBuffer + remainingSpace, newPersonSize - remainingSpace);

        // Create a new block for the remaining content of the second buffer
        fseek(file, 0, SEEK_END);
        fwrite(secondBuffer + remainingSpace, sizeof(char), newPersonSize - remainingSpace, file);
    } else {
        // Enough space, add the new person to the first buffer
        memcpy(buffer + sizeof(Person), newPerson, newPersonSize);

        // Paste the content of the first buffer back to the file
        fseek(file, 0, SEEK_SET);
        fwrite(buffer, sizeof(char), sizeof(buffer), file);
    }

    fclose(file);
}

int BufferHandling() {
    // Example: create a new person
    Person newPerson;
    strcpy(newPerson.Size, "256");
    newPerson.Flag = '1';
    strcpy(newPerson.ID, "ID2");
    strcpy(newPerson.Age, "25");
    strcpy(newPerson.Name, "John");
    strcpy(newPerson.Surname, "Doe");

    // Print the new person
    printf("New Person:\n");
    printPerson(&newPerson);

    // Add the new person to the file
    addPersonToFile("file.txt", &newPerson);

    // Print the updated content of the file
    FILE *file = fopen("file.txt", "r");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        rewind(file);

        char fileContent[MAX_BUFFER_SIZE + 1];
        fread(fileContent, sizeof(char), fileSize, file);
        fileContent[fileSize] = '\0';

        printf("\nUpdated File Content:\n%s\n", fileContent);

        fclose(file);
    } else {
        printf("Error opening the file for reading.\n");
    }

    return 0;
}

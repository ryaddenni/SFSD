#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void searchPersonById(const char *idToSearch) {
    FILE *file = fopen("users.txt", "r");
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
            // Find the position of the specified ID
            int position = idLocation - buffer;

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

            // Close the file
            fclose(file);
            return;
        }
    }

    // ID not found
    printf("ID not found in the file.\n");

    // Close the file
    fclose(file);
}

int main() {
    // Prompt user for the ID to search
    char idToSearch[BUFFER_SIZE];
    printf("Enter the ID to search: ");
    fgets(idToSearch, sizeof(idToSearch), stdin);
    idToSearch[strcspn(idToSearch, "\n")] = '\0';  // Remove newline character

    // Call the search function
    searchPersonById(idToSearch);

    return 0;
}

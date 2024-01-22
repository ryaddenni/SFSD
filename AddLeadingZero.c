#include <stdio.h>
#include <stdlib.h>

void addLeadingZero(char *numberStr) {
    int number = atoi(numberStr); // Convert string to integer
    if (number < 100) {
        sprintf(numberStr, "%03d", number); // Format the string with leading zeros
    }
}


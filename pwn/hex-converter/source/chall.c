#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 0x120

int win() {
    system("/bin/sh");
    return 0;
}

void clear_trailing_newline() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
    return;
}

int convert(int is_slicing_enabled) {
    int i;
    int len;
    char buf[120];
    printf("Enter the text to convert: ");
    fflush(stdout);
    fgets(buf, sizeof(buf), stdin);

    if (is_slicing_enabled) {
        printf("Enter the number of characters to convert: ");
        fflush(stdout);
        if (scanf("%d", &len) != 1) {
            printf("Invalid length.\n\n");
            clear_trailing_newline();
            return 1;
        }
        clear_trailing_newline();
        if (len > MAX_SIZE) {
            printf("The length can't be greater than the text size.\n\n");
            return 1;
        }
    } else {
        len = strlen(buf) - 1;
    }
    
    char* hex_buf = (char*)malloc((len * 2) + 1);
    for (i = 0; i < len; i++) {
        sprintf(&hex_buf[i*2], "%02x", (unsigned char)buf[i]);
    }
    printf("Here is the hex-converted text: 0x%s\n\n", hex_buf);
    return 1;
}

void menu() {
    printf("What would you like to do?\n");
    printf("1. Convert text to hex\n");
    printf("2. Convert text to hex with slicing (beta)\n");
    printf("3. Leave your feedback\n");
    printf("4. Exit\n");
    // printf("5. Print flag\n");
    return;
}

void feedback() {
    char buf[120];
    printf("Rate your experience with us: ");
    fflush(stdout);
    fgets(buf, MAX_SIZE, stdin);
    printf("We really appreciate it. Bye!\n");
    fflush(stdout);
    return;
}

int main() {
    int option;
    while (true) {
        menu();
        if (scanf("%d", &option) != 1) {
            printf("Invalid choice.\n\n");
            clear_trailing_newline();
            continue;
        }
        clear_trailing_newline();
        if (option == 1) {
            convert(0);
        } else if (option == 2) {
            convert(1);
        } else if (option == 3) {
            feedback();
            exit(0);
        } else if (option == 4) {
            printf("Bye!\n");
            exit(0);
        // } else if (option == 5) {
        //     win();
        } else {
            printf("Invalid choice.\n\n");
            continue;
        }
    }
}

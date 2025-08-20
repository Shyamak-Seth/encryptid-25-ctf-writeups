#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 9

int sudoku_grid[SIZE * SIZE];
int visible[SIZE * SIZE];

// get the index of the chosen cell inside the grid
int index(int row, int col) {
    return row * SIZE + col;
}

// to prevent memory corruption
int is_safe(int grid[], int row, int col, int num) {
    for (int i = 0; i < SIZE; i++) {
        if (grid[index(row, i)] == num || grid[index(i, col)] == num)
            return 0;

        int boxRow = (row / 3) * 3 + i / 3;
        int boxCol = (col / 3) * 3 + i % 3;
        if (grid[index(boxRow, boxCol)] == num)
            return 0;
    }
    return 1;
}

// generate and fill the sudoku
int fill_sudoku(int grid[], int pos) {
    if (pos == SIZE * SIZE) return 1;

    int row = pos / SIZE;
    int col = pos % SIZE;

    int nums[SIZE];
    for (int i = 0; i < SIZE; i++) {
        nums[i] = i + 1;
    }
    for (int i = 0; i < SIZE; i++) {
        int r = rand() % SIZE;
        int temp = nums[i];
        nums[i] = nums[r];
        nums[r] = temp;
    }

    for (int i = 0; i < SIZE; i++) {
        if (is_safe(grid, row, col, nums[i])) {
            grid[pos] = nums[i];
            if (fill_sudoku(grid, pos + 1)) return 1;
            grid[pos] = 0;
        }
    }
    return 0;
}

// randomly decide which cells are to be shown in the grid
void generate_mask() {
    for (int i = 0; i < SIZE; i++) {
        int shown_cols[SIZE] = {0};
        for (int j = 0; j < 3; j++) {
            int c;
            do {
                c = rand() % SIZE;
            } while (shown_cols[c]);
            shown_cols[c] = 1;
            visible[index(i, c)] = 1;
        }
    }

    for (int j = 0; j < SIZE; j++) {
        int shown_rows[SIZE] = {0};
        for (int i = 0; i < 3; i++) {
            int r;
            do {
                r = rand() % SIZE;
            } while (shown_rows[r]);
            shown_rows[r] = 1;
            visible[index(r, j)] = 1;
        }
    }
}

// neatly format and print the sudoku
void print_grid(int grid[], int mask[]) {
    printf("\n   ");
    for (int j = 0; j < SIZE; j++) printf(" %d ", j);
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        printf(" %d ", i);
        for (int j = 0; j < SIZE; j++) {
            int idx = index(i, j);
            if (mask[idx])
                if (grid[idx] != 0)
                    printf(" %d ", grid[idx]);
                else
                    printf(" - ");
            else
                printf(" - ");
        }
        printf("\n");
    }
}

// win check
int check_completion(int user_grid[]) {
    for (int i = 0; i < SIZE * SIZE; i++) {
        if (user_grid[i] == 0) return 0;
    }
    return 1;
}

int banner() {
    printf(" __    __  ____ __      ___   ___   ___  ___  ____    ______   ___\n");
    printf(" ||    || ||    ||     //    // \\\\  ||\\\\//|| ||       | || |  // \\\\ \n");
    printf(" \\\\ /\\ // ||==  ||    ((    ((   )) || \\/ || ||==       ||   ((   ))\n");
    printf("  \\V/\\V/  ||___ ||__|  \\\\__  \\\\_//  ||    || ||___      ||    \\\\_// \n\n");

    printf("             _____ _    _ _____   ____  _  ___    _\n");
    printf("            / ____| |  | |  __ \\ / __ \\| |/ / |  | |\n");
    printf("           | (___ | |  | | |  | | |  | | \' /| |  | |\n");
    printf("            \\___ \\| |  | | |  | | |  | |  < | |  | |\n");
    printf("            ____) | |__| | |__| | |__| | . \\| |__| |\n");
    printf("           |_____/ \\____/|_____/ \\____/|_|\\_\\\\____/ \n\n");
    return 0;
}

int main() {
    banner();
    srand(time(NULL));
    int user_grid[SIZE * SIZE] = {0}; // the grid that the user sees
    char user_id[32];
    char command[96];
    int random_number = rand();
    snprintf(user_id, sizeof(user_id), "%ld", random_number);
    
    fill_sudoku(sudoku_grid, 0);
    generate_mask();

    // fill the grid only with the visible numbers
    for (int i = 0; i < SIZE * SIZE; i++) {
        if (visible[i])
            user_grid[i] = sudoku_grid[i];
    }
    
    // continue the game until the player wins
    while (!check_completion(user_grid)) {
        print_grid(user_grid, (int[SIZE * SIZE]){[0 ... SIZE * SIZE - 1] = 1});
        int row, col, num;

        printf("\nEnter row: ");
        int row_status = scanf("%d", &row);
        if (row_status != 1) { // the input was not a number
            printf("Invalid input. Please try again.\n");
            continue;
        }
        printf("Enter column: ");
        int column_status = scanf("%d", &col);
        if (column_status != 1) { // the input was not a number
            printf("Invalid input. Please try again.\n");
            continue;
        }
        printf("Enter number: ");
        int num_status = scanf("%d", &num);
        if (num_status != 1) { // the input was not a number
            printf("Invalid input. Please try again.\n");
            continue;
        }

        // prevent out-of-bounds write access on the stack
        if (row >= SIZE || col >= SIZE) {
            printf("Invalid row or column. Please try again.\n");
            continue;
        }
        
        // update the visible grid
        int idx = index(row, col);
        user_grid[idx] = num;

        // log the attempt
        snprintf(command, sizeof(command), "echo 'ATTEMPT BY USER %s' >> /tmp/logs.txt", user_id);
        system(command);
        printf("\n");
    }

    // win
    printf("\nYou win!\n");
    return 0;
}

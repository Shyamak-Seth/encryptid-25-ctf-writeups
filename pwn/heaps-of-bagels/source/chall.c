#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 64
#define MAX_ORDERS 10

typedef struct {
    char* username_ptr;
    int user_age;
    int user_weight;
    int user_height;
    int user_bmi;
} UserProfile;

char** order_bagel() {
    char buf[MAX_SIZE];
    printf("Enter bagel flavor: ");
    fflush(stdout);
    fgets(buf, MAX_SIZE, stdin);
    char* heap_buf = (char *)malloc(sizeof(buf));
    if (heap_buf == NULL) {
        printf("A critical error has occured, and the program must be stopped. Sorry!\n");
        exit(1);
    }
    memcpy(heap_buf, buf, sizeof(buf));
    char** heap_buf_ptr = (char **)malloc(sizeof(char *));
    if (heap_buf_ptr == NULL) {
        printf("A critical error has occured, and the program must be stopped. Sorry!\n");
        free(heap_buf);
        exit(1);
    }
    *heap_buf_ptr = heap_buf;
    return heap_buf_ptr;
}

char* update_profile() {
    int len;
    printf("How long is your name: ");
    fflush(stdout);
    if (scanf("%d", &len) != 1) {
        printf("Invalid length.\n");
        exit(1);
    }
    if (len < 0) {
        printf("Length can't be negative!!\n");
    }
    getchar();
    char* name_buf = (char *)malloc(len+1);
    if (name_buf == NULL) {
        printf("A critical error has occured.\n");
        exit(1);
    }
    printf("Enter your name: ");
    fflush(stdout);
    fgets(name_buf, len+1, stdin);
    printf("Profile updated!\n");
    return name_buf;
}

void menu() {
    printf("\nHow can we help you today?\n");
    printf("1. Order a bagel\n");
    printf("2. View order status\n");
    printf("3. Edit your bagel order\n");
    printf("4. Cancel an order\n");
    printf("5. Show your profile\n");
    printf("6. Update your profile\n");
    printf("7. Show the quote of the day\n");
    printf("8. Exit\n");
    printf("=> ");
    fflush(stdout);
}

void banner() {
    printf("        ███████████        \n");
    printf("     ███░░░░░░░░░░███     \n");
    printf("   ██░░░░░░░ ░░░░░░░██    \n");
    printf("  ██░░ ░░░░░░░░░░░░░░██   \n");
    printf(" ██░░░░░░░░░░░░░░ ░░░██  \n");
    printf(" ██░░░░░░░░░░░░░░░░░░██  \n");
    printf(" ██░░░░!!BAGELS!!░░░░██  \n");
    printf(" ██░░░░░░░░░░░░░░░░░░██  \n");
    printf(" ██░░░ ░░░░░░░░░░░ ░░██  \n");
    printf("  ██░░░░░░░░░░░░░░░░░██   \n");
    printf("   ██░░░░░░░░░ ░░░░░██    \n");
    printf("     ███░░░░░░░░░███      \n");
    printf("        █████████         \n\n\n");
}

int main() {
    int choice, index;
    int current_order = -1;
    char** mem_list[10];
    char *user;
    const char *quote_of_the_day = "Bagels take you to a place that is beyond reality.";
    banner();
    user = update_profile();
    while (1) {
        menu();
        if (scanf("%d", &choice) != 1) break;
        getchar();
        switch (choice) {
            case 1:
                if (current_order >= MAX_ORDERS - 1) {
                    printf("Maximum order limit reached!\n");
                    break;
                }
                char** my_buf = order_bagel();
                current_order += 1;
                printf("Your Order ID is: %d\n", current_order);
                mem_list[current_order] = my_buf;
                break;
            case 2:
                printf("Enter Order ID: ");
                fflush(stdout);
                if (scanf("%d", &index) != 1) {
                    printf("Please enter a valid order ID.\n");
                    break;
                }
                if (index < 0 || index > current_order) {
                    printf("Order not found!\n");
                    break;
                }
                getchar();
                printf("Status for Order %d\n", index);
                printf("Flavor: %s", *mem_list[index]);
                printf("Status: departured...\n");
                break;
            case 3:
                printf("Enter Order ID: ");
                fflush(stdout);
                if (scanf("%d", &index) != 1) {
                    printf("Please enter a valid order ID.\n");
                    break;
                }
                if (index < 0 || index > current_order) {
                    printf("Order not found!\n");
                    break;
                }
                getchar();
                printf("Enter new flavor: ");
		        fflush(stdout);
                char* order_buf = *mem_list[index];
                fgets(order_buf, MAX_SIZE, stdin);
                printf("Order updated successfully!\n");
                break;
            case 4:
                printf("Enter Order ID: ");
                fflush(stdout);
                if (scanf("%d", &index) != 1) {
                    printf("Please enter a valid order ID.\n");
                    break;
                }
                if (index < 0 || index > current_order) {
                    printf("Order not found!\n");
                    break;
                }
                getchar();
                free(*mem_list[index]);
                free(mem_list[index]);
                break;
            case 5:
                UserProfile* user_profile = (UserProfile*)&user;
                printf("Your username: %s\n", user_profile->username_ptr);
                printf("Your age: %d\n", user_profile->user_age);
                printf("Your weight: %d kg\n", user_profile->user_weight);
                printf("Your height: %d cm\n", user_profile->user_height);
                printf("Your BMI: %d whatevs\n", user_profile->user_bmi);
                break;
            case 6:
                user = update_profile();
                printf("Username updated to %s\n", user);
                break;
            case 7:
                printf("The quote of the day is: \"%s\"\n", quote_of_the_day);
                break;
            case 8:
                printf("Bye!\n");
                exit(0);
            default:
                printf("Invalid choice.\n");
                break;
        }
    }
}

/* 
 * ============================================================
 *  VENDING MACHINE SIMULATION - REVISED FINAL VERSION
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================= CONSTANTS ================= */

#define MAX_PRODUCTS   20
#define MAX_NAME_LEN   50
#define NUM_STORES     4
#define MAX_INV_LINES  100

/* ================= STRUCTURES ================= */

typedef struct {
    char name[MAX_NAME_LEN];
    float price;
    int stock;
} Product;

typedef struct {
    char storeName[MAX_NAME_LEN];
    char category[MAX_NAME_LEN];
    char filename[MAX_NAME_LEN];

    Product items[MAX_PRODUCTS];
    int productCount;

} Store;

/* ================= STORE DATA ================= */

Store stores[NUM_STORES] = {

    { "Mariejoe's Store", "Drinks", "mariejoe_stock.txt", {}, 0 },

    { "Jasmine's Store", "Chips", "jasmine_stock.txt", {}, 0 },

    { "Aizeah's Store", "Ready-to-Eat Meals",
      "aizeah_stock.txt", {}, 0 },

    { "Cyril's Store", "Candies",
      "cyril_stock.txt", {}, 0 }
};

/* ================= FUNCTION PROTOTYPES ================= */

void loadStock(Store *s);
void saveStock(Store *s);

void displayProducts(Store *s, int storeNumber);

void viewAllProducts(void);
void viewStoreProducts(int storeIdx);

void buyProduct(float *balance,
                char purchases[][MAX_NAME_LEN],
                float prices[],
                int *purchaseCount);

void viewInventory(float balance,
                   char purchases[][MAX_NAME_LEN],
                   float prices[],
                   int count);

float loadBalance(void);

void saveInventory(float balance,
                   char purchases[][MAX_NAME_LEN],
                   float prices[],
                   int count);

void loadInventory(float *balance,
                   char purchases[][MAX_NAME_LEN],
                   float prices[],
                   int *count);

void displayMainMenu(void);

void printDivider(char ch, int width);
void printHeader(const char *title);

int getValidInt(int min, int max);

/* ============================================================
 *  FILE HANDLING
 * ============================================================
 */

void loadStock(Store *s) {

    FILE *fp = fopen(s->filename, "r");

    if (fp == NULL) {
        printf("[ERROR] Cannot open %s\n", s->filename);
        exit(1);
    }

    s->productCount = 0;

    while (s->productCount < MAX_PRODUCTS &&
           fscanf(fp, "%s %f %d",
                  s->items[s->productCount].name,
                  &s->items[s->productCount].price,
                  &s->items[s->productCount].stock) == 3) {

        char *p = s->items[s->productCount].name;

        while (*p) {

            if (*p == '_')
                *p = ' ';

            p++;
        }

        s->productCount++;
    }

    fclose(fp);
}

void saveStock(Store *s) {

    FILE *fp = fopen(s->filename, "w");

    if (fp == NULL) {
        printf("[ERROR] Cannot save %s\n", s->filename);
        return;
    }

    for (int i = 0; i < s->productCount; i++) {

        char temp[MAX_NAME_LEN];

        strcpy(temp, s->items[i].name);

        char *p = temp;

        while (*p) {

            if (*p == ' ')
                *p = '-';

            p++;
        }

        fprintf(fp, "%s %.2f %d\n",
                temp,
                s->items[i].price,
                s->items[i].stock);
    }

    fclose(fp);
}

/* ============================================================
 *  INVENTORY FILE
 * ============================================================
 */

float loadBalance(void) {

    FILE *fp = fopen("inventory.txt", "r");

    if (fp == NULL)
        return 500.00f;

    float balance = 500.00f;

    fscanf(fp, "%f", &balance);

    fclose(fp);

    return balance;
}

void saveInventory(float balance,
                   char purchases[][MAX_NAME_LEN],
                   float prices[],
                   int count) {

    FILE *fp = fopen("inventory.txt", "w");

    if (fp == NULL) {
        printf("[ERROR] Cannot save inventory.\n");
        return;
    }

    fprintf(fp, "%.2f\n", balance);

    for (int i = 0; i < count; i++) {

        char temp[MAX_NAME_LEN];

        strcpy(temp, purchases[i]);

        char *p = temp;

        while (*p) {

            if (*p == ' ')
                *p = '_';

            p++;
        }

        fprintf(fp, "%s %.2f\n",
                temp,
                prices[i]);
    }

    fclose(fp);
}

void loadInventory(float *balance,
                   char purchases[][MAX_NAME_LEN],
                   float prices[],
                   int *count) {

    *balance = 500.00f;
    *count = 0;

    FILE *fp = fopen("inventory.txt", "r");

    if (fp == NULL)
        return;

    fscanf(fp, "%f", balance);

    while (*count < MAX_INV_LINES &&
           fscanf(fp, "%s %f",
                  purchases[*count],
                  &prices[*count]) == 2) {

        char *p = purchases[*count];

        while (*p) {

            if (*p == '_')
                *p = ' ';

            p++;
        }

        (*count)++;
    }

    fclose(fp);
}

/* ============================================================
 *  DISPLAY FUNCTIONS
 * ============================================================
 */

void printDivider(char ch, int width) {

    for (int i = 0; i < width; i++)
        putchar(ch);

    putchar('\n');
}

void printHeader(const char *title) {

    printDivider('=', 60);

    printf("  %s\n", title);

    printDivider('=', 60);
}

void displayProducts(Store *s, int storeNumber) {

    printf("\n[STORE %d] %s - %s\n",
           storeNumber,
           s->storeName,
           s->category);

    printDivider('-', 60);

    printf("%-5s %-25s %-12s %-10s\n",
           "No.",
           "Product",
           "Price",
           "Stock");

    printDivider('-', 60);

    for (int i = 0; i < s->productCount; i++) {

        if (s->items[i].stock > 0) {

            printf("%-5d %-25s PHP %-7.2f %d pcs\n",
                   i + 1,
                   s->items[i].name,
                   s->items[i].price,
                   s->items[i].stock);

        } else {

            printf("%-5d %-25s %-12s %s\n",
                   i + 1,
                   s->items[i].name,
                   "---",
                   "OUT OF STOCK");
        }
    }

    printDivider('-', 60);
}

/* ============================================================
 *  FEATURES
 * ============================================================
 */

void viewAllProducts(void) {

    printHeader("VENDING MACHINE - ALL STORES");

    for (int i = 0; i < NUM_STORES; i++) {

        displayProducts(&stores[i], i + 1);
    }
}

void viewStoreProducts(int storeIdx) {

    displayProducts(&stores[storeIdx], storeIdx + 1);
}

void buyProduct(float *balance,
                char purchases[][MAX_NAME_LEN],
                float prices[],
                int *purchaseCount) {

    printHeader("BUY A PRODUCT");

    printf("\nSelect a Store:\n");

    for (int i = 0; i < NUM_STORES; i++) {

        printf("[%d] %s (%s)\n",
               i + 1,
               stores[i].storeName,
               stores[i].category);
    }

    printf("[0] Cancel\n");

    printf("\nEnter store number: ");

    int sc = getValidInt(0, NUM_STORES);

    if (sc == 0) {
        printf("\nPurchase cancelled.\n");
        return;
    }

    int storeIdx = sc - 1;

    viewStoreProducts(storeIdx);

    printf("Select product number: ");

    int pc = getValidInt(1, stores[storeIdx].productCount);

    int prodIdx = pc - 1;

    Product *prod = &stores[storeIdx].items[prodIdx];

    if (prod->stock <= 0) {

        printf("\n[!] Product out of stock.\n");
        return;
    }

    if (*balance < prod->price) {

        printf("\n[!] Insufficient balance.\n");
        printf("Your balance: PHP %.2f\n", *balance);

        return;
    }

    printf("\n========== RECEIPT ==========\n");

    printf("Product : %s\n", prod->name);
    printf("Price   : PHP %.2f\n", prod->price);
    printf("Balance : PHP %.2f -> PHP %.2f\n",
           *balance,
           *balance - prod->price);

    printf("=============================\n");

    printf("Confirm purchase? [1] Yes [0] No: ");

    int confirm = getValidInt(0, 1);

    if (confirm == 0) {

        printf("\nPurchase cancelled.\n");
        return;
    }

    *balance -= prod->price;

    prod->stock--;

    strcpy(purchases[*purchaseCount], prod->name);

    prices[*purchaseCount] = prod->price;

    (*purchaseCount)++;

    saveStock(&stores[storeIdx]);

    saveInventory(*balance,
                  purchases,
                  prices,
                  *purchaseCount);

    printf("\n[OK] Successfully purchased!\n");
}

void viewInventory(float balance,
                   char purchases[][MAX_NAME_LEN],
                   float prices[],
                   int count) {

    printHeader("MY INVENTORY");

    printf("\nRemaining Balance : PHP %.2f\n",
           balance);

    printDivider('-', 60);

    if (count == 0) {

        printf("No purchases yet.\n");

    } else {

        printf("%-5s %-30s %-10s\n",
               "No.",
               "Product",
               "Price");

        printDivider('-', 60);

        float total = 0;

        for (int i = 0; i < count; i++) {

            printf("%-5d %-30s PHP %.2f\n",
                   i + 1,
                   purchases[i],
                   prices[i]);

            total += prices[i];
        }

        printDivider('-', 60);

        printf("Total Spent     : PHP %.2f\n", total);

        printf("Items Purchased : %d\n", count);
    }

    printDivider('=', 60);
}

/* ============================================================
 *  MENU
 * ============================================================
 */

void displayMainMenu(void) {

    printf("\n");

    printDivider('*', 60);

    printf("        CANTEEN VENDING MACHINE\n");

    printf("         4 Stores | Dynamic Stocks\n");

    printDivider('*', 60);

    printf("[1] View All Products\n");
    printf("[2] Buy a Product\n");
    printf("[3] View My Inventory\n");
    printf("[0] Exit\n");

    printDivider('*', 60);

    printf("Choose an option: ");
}

int getValidInt(int min, int max) {

    int val;

    char buf[100];

    while (1) {

        if (fgets(buf, sizeof(buf), stdin) != NULL) {

            if (sscanf(buf, "%d", &val) == 1 &&
                val >= min &&
                val <= max) {

                return val;
            }
        }

        printf("[!] Invalid input. Enter (%d-%d): ",
               min,
               max);
    }
}

/* ============================================================
 *  MAIN
 * ============================================================
 */

int main(void) {

    for (int i = 0; i < NUM_STORES; i++) {

        loadStock(&stores[i]);
    }

    float balance;

    char purchases[MAX_INV_LINES][MAX_NAME_LEN];

    float prices[MAX_INV_LINES];

    int purchaseCount;

    loadInventory(&balance,
                  purchases,
                  prices,
                  &purchaseCount);

    int choice;

    do {

        displayMainMenu();

        choice = getValidInt(0, 3);

        switch (choice) {

            case 1:
                viewAllProducts();
                break;

            case 2:
                buyProduct(&balance,
                           purchases,
                           prices,
                           &purchaseCount);
                break;

            case 3:
                viewInventory(balance,
                              purchases,
                              prices,
                              purchaseCount);
                break;

            case 0:

                printf("\nThank you for using the system!\n");
                printf("Goodbye!\n");

                break;
        }

    } while (choice != 0);

    return 0;
}
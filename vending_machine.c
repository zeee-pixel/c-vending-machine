/*
 * ============================================================
 *  VENDING MACHINE SIMULATION - FINAL PROJECT
 *  Group Members:
 *    - Mariejoe  (Store 1: Drinks)
 *    - Jasmine   (Store 2: Chips)
 *    - Aizeah    (Store 3: Ready-to-Eat Meals / Cup Noodles)
 *    - Cyril     (Store 4: Candies)
 *
 *  Description:
 *    A console-based vending machine simulation where a student
 *    starts with PHP 500.00 pocket money and can browse and buy
 *    products from 4 different stores. All stock and inventory
 *    data are read from and written to text files.
 *
 *  Files Used:
 *    - mariejoe_stock.txt  : Drinks stock
 *    - jasmine_stock.txt   : Chips stock
 *    - aizeah_stock.txt    : Cup Noodles stock
 *    - cyril_stock.txt     : Candies stock
 *    - inventory.txt       : Student's purchased items + remaining cash
 *
 *  Compile:  gcc vending_machine.c -o vending_machine
 *  Run:      ./vending_machine
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Constants ─────────────────────────────────────────── */
#define MAX_PRODUCTS   5        /* products per store        */
#define MAX_NAME_LEN   40       /* max chars in product name */
#define NUM_STORES     4        /* total number of stores    */
#define MAX_INV_LINES  100      /* max lines in inventory    */

/* ── Data Structures ────────────────────────────────────── */
typedef struct {
    char name[MAX_NAME_LEN];
    float price;
    int   stock;
} Product;

typedef struct {
    char  storeName[MAX_NAME_LEN];
    char  category[MAX_NAME_LEN];
    char  filename[MAX_NAME_LEN];
    Product items[MAX_PRODUCTS];
} Store;

/* ── Global Store Definitions ───────────────────────────── */
Store stores[NUM_STORES] = {
    { "Mariejoe's Store", "Drinks",              "mariejoe_stock.txt", {} },
    { "Jasmine's Store",  "Chips",               "jasmine_stock.txt",  {} },
    { "Aizeah's Store",   "Ready-to-Eat Meals",  "aizeah_stock.txt",   {} },
    { "Cyril's Store",    "Candies",              "cyril_stock.txt",    {} }
};

/* ── Function Prototypes ─────────────────────────────────── */
void  loadStock(Store *s);
void  saveStock(Store *s);
float loadBalance(void);
void  saveInventory(float balance, char purchases[][MAX_NAME_LEN],
                    float prices[], int count);
void  loadInventory(float *balance, char purchases[][MAX_NAME_LEN],
                    float prices[], int *count);
void  printDivider(char ch, int width);
void  printHeader(const char *title);
void  viewAllProducts(void);
void  viewStoreProducts(int storeIdx);
void  buyProduct(float *balance, char purchases[][MAX_NAME_LEN],
                 float prices[], int *purchaseCount);
void  viewInventory(float balance, char purchases[][MAX_NAME_LEN],
                    float prices[], int count);
void  displayMainMenu(void);
int   getValidInt(int min, int max);

/* ============================================================
 *  FILE HANDLING — STOCK
 * ============================================================ */

/* Load a store's products from its text file.
   Format per line: ProductName Price Stock             */
void loadStock(Store *s) {
    FILE *fp = fopen(s->filename, "r");
    if (fp == NULL) {
        printf("[ERROR] Could not open %s\n", s->filename);
        exit(1);
    }
    int i = 0;
    while (i < MAX_PRODUCTS &&
           fscanf(fp, "%s %f %d",
                  s->items[i].name,
                  &s->items[i].price,
                  &s->items[i].stock) == 3) {
        /* Replace underscores with spaces for display */
        char *p = s->items[i].name;
        while (*p) { if (*p == '_') *p = ' '; p++; }
        i++;
    }
    fclose(fp);
}

/* Save updated stock back to the store's text file.   */
void saveStock(Store *s) {
    FILE *fp = fopen(s->filename, "w");
    if (fp == NULL) {
        printf("[ERROR] Could not save %s\n", s->filename);
        return;
    }
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        /* Replace spaces back with underscores for storage */
        char temp[MAX_NAME_LEN];
        strcpy(temp, s->items[i].name);
        char *p = temp;
        while (*p) { if (*p == ' ') *p = '_'; p++; }
        fprintf(fp, "%s %.2f %d\n", temp,
                s->items[i].price, s->items[i].stock);
    }
    fclose(fp);
}

/* ============================================================
 *  FILE HANDLING — INVENTORY
 * ============================================================ */

/* Read balance from inventory.txt (first line = balance).   */
float loadBalance(void) {
    FILE *fp = fopen("inventory.txt", "r");
    if (fp == NULL) return 500.00f;   /* default starting cash */
    float bal = 500.00f;
    fscanf(fp, "%f", &bal);
    fclose(fp);
    return bal;
}

/* Save balance and all purchased items to inventory.txt.
   Format:
     <balance>
     <ProductName> <Price>        (one per purchase)          */
void saveInventory(float balance, char purchases[][MAX_NAME_LEN],
                   float prices[], int count) {
    FILE *fp = fopen("inventory.txt", "w");
    if (fp == NULL) {
        printf("[ERROR] Could not save inventory.\n");
        return;
    }
    fprintf(fp, "%.2f\n", balance);
    for (int i = 0; i < count; i++) {
        char temp[MAX_NAME_LEN];
        strcpy(temp, purchases[i]);
        char *p = temp;
        while (*p) { if (*p == ' ') *p = '_'; p++; }
        fprintf(fp, "%s %.2f\n", temp, prices[i]);
    }
    fclose(fp);
}

/* Load balance and purchase history from inventory.txt.     */
void loadInventory(float *balance, char purchases[][MAX_NAME_LEN],
                   float prices[], int *count) {
    *balance = 500.00f;
    *count   = 0;
    FILE *fp = fopen("inventory.txt", "r");
    if (fp == NULL) return;
    fscanf(fp, "%f", balance);
    while (*count < MAX_INV_LINES &&
           fscanf(fp, "%s %f",
                  purchases[*count],
                  &prices[*count]) == 2) {
        /* Replace underscores with spaces for display */
        char *p = purchases[*count];
        while (*p) { if (*p == '_') *p = ' '; p++; }
        (*count)++;
    }
    fclose(fp);
}

/* ============================================================
 *  DISPLAY HELPERS
 * ============================================================ */

void printDivider(char ch, int width) {
    for (int i = 0; i < width; i++) putchar(ch);
    putchar('\n');
}

void printHeader(const char *title) {
    printDivider('=', 55);
    printf("  %s\n", title);
    printDivider('=', 55);
}

/* ============================================================
 *  CORE FEATURES
 * ============================================================ */

/* Feature 1 — View all products across all 4 stores.        */
void viewAllProducts(void) {
    printHeader("VENDING MACHINE — ALL STORES");
    for (int s = 0; s < NUM_STORES; s++) {
        printf("\n  [STORE %d] %s — %s\n",
               s + 1, stores[s].storeName, stores[s].category);
        printDivider('-', 55);
        printf("  %-3s  %-26s  %8s  %6s\n",
               "No.", "Product", "Price", "Stock");
        printDivider('-', 55);
        for (int i = 0; i < MAX_PRODUCTS; i++) {
            if (stores[s].items[i].stock > 0) {
                printf("  %-3d  %-26s  PHP%6.2f  %4d pcs\n",
                       i + 1,
                       stores[s].items[i].name,
                       stores[s].items[i].price,
                       stores[s].items[i].stock);
            } else {
                printf("  %-3d  %-26s  %8s  %s\n",
                       i + 1,
                       stores[s].items[i].name,
                       "---", "OUT OF STOCK");
            }
        }
    }
    printDivider('=', 55);
}

/* View products for a single store (used inside buy flow).  */
void viewStoreProducts(int storeIdx) {
    Store *s = &stores[storeIdx];
    printf("\n  [%s — %s]\n", s->storeName, s->category);
    printDivider('-', 55);
    printf("  %-3s  %-26s  %8s  %6s\n",
           "No.", "Product", "Price", "Stock");
    printDivider('-', 55);
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (s->items[i].stock > 0) {
            printf("  %-3d  %-26s  PHP%6.2f  %4d pcs\n",
                   i + 1,
                   s->items[i].name,
                   s->items[i].price,
                   s->items[i].stock);
        } else {
            printf("  %-3d  %-26s  %8s  %s\n",
                   i + 1,
                   s->items[i].name,
                   "---", "OUT OF STOCK");
        }
    }
    printDivider('-', 55);
}

/* Feature 2 — Buy a product.
   Steps: pick store → pick product → confirm → deduct.      */
void buyProduct(float *balance,
                char   purchases[][MAX_NAME_LEN],
                float  prices[],
                int   *purchaseCount) {

    printHeader("BUY A PRODUCT");
    printf("\n  Select a Store:\n");
    for (int i = 0; i < NUM_STORES; i++)
        printf("    [%d] %s (%s)\n",
               i + 1, stores[i].storeName, stores[i].category);
    printf("    [0] Cancel\n\n");
    printf("  Enter store number: ");
    int sc = getValidInt(0, NUM_STORES);
    if (sc == 0) { printf("\n  Purchase cancelled.\n"); return; }

    int storeIdx = sc - 1;
    viewStoreProducts(storeIdx);

    printf("  Select product number (0 to cancel): ");
    int pc = getValidInt(0, MAX_PRODUCTS);
    if (pc == 0) { printf("\n  Purchase cancelled.\n"); return; }

    int prodIdx = pc - 1;
    Product *prod = &stores[storeIdx].items[prodIdx];

    /* Stock check */
    if (prod->stock <= 0) {
        printf("\n  [!] Sorry, %s is out of stock!\n", prod->name);
        return;
    }

    /* Balance check */
    if (*balance < prod->price) {
        printf("\n  [!] Insufficient balance!\n");
        printf("      Item costs PHP %.2f | Your balance: PHP %.2f\n",
               prod->price, *balance);
        return;
    }

    /* Confirm purchase */
    printf("\n  Confirm purchase:\n");
    printf("    Product : %s\n", prod->name);
    printf("    Price   : PHP %.2f\n", prod->price);
    printf("    Balance : PHP %.2f → PHP %.2f\n",
           *balance, *balance - prod->price);
    printf("\n  Proceed? [1] Yes  [0] No: ");
    int confirm = getValidInt(0, 1);
    if (confirm == 0) { printf("\n  Purchase cancelled.\n"); return; }

    /* Process transaction */
    *balance         -= prod->price;
    prod->stock      -= 1;

    /* Record in purchase history */
    if (*purchaseCount < MAX_INV_LINES) {
        strcpy(purchases[*purchaseCount], prod->name);
        prices[*purchaseCount] = prod->price;
        (*purchaseCount)++;
    }

    /* Persist changes to files */
    saveStock(&stores[storeIdx]);
    saveInventory(*balance, purchases, prices, *purchaseCount);

    printf("\n  [OK] Purchased %s for PHP %.2f!\n", prod->name, prod->price);
    printf("       Remaining balance: PHP %.2f\n", *balance);
}

/* Feature 3 — View student's purchase history + balance.   */
void viewInventory(float balance,
                   char  purchases[][MAX_NAME_LEN],
                   float prices[],
                   int   count) {

    printHeader("MY INVENTORY");
    printf("\n  Remaining Balance : PHP %.2f\n", balance);
    printDivider('-', 55);

    if (count == 0) {
        printf("  No purchases yet.\n");
    } else {
        printf("  %-3s  %-30s  %8s\n", "No.", "Product", "Price");
        printDivider('-', 55);
        float total = 0;
        for (int i = 0; i < count; i++) {
            printf("  %-3d  %-30s  PHP%6.2f\n",
                   i + 1, purchases[i], prices[i]);
            total += prices[i];
        }
        printDivider('-', 55);
        printf("  Total Spent       : PHP %.2f\n", total);
        printf("  Items Purchased   : %d\n", count);
    }
    printDivider('=', 55);
}

/* ============================================================
 *  MENU & INPUT HELPERS
 * ============================================================ */

void displayMainMenu(void) {
    printf("\n");
    printDivider('*', 55);
    printf("  CANTEEN VENDING MACHINE\n");
    printf("  4 Stores | 20 Products\n");
    printDivider('*', 55);
    printf("  [1] View All Products\n");
    printf("  [2] Buy a Product\n");
    printf("  [3] View My Inventory\n");
    printf("  [0] Exit\n");
    printDivider('*', 55);
    printf("  Choose an option: ");
}

/* Read a valid integer in [min, max], re-prompt on bad input. */
int getValidInt(int min, int max) {
    int val;
    char buf[50];
    while (1) {
        if (fgets(buf, sizeof(buf), stdin) != NULL) {
            if (sscanf(buf, "%d", &val) == 1 &&
                val >= min && val <= max) {
                return val;
            }
        }
        printf("  [!] Invalid input. Enter a number (%d-%d): ", min, max);
    }
}

/* ============================================================
 *  MAIN
 * ============================================================ */
int main(void) {

    /* ── Load all store stocks from files ── */
    for (int i = 0; i < NUM_STORES; i++)
        loadStock(&stores[i]);

    /* ── Load student balance and purchase history ── */
    float balance = 500.00f;
    char  purchases[MAX_INV_LINES][MAX_NAME_LEN];
    float prices[MAX_INV_LINES];
    int   purchaseCount = 0;
    loadInventory(&balance, purchases, prices, &purchaseCount);

    /* ── Main looppp ── */
    int choice;
    do {
        displayMainMenu();
        choice = getValidInt(0, 3);

        switch (choice) {
            case 1:
                viewAllProducts();
                break;
            case 2:
                buyProduct(&balance, purchases, prices, &purchaseCount);
                break;
            case 3:
                viewInventory(balance, purchases, prices, purchaseCount);
                break;
            case 0:
                printf("\n  Thank you for using the Canteen Vending Machine!\n");
                printf("  Goodbye!\n\n");
                break;
        }

    } while (choice != 0);

    return 0;
}

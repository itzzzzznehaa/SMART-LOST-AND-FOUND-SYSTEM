#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_NAME "records.dat"

// Structure for Item
struct Item
{
    int id;
    char name[50];
    char category[30];
    char location[50];
    char description[100];
    struct Item *next;
};

// Structure for Stack
struct Stack
{
    struct Item item;
    int wasLost;
    struct Stack *next;
};

// Structure for Queue
struct Queue
{
    int id;
    char name[50];
    struct Queue *next;
};

// Structure for File Storage
struct ItemData
{
    int id;
    char name[50];
    char category[30];
    char location[50];
    char description[100];
    char status[10];
};

// Global pointers
struct Item *lostHead = NULL;
struct Item *foundHead = NULL;

struct Stack *top = NULL;

struct Queue *front = NULL;
struct Queue *rear = NULL;

int nextId = 101;

// Function declarations
void reportLostItem();
void reportFoundItem();
void reportItem(struct Item **head, char status[]);
void displayItems();
void displayList(struct Item *head, char status[]);
void searchItem();
void searchList(struct Item *head, char keyword[], char status[], int *found);
void matchItems();
void deleteItem();
void deleteFromList(struct Item **head, int id);
void push(struct Item item, int wasLost);
void undoDelete();
void enqueue(int id, char name[]);
void displayQueue();
void insertItem(struct Item **head, struct Item *newItem);
void saveData();
void loadData();
void saveList(FILE *fp, struct Item *head, char status[]);
int readInteger();
void readString(char str[], int size);
int equalIgnoreCase(char a[], char b[]);
int containsIgnoreCase(char text[], char keyword[]);
void freeMemory();


// Read string input
void readString(char str[], int size)
{
    if(fgets(str, size, stdin) == NULL)
    {
        str[0] = '\0';
        return;
    }

    if(strchr(str, '\n') == NULL)
    {
        int ch;
        while((ch = getchar()) != '\n' && ch != EOF);
    }

    str[strcspn(str, "\n")] = '\0';
}


// Read integer input
int readInteger()
{
    char input[30];
    char *end;
    long value;

    readString(input, sizeof(input));

    value = strtol(input, &end, 10);

    if(end == input || *end != '\0' || value < 0 ||
       value > 2147483647L)
        return -1;

    return (int)value;
}


// Compare strings without case sensitivity
int equalIgnoreCase(char a[], char b[])
{
    while(*a && *b)
    {
        if(tolower((unsigned char)*a) !=
           tolower((unsigned char)*b))
            return 0;

        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}


// Search substring without case sensitivity
int containsIgnoreCase(char text[], char keyword[])
{
    int i, j;

    if(keyword[0] == '\0')
        return 0;

    for(i = 0; text[i] != '\0'; i++)
    {
        for(j = 0; keyword[j] != '\0'; j++)
        {
            if(text[i+j] == '\0' ||
               tolower((unsigned char)text[i+j]) !=
               tolower((unsigned char)keyword[j]))
                break;
        }

        if(keyword[j] == '\0')
            return 1;
    }

    return 0;
}


// Insert item into linked list
void insertItem(struct Item **head, struct Item *newItem)
{
    newItem->next = NULL;

    if(*head == NULL)
    {
        *head = newItem;
        return;
    }

    struct Item *temp = *head;

    while(temp->next != NULL)
        temp = temp->next;

    temp->next = newItem;
}


// Report lost or found item
void reportItem(struct Item **head, char status[])
{
    struct Item *newItem = malloc(sizeof(struct Item));

    if(newItem == NULL)
    {
        printf("\nMemory allocation failed!");
        return;
    }

    newItem->id = nextId++;

    printf("\nEnter Item Name: ");
    readString(newItem->name, sizeof(newItem->name));

    if(newItem->name[0] == '\0')
    {
        printf("\nItem name cannot be empty!");
        free(newItem);
        nextId--;
        return;
    }

    printf("Enter Category: ");
    readString(newItem->category, sizeof(newItem->category));

    printf("Enter Location: ");
    readString(newItem->location, sizeof(newItem->location));

    printf("Enter Description: ");
    readString(newItem->description, sizeof(newItem->description));

    newItem->next = NULL;

    insertItem(head, newItem);

    enqueue(newItem->id, newItem->name);

    saveData();

    printf("\n%s item registered successfully!", status);
    printf("\nYour Item ID is: %d\n", newItem->id);
}


void reportLostItem()
{
    reportItem(&lostHead, "Lost");
}


void reportFoundItem()
{
    reportItem(&foundHead, "Found");
}


// Display linked list
void displayList(struct Item *head, char status[])
{
    struct Item *temp = head;

    if(temp == NULL)
    {
        printf("\nNo %s items registered.\n", status);
        return;
    }

    while(temp != NULL)
    {
        printf("\n--------------------------------");
        printf("\nItem ID: %d", temp->id);
        printf("\nName: %s", temp->name);
        printf("\nCategory: %s", temp->category);
        printf("\nLocation: %s", temp->location);
        printf("\nDescription: %s", temp->description);
        printf("\nStatus: %s", status);

        temp = temp->next;
    }

    printf("\n--------------------------------\n");
}


// Display all items
void displayItems()
{
    printf("\n========== LOST ITEMS ==========");
    displayList(lostHead, "Lost");

    printf("\n========== FOUND ITEMS ==========");
    displayList(foundHead, "Found");
}


// Search linked list
void searchList(struct Item *head, char keyword[],
                char status[], int *found)
{
    struct Item *temp = head;

    while(temp != NULL)
    {
        if(containsIgnoreCase(temp->name, keyword) ||
           containsIgnoreCase(temp->category, keyword))
        {
            printf("\n-----------------------------");
            printf("\nID: %d", temp->id);
            printf("\nName: %s", temp->name);
            printf("\nCategory: %s", temp->category);
            printf("\nLocation: %s", temp->location);
            printf("\nStatus: %s\n", status);

            *found = 1;
        }

        temp = temp->next;
    }
}


// Search item
void searchItem()
{
    char keyword[50];
    int found = 0;

    printf("\nEnter item name or category: ");
    readString(keyword, sizeof(keyword));

    if(keyword[0] == '\0')
    {
        printf("\nSearch keyword cannot be empty!");
        return;
    }

    searchList(lostHead, keyword, "Lost", &found);
    searchList(foundHead, keyword, "Found", &found);

    if(!found)
        printf("\nNo matching items found.\n");
}


// Match lost and found items
void matchItems()
{
    struct Item *lost = lostHead;
    struct Item *found;
    int matches = 0;

    printf("\n========== POSSIBLE MATCHES ==========\n");

    while(lost != NULL)
    {
        found = foundHead;

        while(found != NULL)
        {
            if(equalIgnoreCase(lost->name, found->name) &&
               equalIgnoreCase(lost->category, found->category))
            {
                printf("\nPossible Match Found!");
                printf("\nLost Item ID: %d", lost->id);
                printf("\nFound Item ID: %d", found->id);
                printf("\nItem Name: %s", lost->name);
                printf("\nLost Location: %s", lost->location);
                printf("\nFound Location: %s", found->location);
                printf("\n--------------------------------");

                matches++;
            }

            found = found->next;
        }

        lost = lost->next;
    }

    if(matches == 0)
        printf("\nNo possible matches found.\n");
}


// Push deleted item into stack
void push(struct Item item, int wasLost)
{
    struct Stack *newNode = malloc(sizeof(struct Stack));

    if(newNode == NULL)
    {
        printf("\nUnable to save item for undo!");
        return;
    }

    newNode->item = item;
    newNode->item.next = NULL;
    newNode->wasLost = wasLost;

    newNode->next = top;
    top = newNode;
}


// Delete item from linked list
void deleteFromList(struct Item **head, int id)
{
    struct Item *temp = *head;
    struct Item *prev = NULL;

    while(temp != NULL)
    {
        if(temp->id == id)
        {
            if(prev == NULL)
                *head = temp->next;
            else
                prev->next = temp->next;

            temp->next = NULL;

            push(*temp, head == &lostHead);

            free(temp);

            printf("\nItem deleted successfully!");

            saveData();
            return;
        }

        prev = temp;
        temp = temp->next;
    }
}


// Delete item
void deleteItem()
{
    int id;

    printf("\nEnter Item ID to delete: ");
    id = readInteger();

    if(id < 0)
    {
        printf("\nInvalid ID!");
        return;
    }

    struct Item *temp = lostHead;

    while(temp != NULL)
    {
        if(temp->id == id)
        {
            deleteFromList(&lostHead, id);
            return;
        }

        temp = temp->next;
    }

    temp = foundHead;

    while(temp != NULL)
    {
        if(temp->id == id)
        {
            deleteFromList(&foundHead, id);
            return;
        }

        temp = temp->next;
    }

    printf("\nItem ID not found!");
}


// Undo last deletion
void undoDelete()
{
    if(top == NULL)
    {
        printf("\nNothing to undo!");
        return;
    }

    struct Stack *temp = top;

    struct Item *restored = malloc(sizeof(struct Item));

    if(restored == NULL)
    {
        printf("\nMemory allocation failed!");
        return;
    }

    *restored = top->item;
    restored->next = NULL;

    int wasLost = top->wasLost;

    top = top->next;

    free(temp);

    if(wasLost)
        insertItem(&lostHead, restored);
    else
        insertItem(&foundHead, restored);

    printf("\nItem restored successfully!");

    saveData();
}


// Queue: Add recent report
void enqueue(int id, char name[])
{
    struct Queue *newNode = malloc(sizeof(struct Queue));

    if(newNode == NULL)
        return;

    newNode->id = id;
    strcpy(newNode->name, name);
    newNode->next = NULL;

    if(rear == NULL)
        front = rear = newNode;
    else
    {
        rear->next = newNode;
        rear = newNode;
    }
}


// Display recent reports
void displayQueue()
{
    struct Queue *temp = front;

    if(front == NULL)
    {
        printf("\nNo recent reports available.");
        return;
    }

    printf("\n========== RECENT REPORTS ==========\n");

    while(temp != NULL)
    {
        printf("\nItem ID: %d", temp->id);
        printf("\nItem Name: %s\n", temp->name);

        temp = temp->next;
    }
}


// Save linked list into file
void saveList(FILE *fp, struct Item *head, char status[])
{
    struct ItemData data;

    while(head != NULL)
    {
        data.id = head->id;

        strcpy(data.name, head->name);
        strcpy(data.category, head->category);
        strcpy(data.location, head->location);
        strcpy(data.description, head->description);
        strcpy(data.status, status);

        fwrite(&data, sizeof(data), 1, fp);

        head = head->next;
    }
}


// Save all records
void saveData()
{
    FILE *fp = fopen(FILE_NAME, "wb");

    if(fp == NULL)
    {
        printf("\nUnable to save records!");
        return;
    }

    saveList(fp, lostHead, "Lost");
    saveList(fp, foundHead, "Found");

    fclose(fp);
}


// Load records from file
void loadData()
{
    FILE *fp = fopen(FILE_NAME, "rb");

    if(fp == NULL)
        return;

    struct ItemData data;

    while(fread(&data, sizeof(data), 1, fp) == 1)
    {
        if(data.id <= 0 ||
           (strcmp(data.status, "Lost") != 0 &&
            strcmp(data.status, "Found") != 0))
            continue;

        struct Item *newItem = malloc(sizeof(struct Item));

        if(newItem == NULL)
            break;

        newItem->id = data.id;

        strcpy(newItem->name, data.name);
        strcpy(newItem->category, data.category);
        strcpy(newItem->location, data.location);
        strcpy(newItem->description, data.description);

        newItem->next = NULL;

        if(strcmp(data.status, "Lost") == 0)
            insertItem(&lostHead, newItem);
        else
            insertItem(&foundHead, newItem);

        if(data.id >= nextId)
            nextId = data.id + 1;
    }

    fclose(fp);
}


// Free allocated memory
void freeMemory()
{
    struct Item *temp;
    struct Queue *qtemp;
    struct Stack *stemp;

    while(lostHead != NULL)
    {
        temp = lostHead;
        lostHead = lostHead->next;
        free(temp);
    }

    while(foundHead != NULL)
    {
        temp = foundHead;
        foundHead = foundHead->next;
        free(temp);
    }

    while(front != NULL)
    {
        qtemp = front;
        front = front->next;
        free(qtemp);
    }

    while(top != NULL)
    {
        stemp = top;
        top = top->next;
        free(stemp);
    }
}


// Main function
int main()
{
    int choice;

    loadData();

    do
    {
        printf("\n\n====================================");
        printf("\n       SMART LOST & FOUND SYSTEM");
        printf("\n====================================");

        printf("\n1. Report Lost Item");
        printf("\n2. Report Found Item");
        printf("\n3. Search Item");
        printf("\n4. Match Lost & Found Items");
        printf("\n5. Display All Items");
        printf("\n6. Delete Item");
        printf("\n7. View Recent Reports");
        printf("\n8. Undo Last Deletion");
        printf("\n9. Exit");

        printf("\n\nEnter your choice: ");

        choice = readInteger();

        switch(choice)
        {
            case 1:
                reportLostItem();
                break;

            case 2:
                reportFoundItem();
                break;

            case 3:
                searchItem();
                break;

            case 4:
                matchItems();
                break;

            case 5:
                displayItems();
                break;

            case 6:
                deleteItem();
                break;

            case 7:
                displayQueue();
                break;

            case 8:
                undoDelete();
                break;

            case 9:
                saveData();
                printf("\nThank you for using our system!");
                break;

            default:
                printf("\nInvalid choice! Please try again.");
        }

    } while(choice != 9);

    freeMemory();

    return 0;
}
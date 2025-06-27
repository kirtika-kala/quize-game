#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_WORDS 100
#define MAX_LEN 50

// ------ Trie ----
typedef struct Trie {
    struct Trie* kids[26];
    int end;
} Trie;

Trie* makeTrie() {
    Trie* t = (Trie*)malloc(sizeof(Trie));
    t->end = 0;
    for (int i = 0; i < 26; i++)
        t->kids[i] = NULL;
    return t;
}

void addWord(Trie* root, const char* word) {
    Trie* cur = root;
    for (int i = 0; word[i]; i++) {
        int idx = tolower(word[i]) - 'a';
        if (!cur->kids[idx])
            cur->kids[idx] = makeTrie();
        cur = cur->kids[idx];
    }
    cur->end = 1;
}

// --------- Heap ----------
typedef struct {
    char word[MAX_LEN];
    int score;
} Word;

void swap(Word* a, Word* b) {
    Word temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(Word arr[], int n, int i) {
    int big = i, l = 2*i+1, r = 2*i+2;
    if (l < n && arr[l].score > arr[big].score) big = l;
    if (r < n && arr[r].score > arr[big].score) big = r;
    if (big != i) {
        swap(&arr[i], &arr[big]);
        heapify(arr, n, big);
    }
}

Word popMax(Word heap[], int* n) {
    Word top = heap[0];
    heap[0] = heap[--(*n)];
    heapify(heap, *n, 0);
    return top;
}

//----------- Stack -------------
typedef struct Stack {
    char ch;
    struct Stack* next;
} Stack;

void push(Stack** top, char ch) {
    Stack* new = (Stack*)malloc(sizeof(Stack));
    new->ch = ch;
    new->next = *top;
    *top = new;
}

void showStack(Stack* top) {
    printf("Wrong: ");
    while (top) {
        printf("%c ", top->ch);
        top = top->next;
    }
    printf("\n");
}

// --------- Linked List ----------
typedef struct Node {
    char ch;
    struct Node* next;
} Node;

void add(Node** head, char ch) {
    Node* new = (Node*)malloc(sizeof(Node));
    new->ch = ch;
    new->next = *head;
    *head = new;
}

int guessed(Node* head, char ch) {
    while (head) {
        if (head->ch == ch) return 1;
        head = head->next;
    }
    return 0;
}

void freeList(Node* head) {
    while (head) {
        Node* tmp = head;
        head = head->next;
        free(tmp);
    }
}

// ------- File Load --------
int loadWords(char words[][MAX_LEN], Trie* root, Word heap[]) {
    FILE* f = fopen("words.txt", "r");
    if (!f) {
        printf("Error: Can't open words.txt\n");
        return 0;
    }

    int count = 0;
    while (fgets(words[count], MAX_LEN, f) && count < MAX_WORDS) {
        words[count][strcspn(words[count], "\n")] = '\0';
        addWord(root, words[count]);
        strcpy(heap[count].word, words[count]);
        heap[count].score = strlen(words[count]);  // longer word = higher priority
        count++;
    }

    fclose(f);
    return count;
}

// ------ Difficulty ------
int getLevel() {
    int lvl;
    printf("Choose level (1=Easy, 2=Med, 3=Hard): ");
    scanf("%d", &lvl);

    if (lvl == 1) return 8;
    else if (lvl == 2) return 6;
    else if (lvl == 3) return 4;
    else {
        printf("Invalid. Default: Medium\n");
        return 6;
    }
}

// ------- Game --------
void showWord(const char* word, Node* guess) {
    for (int i = 0; word[i]; i++) {
        if (guessed(guess, word[i]))
            printf("%c ", word[i]);
        else
            printf("_ ");
    }
    printf("\n");
}

int isDone(const char* word, Node* guess) {
    for (int i = 0; word[i]; i++) {
        if (!guessed(guess, word[i]))
            return 0;
    }
    return 1;
}

void playGame() {
    char words[MAX_WORDS][MAX_LEN];
    Trie* root = makeTrie();
    Word heap[MAX_WORDS];

    int total = loadWords(words, root, heap);
    if (total == 0) {
        printf("No words loaded.\n");
        return;
    }

    int pick = rand() % total;
    Word secret = heap[pick];

    int lives = getLevel();
    Node* guess = NULL;
    Stack* wrong = NULL;

    printf("    Word Quest    \n");

    while (lives > 0) {
        printf("\nWord: ");
        showWord(secret.word, guess);
        printf("Lives: %d\n", lives);
        printf("Your guess: ");

        char ch;
        scanf(" %c", &ch);
        ch = tolower(ch);

        if (!isalpha(ch)) {
            printf("Only letters allowed.\n");
            continue;
        }

        if (guessed(guess, ch)) {
            printf("Already guessed '%c'.\n", ch);
            continue;
        }

        add(&guess, ch);

        if (strchr(secret.word, ch)) {
            printf("Correct!\n");
        } else {
            printf("Wrong!\n");
            lives--;
            push(&wrong, ch);
        }

        showStack(wrong);

        if (isDone(secret.word, guess)) {
            printf("\n You won! Word: %s\n", secret.word);
            freeList(guess);
            return;
        }
    }

    printf("\n You lost! Word was: %s\n", secret.word);
    freeList(guess);
}

// ------Main------
int main() {
    srand(time(0));
    char again;

    do {
        playGame();
        printf("Play again? (y/n): ");
        scanf(" %c", &again);
        again = tolower(again);
    } while (again == 'y');

    printf("Bye! \n");
    return 0;
}

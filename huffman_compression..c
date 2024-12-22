#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HEIGHT 100
#define MAX_CHAR 256

typedef struct HuffmanNode {
    char data;
    unsigned freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    HuffmanNode** array;
} MinHeap;

HuffmanNode* createNode(char data, unsigned freq) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    node->data = data;
    node->freq = freq;
    node->left = node->right = NULL;
    return node;
}

MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (HuffmanNode**)malloc(minHeap->capacity * sizeof(HuffmanNode*));
    return minHeap;
}

void swapNodes(HuffmanNode** a, HuffmanNode** b) {
    HuffmanNode* temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapNodes(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

HuffmanNode* extractMin(MinHeap* minHeap) {
    HuffmanNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, HuffmanNode* node) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

MinHeap* buildMinHeap(char data[], int freq[], int size) {
    MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = createNode(data[i], freq[i]);

    minHeap->size = size;
    for (int i = (minHeap->size - 2) / 2; i >= 0; --i)
        minHeapify(minHeap, i);

    return minHeap;
}

HuffmanNode* buildHuffmanTree(char data[], int freq[], int size) {
    HuffmanNode *left, *right, *top;
    MinHeap* minHeap = buildMinHeap(data, freq, size);

    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = createNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

void generateCodes(HuffmanNode* root, char* code, int top, char codes[MAX_CHAR][MAX_TREE_HEIGHT]) {
    if (root->left) {
        code[top] = '0';
        generateCodes(root->left, code, top + 1, codes);
    }

    if (root->right) {
        code[top] = '1';
        generateCodes(root->right, code, top + 1, codes);
    }

    if (!root->left && !root->right) {
        code[top] = '\0';
        strcpy(codes[(unsigned char)root->data], code);
    }
}

void calculateFrequency(const char* text, char* data, int* freq, int* uniqueChars) {
    int count[MAX_CHAR] = {0};
    for (int i = 0; text[i]; i++)
        count[(unsigned char)text[i]]++;

    *uniqueChars = 0;
    for (int i = 0; i < MAX_CHAR; i++) {
        if (count[i]) {
            data[*uniqueChars] = (char)i;
            freq[*uniqueChars] = count[i];
            (*uniqueChars)++;
        }
    }
}

int calculateCompressedSize(const char* text, char codes[MAX_CHAR][MAX_TREE_HEIGHT]) {
    int size = 0;
    for (int i = 0; text[i]; i++)
        size += strlen(codes[(unsigned char)text[i]]);
    return size;
}

void huffmanCompression(const char* text) {
    char data[MAX_CHAR];
    int freq[MAX_CHAR];
    int uniqueChars;

    calculateFrequency(text, data, freq, &uniqueChars);
    HuffmanNode* root = buildHuffmanTree(data, freq, uniqueChars);

    char codes[MAX_CHAR][MAX_TREE_HEIGHT] = {0};
    char code[MAX_TREE_HEIGHT];
    generateCodes(root, code, 0, codes);

    int originalSize = strlen(text) * 8; // 1 char = 8 bits
    int compressedSize = calculateCompressedSize(text, codes);

    printf("Original size: %d bits\n", originalSize);
    printf("Compressed size: %d bits\n", compressedSize);
}

int main() {
    char text[1000];
    printf("Enter text to compress: ");
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = '\0'; // Remove newline character

    huffmanCompression(text);
    return 0;
}

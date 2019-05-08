#include <iostream>
#include <vector>

#define PARENT(i) (i/2)

using namespace std;

vector<int> heap;
int heapSize;

void swap(int *a, int *b) {
    int* aux = a;
    a = b;
    b = aux;
}

void initHeap(int size) {
    heapSize = 0;
    heap.resize(size);
}

void printHeap() {
    printf("Heap Size = %d\n\t", heapSize);
    for(int i = 0; i < heapSize; i++)
        printf("%d ", heap[i]);
    printf("\n");
}

void siftUp(int i) {
    int j = PARENT(i);
    if (j >= 0 && heap[j] > heap[i]) {
        swap(heap[j], heap[i]);
        siftUp(j);
    }
}

void insert(int key) {
    heapSize++;
    if ((unsigned int) heapSize == heap.size()) {
        heap.resize(2*heapSize);
    }
    int i = heapSize-1;
    heap[i] = key;
    siftUp(i);
}

int main() {
    initHeap(5);
    insert(2);
    insert(7);
    insert(6);
    insert(1);
    insert(20);
    insert(3);
    insert(5);
    insert(4);
    insert(29);
    insert(30);
    insert(45);
    insert(17);
    insert(15);
    
    printHeap();

    return 1;
}
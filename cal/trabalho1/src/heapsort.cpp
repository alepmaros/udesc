// Alexandre Maros & Nadyan S. Pscheidt
// CAL - Trabalho 1

#include "heapsort.h"

#include <algorithm>

void heapify(int arr[], int n, int i)
{
    int largest = i; 
    int l = 2*i + 1;
    int r = 2*i + 2; 
 
    if (l < n && arr[l] > arr[largest])
        largest = l;

    if (r < n && arr[r] > arr[largest])
        largest = r;

    if (largest != i) {
        std::swap(arr[i], arr[largest]);

        heapify(arr, n, largest);
    }
}
 
// main function to do heap sort
void heapSort(int arr[], int n)
{
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);
 
    for (int i=n-1; i>=0; i--) {
        std::swap(arr[0], arr[i]);

        heapify(arr, i, 0);
    }
}
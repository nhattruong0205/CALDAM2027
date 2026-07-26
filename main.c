#include "function.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main()
{
    int n = 4;
    int arr[n];

    initialize_identity_permutation(arr, n);

    int r = 16;
    unrank1(n, r, arr);

    printf("unrank(r):  ");
    print_array(arr, n);
    for (int i = 4; i < 11; i++)
    {
        ComputeTDistanceFromIdentity(i);
    }

    return 0;
}
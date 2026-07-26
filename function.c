#include "function.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_N 20

// Helper functions
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void compute_inverse(int pi[], int pi_inv[], int n)
{
    for (int i = 0; i < n; i++)
    {
        pi_inv[pi[i]] = i;
    }
}

void print_array(int arr[], int n)
{
    printf("[");
    for (int i = 0; i < n; i++)
    {
        if (i > 0)
        {
            printf(" ");
        }
        printf("%d", arr[i] + 1);
    }
    printf("] \n");
}

// =============== Saving files function ======================
void save_D_to_file(const char *filename, int *D, long long size)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        printf("Error: cannot open %s for writing.\n", filename);
        return;
    }
    for (long long i = 0; i < size; i++)
    {
        fprintf(fp, "%d\n", D[i]);
    }
    fclose(fp);
    printf("Saved D array to %s (%lld elements)\n", filename, size);
}

void initialize_identity_permutation(int *pid, int n)
{
    for (int i = 0; i < n; i++)
    {
        pid[i] = i;
    }
}

void transposition(const int *src, int *dst, int n, int i, int j, int k)
{
    int idx = 0;

    // 1. Prefix: [0..i-1]
    for (int x = 0; x < i; ++x)
        dst[idx++] = src[x];

    // 2. Block: [j..k]
    for (int x = j; x <= k; ++x)
        dst[idx++] = src[x];

    // 3. Middle: [i..j-1]
    for (int x = i; x < j; ++x)
        dst[idx++] = src[x];

    // 4. Suffix: [k+1..n-1]
    for (int x = k + 1; x < n; ++x)
        dst[idx++] = src[x];
}

// Original recursive rank1 function: computes the lexicographic rank of a permutation
int rank1(int n, int pi[], int pi_inv[])
{
    if (n == 1)
        return 0;

    int s = pi[n - 1];

    swap(&pi[n - 1], &pi[pi_inv[n - 1]]);
    swap(&pi_inv[s], &pi_inv[n - 1]);

    return s + n * rank1(n - 1, pi, pi_inv);
}

int rank_safe(int n, const int src[], int *inv_buf)
{
    int tmp[MAX_N], tmp_inv[MAX_N];
    memcpy(tmp, src, n * sizeof(int));         // work on a copy
    memcpy(tmp_inv, inv_buf, n * sizeof(int)); // inv must start correct
    return rank1(n, tmp, tmp_inv);             // rank1 can now swap freely
}

// Original recursive unrank1: Builds a permutation from a given rank
void unrank1(int n, int r, int pi[])
{
    if (n > 0)
    {
        swap(&pi[n - 1], &pi[r % n]);
        unrank1(n - 1, r / n, pi);
    }
}

//================== Compute distance array ====================
// BFS from the identity permutation over single-block transpositions.
// D[r] is the minimum number of transpositions needed to reach the
// permutation with rank r (rank1/unrank1) from the identity; D has n! entries.
int *ComputeTDistanceFromIdentity(int n)
{
    long long fact = 1;
    for (int x = 2; x <= n; x++)
        fact *= x;

    int *D = malloc(fact * sizeof(int));
    bool *visited = calloc(fact, sizeof(bool));
    int *queue = malloc(fact * sizeof(int));
    long long q_head = 0, q_tail = 0;

    int *pi = malloc(n * sizeof(int));
    int *pi_inv = malloc(n * sizeof(int));
    initialize_identity_permutation(pi, n);
    compute_inverse(pi, pi_inv, n);

    int start = rank_safe(n, pi, pi_inv);
    D[start] = 0;
    visited[start] = true;
    queue[q_tail++] = start;

    int *result = malloc(n * sizeof(int));
    int *tmp = malloc(n * sizeof(int));
    int *tmp_inv = malloc(n * sizeof(int));

    while (q_head < q_tail)
    {
        int current_rank = queue[q_head++];

        initialize_identity_permutation(result, n);
        unrank1(n, current_rank, result);

        for (int i = 0; i < n; ++i)
            for (int j = i + 1; j < n; ++j)
                for (int k = j; k < n; ++k)
                {
                    transposition(result, tmp, n, i, j, k);
                    compute_inverse(tmp, tmp_inv, n);
                    int rank_tmp = rank_safe(n, tmp, tmp_inv);

                    if (!visited[rank_tmp])
                    {
                        visited[rank_tmp] = true;
                        D[rank_tmp] = D[current_rank] + 1;
                        queue[q_tail++] = rank_tmp;
                    }
                }
    }

    // Comment after
    char filename[512];
    snprintf(filename, sizeof(filename),
             "/Users/nhattruong/Documents/Summer 2026/CALDAM2027/dArray/distances_n%d.txt", n);
    save_D_to_file(filename, D, fact);

    return D;
}

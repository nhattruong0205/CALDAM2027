#include "function.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Helper functions
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void compute_inverse(const int pi[], int pi_inv[], int n)
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

// Path of the saved distance file for a given n, matching the filename
// ComputeTDistanceFromIdentity writes to.
void d_array_path(char *buf, size_t bufsize, int n)
{
    snprintf(buf, bufsize,
             "/Users/nhattruong/Documents/Summer 2026/CALDAM2027/dArray/distances_n%d.txt", n);
}

// Reads a single value D[r] from a previously saved distance file (one int
// per line, as written by save_D_to_file), without loading the whole array
// or recomputing the BFS.
int read_D_from_file(const char *filename, long long r)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("Error: cannot open %s for reading.\n", filename);
        return -1;
    }

    int value = -1;
    for (long long i = 0; i <= r; i++)
        fscanf(fp, "%d", &value);

    fclose(fp);
    return value;
}

// Loads a full saved distance array (one int per line, as written by
// save_D_to_file) into memory in a single pass. Returns NULL if the file
// cannot be opened; caller owns the returned buffer (free it).
int *load_D_from_file(const char *filename, long long size)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("Error: cannot open %s for reading.\n", filename);
        return NULL;
    }

    int *D = malloc(size * sizeof(int));
    for (long long i = 0; i < size; i++)
        fscanf(fp, "%d", &D[i]);

    fclose(fp);
    return D;
}

// Looks up the actual transposition distance of a 0-indexed permutation pi
// (size n) from identity, by ranking pi and reading D[rank] out of the
// saved dArray/distances_n{n}.txt file (must already exist).
int actual_distance_from_identity(const int *pi, int n)
{
    int pi_inv[MAX_N];
    compute_inverse(pi, pi_inv, n);
    int r = rank_safe(n, pi, pi_inv);

    char filename[512];
    d_array_path(filename, sizeof(filename), n);
    return read_D_from_file(filename, r);
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

// =============== Breakpoint graph (black/gray edges, cycles) ===============

// Extends a 0-indexed permutation of size n into the "hat" permutation of
// size n+2 by prepending 0 and appending n+1, using 1-indexed values for
// the original entries (e.g. internal [1 3 2 4 0] -> [0 2 4 3 5 1 6]).
void extend_permutation(const int *src, int *dst, int n)
{
    dst[0] = 0;
    for (int i = 0; i < n; i++)
        dst[i + 1] = src[i] + 1;
    dst[n + 1] = n + 1;
}

// Builds the black edges (ext[i], ext[i-1]) and the gray edges (i-1, i) of
// the breakpoint graph for the extended permutation ext (length n+2).
// Both black and gray have n+1 edges.
void build_edges(const int *ext, int n, Edge *black, Edge *gray)
{
    for (int i = 1; i <= n + 1; i++)
    {
        black[i - 1].u = ext[i];
        black[i - 1].v = ext[i - 1];
        gray[i - 1].u = i - 1;
        gray[i - 1].v = i;
    }
}

void print_edges(const char *label, const Edge *edges, int count)
{
    printf("%s: ", label);
    for (int i = 0; i < count; i++)
        printf("(%d %d) ", edges[i].u, edges[i].v);
    printf("\n");
}

// Decomposes the black/gray graph into alternating cycles. Edges are
// directed (gray: i-1 -> i; black: pi_i -> pi_{i-1}), so every vertex has
// exactly one outgoing edge per color and the decomposition is unique
// (equivalent to the cycles of the permutation black_next(gray_next(.))).
// Returns the number of cycles found and fills cycle_len[c] with the number
// of black edges (== gray edges) in cycle c, cycle_vertices[c] with the
// sequence of vertices visited (starting vertex repeated at the end), and
// cycle_vertex_count[c] with how many entries that is. cycle_len and
// cycle_vertex_count must hold at least n+1 ints.
int find_cycles(const Edge *black, const Edge *gray, int n, int *cycle_len,
                int cycle_vertices[][MAX_CYCLE_VERTS], int *cycle_vertex_count)
{
    int m = n + 1;
    bool gray_used[MAX_N + 2] = {false};
    int black_out[MAX_N + 2], gray_out[MAX_N + 2];

    for (int e = 0; e < m; e++)
    {
        black_out[black[e].u] = e;
        gray_out[gray[e].u] = e;
    }

    int n_cycles = 0;
    for (int start_e = 0; start_e < m; start_e++)
    {
        if (gray_used[start_e])
            continue;

        int start_v = gray[start_e].u;
        int cur_v = start_v;
        int len = 0;
        int need_black = 0;
        int vcount = 0;
        cycle_vertices[n_cycles][vcount++] = cur_v;

        do
        {
            if (need_black)
            {
                int e = black_out[cur_v];
                cur_v = black[e].v;
                len++;
            }
            else
            {
                int e = gray_out[cur_v];
                gray_used[e] = true;
                cur_v = gray[e].v;
            }
            cycle_vertices[n_cycles][vcount++] = cur_v;
            need_black = !need_black;
        } while (cur_v != start_v);

        cycle_len[n_cycles] = len;
        cycle_vertex_count[n_cycles] = vcount;
        n_cycles++;
    }

    return n_cycles;
}

void print_cycles(const int *cycle_len, int cycle_vertices[][MAX_CYCLE_VERTS],
                  const int *cycle_vertex_count, int n_cycles)
{
    for (int c = 0; c < n_cycles; c++)
    {
        printf("Cycle %d (length %d): ", c, cycle_len[c]);
        for (int i = 0; i < cycle_vertex_count[c]; i++)
        {
            if (i > 0)
                printf(" %s ", (i % 2 == 1) ? "-g->" : "-b->");
            printf("%d", cycle_vertices[c][i]);
        }
        printf("\n");
    }
    printf("Odd cycles: %d / %d\n", count_odd_cycles(cycle_len, n_cycles), n_cycles);
}

// Number of odd-length cycles (length = number of black edges in the
// cycle) among cycle_len[0..n_cycles-1].
int count_odd_cycles(const int *cycle_len, int n_cycles)
{
    int odd = 0;
    for (int c = 0; c < n_cycles; c++)
        if (cycle_len[c] % 2 == 1)
            odd++;
    return odd;
}

// Lower bound on the transposition distance: floor(((n+1) - c_odd) / 2).
int lower_bound(int n, int c_odd)
{
    return (int)floor((n + 1 - c_odd) / 2.0);
}

// Runs the full pipeline (extend -> breakpoint graph -> cycles -> odd count
// -> lower_bound) directly from a 0-indexed permutation pi of size n.
int lower_bound_from_permutation(const int *pi, int n)
{
    int ext[MAX_N + 2];
    extend_permutation(pi, ext, n);

    Edge black[MAX_N + 1], gray[MAX_N + 1];
    build_edges(ext, n, black, gray);

    int cycle_len[MAX_N + 1];
    int cycle_vertices[MAX_N + 1][MAX_CYCLE_VERTS];
    int cycle_vertex_count[MAX_N + 1];
    int n_cycles = find_cycles(black, gray, n, cycle_len, cycle_vertices, cycle_vertex_count);

    return lower_bound(n, count_odd_cycles(cycle_len, n_cycles));
}

// Searches all n! permutations of size n for the largest gap between the
// actual transposition distance (from BFS) and the breakpoint-graph lower
// bound, prints every permutation achieving that gap, and returns the gap.
int find_max_gap_permutation(int n)
{
    char filename[512];
    d_array_path(filename, sizeof(filename), n);

    long long fact = 1;
    for (int x = 2; x <= n; x++)
        fact *= x;

    int *D = load_D_from_file(filename, fact);
    if (!D)
        return -1;

    int pi[MAX_N];
    int best_gap = -1;

    for (long long r = 0; r < fact; r++)
    {
        initialize_identity_permutation(pi, n);
        unrank1(n, (int)r, pi);
        int gap = D[r] - lower_bound_from_permutation(pi, n);
        if (gap > best_gap)
            best_gap = gap;
    }

    int count = 0;
    for (long long r = 0; r < fact; r++)
    {
        initialize_identity_permutation(pi, n);
        unrank1(n, (int)r, pi);
        if (D[r] - lower_bound_from_permutation(pi, n) == best_gap)
        {
            count++;
            printf("  ");
            print_array(pi, n);
        }
    }
    printf("Max gap: %d, achieved by %d permutation(s)\n", best_gap, count);

    free(D);
    return best_gap;
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

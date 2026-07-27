#include "function.h"
#include <stdio.h>

int main()
{
    int n = 11;
    int pi[] = {7, 3, 2, 10, 9, 8, 1, 6, 5, 4, 0}; // 0-indexed permutation; prints as [2 4 3 5 1]

    printf("Permutation: ");
    print_array(pi, n);

    int ext[MAX_N + 2];
    extend_permutation(pi, ext, n);

    Edge black[MAX_N + 1], gray[MAX_N + 1];
    build_edges(ext, n, black, gray);

    int cycle_len[MAX_N + 1];
    int cycle_vertices[MAX_N + 1][MAX_CYCLE_VERTS];
    int cycle_vertex_count[MAX_N + 1];
    int n_cycles = find_cycles(black, gray, n, cycle_len, cycle_vertices, cycle_vertex_count);
    print_cycles(cycle_len, cycle_vertices, cycle_vertex_count, n_cycles);

    printf("Lower bound: %d\n", lower_bound_from_permutation(pi, n));
    printf("Actual T-distance: %d\n", actual_distance_from_identity(pi, n));

    find_max_gap_permutation(11);

    return 0;
}

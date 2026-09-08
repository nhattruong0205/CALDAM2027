#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdbool.h>
#include <stddef.h>

typedef struct
{
    unsigned long long permutations_checked;
    unsigned long long unsortable_permutations;
    unsigned long long first_pass_states;
    unsigned long long second_pass_states;
} BruteForceStats;

// Method: brute force (2 passes)
bool brute_force_is_two_pass_sortable(const int *permutation, size_t length);
void brute_force_count_two_pass_sortable(
    size_t length,
    unsigned long long *count);

void brute_force_count_two_pass_sortable_with_stats(
    size_t length,
    unsigned long long *count,
    BruteForceStats *stats);
bool brute_force_all_permutations_two_pass_sortable(
    size_t length,
    unsigned long long *sortable_count);

// Method: brute force (first pass) + check for 231 pattern
bool one_pass_avoids_231(const int *permutation, size_t length);
void brute_force_count_one_pass_231_avoidable(
    size_t length,
    unsigned long long *count);

void brute_force_count_one_pass_231_avoidable_with_stats(
    size_t length,
    unsigned long long *count,
    BruteForceStats *stats);

void pruned_count_one_pass_231_avoidable_with_stats(
    size_t length,
    unsigned long long *count,
    BruteForceStats *stats);

#endif

#include "function.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static void print_usage(const char *program)
{
    fprintf(stderr, "Usage: %s N\n", program);
    fprintf(stderr, "Counts permutations of 1..N sortable by two stack passes.\n");
}

int main(int argc, char **argv)
{
    char *end = NULL;
    unsigned long parsed_length;
    unsigned long long sortable_count;
    unsigned long long one_pass_count;
    unsigned long long heuristic_count;
    BruteForceStats two_pass_stats = {0};
    BruteForceStats one_pass_stats = {0};
    BruteForceStats heuristic_stats = {0};

    if (argc != 2)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    errno = 0;
    parsed_length = strtoul(argv[1], &end, 10);
    if (errno != 0 || end == argv[1] || *end != '\0' || parsed_length > 13)
    {
        fprintf(stderr, "N must be an integer between 0 and 13.\n");
        return EXIT_FAILURE;
    }

    // brute_force_count_two_pass_sortable_with_stats(
    //     (size_t)parsed_length,
    //     &sortable_count,
    //     &two_pass_stats);
    // printf("two-pass average states per permutation: %.2f\n",
    //        (double)(two_pass_stats.first_pass_states +
    //                 two_pass_stats.second_pass_states) /
    //            (double)two_pass_stats.permutations_checked);
    // printf("two-pass unsortable permutations: %llu\n",
    //        two_pass_stats.unsortable_permutations);

    // // Brute-force first pass, then check each completed output for 231

    // brute_force_count_one_pass_231_avoidable_with_stats(
    //     (size_t)parsed_length,
    //     &one_pass_count,
    //     &one_pass_stats);
    // printf("one-pass average states per permutation: %.2f\n",
    //        (double)one_pass_stats.first_pass_states /
    //            (double)one_pass_stats.permutations_checked);
    // printf("one-pass unsortable permutations: %llu\n",
    //        one_pass_stats.unsortable_permutations);

    pruned_count_one_pass_231_avoidable_with_stats(
        (size_t)parsed_length,
        &heuristic_count,
        &heuristic_stats);
    printf("heuristic one-pass unsortable permutations: %llu\n",
           heuristic_stats.unsortable_permutations);
    printf("heuristic average states per permutation: %.2f\n",
           (double)heuristic_stats.first_pass_states /
               (double)heuristic_stats.permutations_checked);

    return EXIT_SUCCESS;
}

// Command
// gcc -o main main.c function.c -lm
// ./main 7

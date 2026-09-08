#include "function.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

static bool brute_force_is_two_pass_sortable_with_stats(
    const int *permutation,
    size_t length,
    BruteForceStats *stats);
static bool second_pass_search(
    const int *input,
    size_t length,
    size_t input_index,
    int *stack,
    size_t stack_size,
    size_t output_size,
    BruteForceStats *stats)
{
    if (stats != NULL)
    {
        ++stats->second_pass_states;
    }
    /* O(n * C_n) time for all valid second-pass schedules; O(n) space. */
    if (input_index == length)
    {
        while (stack_size > 0)
        {
            if (stack[stack_size - 1] != (int)(output_size + 1))
            {
                return false;
            }
            --stack_size;
            ++output_size;
        }
        return output_size == length;
    }

    int previous_value = stack[stack_size];
    /* Push branch: one O(1) operation. */
    stack[stack_size] = input[input_index];
    if (second_pass_search(
            input,
            length,
            input_index + 1,
            stack,
            stack_size + 1,
            output_size,
            stats))
    {
        return true;
    }
    stack[stack_size] = previous_value;

    /* Pop branch: one O(1) operation when the next output is available. */
    if (stack_size > 0 && stack[stack_size - 1] == (int)(output_size + 1))
    {
        int value = stack[stack_size - 1];
        if (second_pass_search(
                input,
                length,
                input_index,
                stack,
                stack_size - 1,
                output_size + 1,
                stats))
        {
            return true;
        }
        stack[stack_size - 1] = value;
    }

    return false;
}

static bool first_pass_search(
    const int *input,
    size_t length,
    size_t input_index,
    int *stack,
    size_t stack_size,
    int *output,
    size_t output_size,
    BruteForceStats *stats)
{
    if (stats != NULL)
    {
        ++stats->first_pass_states;
    }
    /* O(n * C_n) for the first pass; O(n) space. */
    if (input_index == length)
    {
        if (stack_size > 0)
        {
            output[output_size] = stack[stack_size - 1];
            return first_pass_search(
                input,
                length,
                input_index,
                stack,
                stack_size - 1,
                output,
                output_size + 1,
                stats);
        }

        return second_pass_search(output, length, 0, stack, 0, 0, stats);
    }

    int previous_value = stack[stack_size];
    stack[stack_size] = input[input_index];
    if (first_pass_search(
            input,
            length,
            input_index + 1,
            stack,
            stack_size + 1,
            output,
            output_size,
            stats))
    {
        return true;
    }
    stack[stack_size] = previous_value;

    if (stack_size > 0)
    {
        int value = stack[stack_size - 1];
        output[output_size] = stack[stack_size - 1];
        if (first_pass_search(
                input,
                length,
                input_index,
                stack,
                stack_size - 1,
                output,
                output_size + 1,
                stats))
        {
            return true;
        }
        stack[stack_size - 1] = value;
    }

    return false;
}

bool brute_force_is_two_pass_sortable(
    const int *permutation,
    size_t length)
{
    return brute_force_is_two_pass_sortable_with_stats(
        permutation,
        length,
        NULL);
}

static bool brute_force_is_two_pass_sortable_with_stats(
    const int *permutation,
    size_t length,
    BruteForceStats *stats)
{
    /* O(n * C_n^2) time for one permutation; O(n) auxiliary space. */
    int *stack = calloc(length, sizeof(*stack));
    int *first_pass_output = malloc(length * sizeof(*first_pass_output));
    bool sortable;

    if (length > 0 && (stack == NULL || first_pass_output == NULL))
    {
        free(stack);
        free(first_pass_output);
        return false;
    }

    sortable = first_pass_search(
        permutation,
        length,
        0,
        stack,
        0,
        first_pass_output,
        0,
        stats);

    free(stack);
    free(first_pass_output);
    return sortable;
}

static void count_permutations(
    int *permutation,
    bool *used,
    size_t position,
    size_t length,
    unsigned long long *count,
    BruteForceStats *stats)
{
    if (position == length)
    {
        if (stats != NULL)
        {
            ++stats->permutations_checked;
        }
        if (brute_force_is_two_pass_sortable_with_stats(
                permutation,
                length,
                stats))
        {
            ++(*count);
        }
        else
        {
            ++stats->unsortable_permutations;
            /* print_permutation(permutation, length); */
        }
        return;
    }

    for (size_t value = 1; value <= length; ++value)
    {
        if (!used[value])
        {
            used[value] = true;
            permutation[position] = (int)value;
            count_permutations(
                permutation,
                used,
                position + 1,
                length,
                count,
                stats);
            used[value] = false;
        }
    }
}

void brute_force_count_two_pass_sortable(
    size_t length,
    unsigned long long *count)
{
    brute_force_count_two_pass_sortable_with_stats(length, count, NULL);
}

void brute_force_count_two_pass_sortable_with_stats(
    size_t length,
    unsigned long long *count,
    BruteForceStats *stats)
{
    /* O(n! * n * C_n^2) time over all permutations; O(n) space. */
    int *permutation = malloc(length * sizeof(*permutation));
    bool *used = calloc(length + 1, sizeof(*used));

    *count = 0;
    if (length > 0 && (permutation == NULL || used == NULL))
    {
        free(permutation);
        free(used);
        return;
    }

    count_permutations(permutation, used, 0, length, count, stats);

    free(permutation);
    free(used);
}

bool brute_force_all_permutations_two_pass_sortable(
    size_t length,
    unsigned long long *sortable_count)
{
    /* Same O(n! * n * C_n^2) time as the counting operation. */
    unsigned long long total = 1;

    for (size_t value = 2; value <= length; ++value)
    {
        total *= value;
    }

    brute_force_count_two_pass_sortable(length, sortable_count);
    return *sortable_count == total;
}

bool one_pass_avoids_231(const int *permutation, size_t length)
{
    /* O(n) time and O(n) space for one completed output. */
    int *stack = malloc(length * sizeof(*stack));
    size_t stack_size = 0;
    int middle_value = 0;

    if (length > 0 && stack == NULL)
    {
        return false;
    }

    for (size_t index = 0; index < length; ++index)
    {
        int value = permutation[index];

        if (value < middle_value)
        {
            free(stack);
            return false;
        }

        while (stack_size > 0 && value > stack[stack_size - 1])
        {
            middle_value = stack[--stack_size];
        }

        stack[stack_size++] = value;
    }

    free(stack);
    return true;
}

static bool brute_force_first_pass_231_search(
    const int *input,
    size_t length,
    size_t input_index,
    int *stack,
    size_t stack_size,
    int *output,
    size_t output_size,
    BruteForceStats *stats)
{
    if (stats != NULL)
    {
        ++stats->first_pass_states;
    }

    if (input_index == length)
    {
        if (stack_size > 0)
        {
            output[output_size] = stack[stack_size - 1];
            return brute_force_first_pass_231_search(
                input,
                length,
                input_index,
                stack,
                stack_size - 1,
                output,
                output_size + 1,
                stats);
        }

        return one_pass_avoids_231(output, length);
    }

    int previous_value = stack[stack_size];
    stack[stack_size] = input[input_index];
    if (brute_force_first_pass_231_search(
            input,
            length,
            input_index + 1,
            stack,
            stack_size + 1,
            output,
            output_size,
            stats))
    {
        return true;
    }
    stack[stack_size] = previous_value;

    if (stack_size > 0)
    {
        int value = stack[stack_size - 1];
        output[output_size] = value;
        if (brute_force_first_pass_231_search(
                input,
                length,
                input_index,
                stack,
                stack_size - 1,
                output,
                output_size + 1,
                stats))
        {
            return true;
        }
        stack[stack_size - 1] = value;
    }

    return false;
}

static bool contains_231(
    const int *values,
    size_t length)
{
    return !one_pass_avoids_231(values, length);
}

static bool contains_132_in_stack(
    const int *stack,
    size_t stack_size)
{
    int *decreasing_stack = malloc(stack_size * sizeof(*decreasing_stack));
    size_t decreasing_size = 0;
    int middle_value = INT_MIN;

    if (stack_size > 0 && decreasing_stack == NULL)
    {
        return false;
    }

    for (size_t index = stack_size; index-- > 0;)
    {
        int value = stack[index];

        if (value < middle_value)
        {
            free(decreasing_stack);
            return true;
        }

        while (decreasing_size > 0 &&
               value > decreasing_stack[decreasing_size - 1])
        {
            middle_value = decreasing_stack[--decreasing_size];
        }
        decreasing_stack[decreasing_size++] = value;
    }

    free(decreasing_stack);
    return false;
}

typedef struct
{
    uint64_t forbidden;
    int minimum;
} Stack132State;

/*
 * Incremental, undoable version of one_pass_avoids_231's O(n) algorithm.
 * `values` holds the same monotonically-decreasing stack that
 * one_pass_avoids_231 keeps locally; `middle_value` is its running lower
 * bound. Rejected appends leave the state untouched (no undo needed).
 *
 * A single append can pop several elements from `values` before pushing the
 * new one, and only the last-popped one ends up recoverable from
 * middle_value alone -- the rest of the popped slots get reused by later,
 * unrelated pushes deeper in the search before we backtrack this far. So
 * every popped value is recorded on `undo_log` (a stack shared across the
 * whole search) as it is popped; undo replays exactly the entries this
 * append produced, back into their original slots, low index first (which
 * is LIFO order relative to the log, since the lowest freed slot was the
 * last one popped). Because appends/undos nest in strict LIFO order along
 * any root-to-leaf path and each of the n distinct values can be "popped
 * and pending restoration" at most once at a time, log_size never exceeds
 * length.
 */
typedef struct
{
    int *values;
    size_t size;
    int middle_value;
    int *undo_log;
    size_t log_size;
} Output231State;

static bool output231_try_append(
    Output231State *state,
    int value,
    size_t *previous_size,
    int *previous_middle_value)
{
    *previous_size = state->size;
    *previous_middle_value = state->middle_value;

    if (value < state->middle_value)
    {
        return false;
    }

    while (state->size > 0 && value > state->values[state->size - 1])
    {
        state->middle_value = state->values[--state->size];
        state->undo_log[state->log_size++] = state->middle_value;
    }

    state->values[state->size++] = value;
    return true;
}

static void output231_undo_append(
    Output231State *state,
    size_t previous_size,
    int previous_middle_value)
{
    size_t size_before_push = state->size - 1;

    for (size_t index = size_before_push; index < previous_size; ++index)
    {
        state->values[index] = state->undo_log[--state->log_size];
    }
    state->size = previous_size;
    state->middle_value = previous_middle_value;
}

static bool heuristic_first_pass_search(
    const int *input,
    size_t length,
    size_t input_index,
    int *stack,
    size_t stack_size,
    int *output,
    size_t output_size,
    Stack132State current132,
    Stack132State *stack132_by_depth,
    Output231State *output231,
    BruteForceStats *stats)
{
    if (stats != NULL)
    {
        ++stats->first_pass_states;
    }

    if (input_index == length)
    {
        if (stack_size > 0)
        {
            int value = stack[stack_size - 1];
            size_t previous_size;
            int previous_middle_value;

            if (!output231_try_append(
                    output231, value, &previous_size, &previous_middle_value))
            {
                return false;
            }
            output[output_size] = value;
            if (heuristic_first_pass_search(
                    input, length, input_index, stack, stack_size - 1,
                    output, output_size + 1,
                    stack132_by_depth[stack_size - 1],
                    stack132_by_depth, output231, stats))
            {
                return true;
            }
            output231_undo_append(output231, previous_size, previous_middle_value);
            return false;
        }
        return true;
    }

    int previous_value = stack[stack_size];
    int value = input[input_index];
    uint64_t value_bit = UINT64_C(1) << value;

    /* Only the push branch is blocked by a forbidden value -- popping first
     * and retrying later (with a less restrictive 132 state) may still be
     * viable, so we must not abandon this node entirely. */
    if ((current132.forbidden & value_bit) == 0)
    {
        Stack132State next132 = current132;
        if (value > next132.minimum)
        {
            next132.forbidden |=
                (value_bit - 1) &
                ~((UINT64_C(1) << (next132.minimum + 1)) - 1);
        }
        if (value < next132.minimum)
        {
            next132.minimum = value;
        }
        Stack132State previous_child_state = stack132_by_depth[stack_size + 1];
        stack132_by_depth[stack_size + 1] = next132;
        stack[stack_size] = value;
        if (heuristic_first_pass_search(
                input, length, input_index + 1, stack, stack_size + 1,
                output, output_size, next132, stack132_by_depth,
                output231, stats))
        {
            return true;
        }
        stack[stack_size] = previous_value;
        stack132_by_depth[stack_size + 1] = previous_child_state;
    }

    if (stack_size > 0)
    {
        value = stack[stack_size - 1];
        size_t previous_size;
        int previous_middle_value;
        Stack132State previous_pop_state = stack132_by_depth[stack_size];

        if (!output231_try_append(
                output231, value, &previous_size, &previous_middle_value))
        {
            return false;
        }
        output[output_size] = value;
        if (heuristic_first_pass_search(
                input, length, input_index, stack, stack_size - 1,
                output, output_size + 1,
                stack132_by_depth[stack_size - 1],
                stack132_by_depth, output231, stats))
        {
            return true;
        }
        stack[stack_size - 1] = value;
        stack132_by_depth[stack_size] = previous_pop_state;
        output231_undo_append(output231, previous_size, previous_middle_value);
    }
    return false;
}

static void count_one_pass_permutations(
    int *permutation,
    bool *used,
    size_t position,
    size_t length,
    unsigned long long *count,
    BruteForceStats *stats)
{
    if (position == length)
    {
        if (stats != NULL)
        {
            ++stats->permutations_checked;
        }
        int *stack = calloc(length, sizeof(*stack));
        int *output = malloc(length * sizeof(*output));
        bool sortable = false;

        if (length == 0 || (stack != NULL && output != NULL))
        {
            sortable = brute_force_first_pass_231_search(
                permutation,
                length,
                0,
                stack,
                0,
                output,
                0,
                stats);
        }

        free(stack);
        free(output);

        if (sortable)
        {
            ++(*count);
        }
        else
        {
            ++stats->unsortable_permutations;
            /* print_permutation(permutation, length); */
        }
        return;
    }

    for (size_t value = 1; value <= length; ++value)
    {
        if (!used[value])
        {
            used[value] = true;
            permutation[position] = (int)value;
            count_one_pass_permutations(
                permutation,
                used,
                position + 1,
                length,
                count,
                stats);
            used[value] = false;
        }
    }
}

void brute_force_count_one_pass_231_avoidable(
    size_t length,
    unsigned long long *count)
{
    brute_force_count_one_pass_231_avoidable_with_stats(
        length,
        count,
        NULL);
}

void brute_force_count_one_pass_231_avoidable_with_stats(
    size_t length,
    unsigned long long *count,
    BruteForceStats *stats)
{
    /* O(n! * C_n * n) time and O(n) auxiliary space. */
    int *permutation = malloc(length * sizeof(*permutation));
    bool *used = calloc(length + 1, sizeof(*used));

    *count = 0;
    if (length > 0 && (permutation == NULL || used == NULL))
    {
        free(permutation);
        free(used);
        return;
    }

    count_one_pass_permutations(
        permutation,
        used,
        0,
        length,
        count,
        stats);

    free(permutation);
    free(used);
}

static void heuristic_count_permutations(
    int *permutation,
    bool *used,
    size_t position,
    size_t length,
    unsigned long long *count,
    BruteForceStats *stats)
{
    if (position == length)
    {
        ++stats->permutations_checked;
        int *stack = calloc(length, sizeof(*stack));
        int *output = malloc(length * sizeof(*output));
        Stack132State *stack132_by_depth =
            malloc((length + 1) * sizeof(*stack132_by_depth));
        int *output231_values = malloc(length * sizeof(*output231_values));
        int *output231_undo_log = malloc(length * sizeof(*output231_undo_log));

        if (length == 0 ||
            (stack != NULL && output != NULL &&
             stack132_by_depth != NULL && output231_values != NULL &&
             output231_undo_log != NULL))
        {
            Stack132State initial132 = {0, (int)length + 1};
            Output231State output231 = {output231_values, 0, 0, output231_undo_log, 0};

            if (length > 0)
            {
                stack132_by_depth[0] = initial132;
            }

            if (heuristic_first_pass_search(
                    permutation, length, 0, stack, 0, output, 0,
                    initial132, stack132_by_depth, &output231, stats))
            {
                ++(*count);
            }
            else
            {
                ++stats->unsortable_permutations;
            }
        }
        free(stack);
        free(output);
        free(stack132_by_depth);
        free(output231_values);
        free(output231_undo_log);
        return;
    }

    for (size_t value = 1; value <= length; ++value)
    {
        if (!used[value])
        {
            used[value] = true;
            permutation[position] = (int)value;
            heuristic_count_permutations(
                permutation, used, position + 1, length, count, stats);
            used[value] = false;
        }
    }
}

void pruned_count_one_pass_231_avoidable_with_stats(
    size_t length,
    unsigned long long *count,
    BruteForceStats *stats)
{
    int *permutation = malloc(length * sizeof(*permutation));
    bool *used = calloc(length + 1, sizeof(*used));

    *count = 0;
    if (length > 0 && (permutation == NULL || used == NULL))
    {
        free(permutation);
        free(used);
        return;
    }

    heuristic_count_permutations(
        permutation, used, 0, length, count, stats);
    free(permutation);
    free(used);
}

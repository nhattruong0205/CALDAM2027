# One-Stack Sorting Methods

The program counts sortable permutations of `1..n` using one stack. It contains two methods:

1. Brute-force one stack with two passes.
2. Brute-force all first-pass stack schedules, then check completed outputs for
   `231` avoidance.
3. Prune first-pass branches using `231` output and `132` stack checks.

The methods do not use the West-2 restriction.

## Common Step: Generate Permutations

Both methods generate every permutation of `1..n`.

The number of permutations is:

```text
n!
```

Constructing or processing one permutation costs `O(n)` time. Across all inputs, permutation generation costs:

```text
O(n * n!)
```

## Method 1: Brute-Force Two Passes

The main functions are:

```c
brute_force_is_two_pass_sortable
brute_force_count_two_pass_sortable
```

### Step 1: Explore the first pass

At every state, the algorithm tries both legal choices:

1. Push the next input value.
2. Pop the stack top to the intermediate output.

It restores the state after a failed branch, so this is backtracking, not a greedy algorithm.

A complete pass has `n` pushes and `n` pops. The number of valid push/pop schedules is the Catalan number:

```text
C_n = 1/(n+1) * binomial(2n, n)
```

Each schedule has `2n = O(n)` operations. Therefore, the first-pass search costs:

```text
O(n * C_n)
```

### Step 2: Explore the second pass

For every possible first-pass output, the algorithm tries valid push/pop schedules again. The second-pass search costs:

```text
O(n * C_n)
```

### Step 3: Test one permutation

The second-pass search is performed for each first-pass possibility:

```text
O((n * C_n) * C_n)
= O(n * C_n^2)
```

This is the worst-case time for one permutation. The search may stop early when it finds a successful pair of passes.

### Step 4: Test every permutation

There are `n!` input permutations, so the total worst-case running time is:

```text
O(n! * n * C_n^2)
```

Using `C_n = Theta(4^n / n^(3/2))`, this is approximately:

```text
O(n! * 16^n / n^2)
```

The auxiliary space complexity is `O(n)` for the stacks, intermediate output, permutation, and recursion depth.

## Method 2: Brute-Force One Pass, Then Check `231`

The main functions are:

```c
one_pass_avoids_231
brute_force_count_one_pass_231_avoidable
```

This method does not run a second stack pass. For each input permutation, it
enumerates valid push/pop schedules for the first pass. Each completed output
is then checked for the `231` pattern. A `231` pattern consists of indices
`i < j < k` whose values have relative order `2, 3, 1`:

```text
permutation[k] < permutation[i] < permutation[j]
```

### Step 1: Generate input permutations

The outer backtracking generator creates all `n!` permutations. Across all inputs, this costs:

```text
O(n * n!)
```

### Step 2: Enumerate first-pass schedules

The first-pass search tries push and pop at every legal state. There are at
most `C_n` valid schedules, and each schedule has `O(n)` operations:

```text
O(n * C_n) per input permutation
```

### Step 3: Check each completed output for `231`

`one_pass_avoids_231` scans one completed output with a stack. Each value is
pushed once and popped at most once, so one check costs:

```text
O(n) time and O(n) space
```

This is a linear-time pattern detector applied after a first-pass schedule.

### Step 4: Test every permutation

The first-pass search and `O(n)` pattern check are repeated for all `n!` input
permutations:

```text
O(n! * C_n * n)
```

Therefore:

```text
Two passes:                   O(n! * n * C_n^2)
Brute-force first pass + 231: O(n! * n * C_n)
```

The one-pass method uses `O(n)` auxiliary space. If a valid completed output
is found early, the search may stop before visiting all `C_n` schedules for
that input; the bound above is the worst case.

## Important Distinction

## Method 3: Pruned First-Pass Search

The function is:

```c
pruned_count_one_pass_231_avoidable_with_stats
```

This method has the same goal as Method 2, but checks impossible branches
while the first pass is still being constructed:

1. If the output produced so far contains `231`, reject the branch.
2. If no input remains and the stack contains `132` from bottom to top, reject
   the branch, because draining it produces `231` from top to bottom.
3. Otherwise, continue trying push and pop operations.

The pruning does not change the accepted result. It only avoids branches that
cannot produce an acceptable output. Its worst-case bound remains:

```text
O(n! * n * C_n)
```

The actual number of visited states can be lower, which is the improvement
measured by `heuristic average states per permutation`.

The brute-force one-pass method does enumerate first-pass push/pop schedules.
It is different from a direct optimized `231` checker, which skips the `C_n`
schedule factor and would cost only `O(n! * n)`.

If one-pass push/pop schedules were also enumerated explicitly, the bound would instead be:

```text
O(n! * n * C_n)
```

## Compile and Run

From this directory:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror function.c main.c -o main
./main 4
```

The argument is the permutation size `n`.

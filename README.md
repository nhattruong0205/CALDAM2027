# Lower Bound Gap Estimation

**Goal:** improve the gap estimation of the lower bound.

## Build and run

```bash
gcc -o main main.c function.c
./main
```

## Notes

- Permutations are stored 0-indexed internally, but printed 1-indexed.

## Because Tdistance array is to large, we install

```bash
gcc -obrew install git-lfs
git lfs install
```

## Zip big files using 
```bash
gzip -k dArray/distances_n12.txt
```
// Helper functions
void swap(int *a, int *b);

void compute_inverse(int pi[], int pi_inv[], int n);

void print_array(int arr[], int n);

void save_D_to_file(const char *filename, int *D, long long size);

void initialize_identity_permutation(int *pid, int n);

void transposition(const int *src, int *dst, int n, int i, int j, int k);

int *ComputeTDistanceFromIdentity(int n);

int rank1(int n, int pi[], int pi_inv[]);

int rank_safe(int n, const int src[], int *inv_buf);

// Original recursive unrank1: Builds a permutation from a given rank
void unrank1(int n, int r, int pi[]);

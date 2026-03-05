#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MOD 1000000007LL

static long long mod_pow(long long a, long long e) {
    long long r = 1 % MOD;
    a %= MOD;
    while (e > 0) {
        if (e & 1LL) r = (r * a) % MOD;
        a = (a * a) % MOD;
        e >>= 1LL;
    }
    return r;
}

void tuples(int *prime_list,
            int nr_deprime,
            int k,
            int pos,
            int or_current,
            int max_n,
            long long *count)
{
    (void)prime_list;
    (void)nr_deprime;
    (void)pos;
    (void)or_current;

    *count = 0;

    if (max_n < 2 || k <= 0) return;

    unsigned char *is_p = (unsigned char*)malloc((max_n + 1) * sizeof(unsigned char));
    if (!is_p) {
        printf("Memory error (sieve)\n");
        return;
    }

    memset(is_p, 1, (max_n + 1) * sizeof(unsigned char));
    is_p[0] = 0;
    if (max_n >= 1) is_p[1] = 0;

    for (int i = 2; (long long)i * i <= max_n; i++) {
        if (is_p[i]) {
            for (int j = i * i; j <= max_n; j += i)
                is_p[j] = 0;
        }
    }

    
    int B = 0;
    while (((1U << B) <= (unsigned)max_n) && B < 31) B++;
    int N = 1 << B;  // N ~ 2^20 pentru 1e6

   
    long long *f = (long long*)calloc((size_t)N, sizeof(long long));
    if (!f) {
        printf("Memory error (f array)\n");
        free(is_p);
        return;
    }

    for (int x = 2; x <= max_n; x++) {
        if (is_p[x]) f[x] = 1;
    }

    for (int bit = 0; bit < B; bit++) {
        for (int mask = 0; mask < N; mask++) {
            if (mask & (1 << bit)) {
                f[mask] += f[mask ^ (1 << bit)];
                if (f[mask] >= MOD) f[mask] -= MOD; 
            }
        }
    }

    for (int mask = 0; mask < N; mask++) {
        f[mask] = mod_pow(f[mask], (long long)k);
    }


    for (int bit = 0; bit < B; bit++) {
        for (int mask = 0; mask < N; mask++) {
            if (mask & (1 << bit)) {
                f[mask] -= f[mask ^ (1 << bit)];
                f[mask] %= MOD;
                if (f[mask] < 0) f[mask] += MOD;
            }
        }
    }

    long long ans = 0;
    for (int x = 2; x <= max_n; x++) {
        if (is_p[x]) {
            ans += f[x];
            ans %= MOD;
        }
    }

    *count = ans;

    free(f);
    free(is_p);
}

int main()
{
    int n, k;
    printf("Introduce limita superioara n: ");
    if (scanf("%d", &n) != 1) return 0;

    printf("Introduce lungimea k: ");
    if (scanf("%d", &k) != 1) return 0;

    if (n < 2 || k <= 0) {
        printf("Introdu corect n>=2 si k>0.\n");
        return 0;
    }


    int *prime_list = (int*)malloc((n + 1) * sizeof(int));
    if (!prime_list) {
        printf("Memory error\n");
        return 1;
    }
    int nr_deprime = 0;

  
    unsigned char *is_p2 = (unsigned char*)malloc((n + 1) * sizeof(unsigned char));
    if (!is_p2) { free(prime_list); return 1; }

    memset(is_p2, 1, (n + 1) * sizeof(unsigned char));
    is_p2[0] = 0;
    if (n >= 1) is_p2[1] = 0;
    for (int i = 2; (long long)i * i <= n; i++)
        if (is_p2[i])
            for (int j = i * i; j <= n; j += i)
                is_p2[j] = 0;

    for (int x = 2; x <= n; x++)
        if (is_p2[x])
            *(prime_list + nr_deprime++) = x;

    free(is_p2);

    long long result_count = 0;
    tuples(prime_list, nr_deprime, k, 0, 0, n, &result_count);

    printf("T(%d, %d) = %lld (mod %lld)\n", n, k, result_count, MOD);

    free(prime_list);
    return 0;
}
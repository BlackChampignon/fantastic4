#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#define MOD 1000000007LL

static long long mod_pow(long long base, long long exp) {
    long long result = 1;
    base %= MOD;

    while (exp > 0) {
        if (exp & 1LL) {
            result = (result * base) % MOD;
        }
        base = (base * base) % MOD;
        exp >>= 1LL;
    }

    return result;
}

static double time_diff_sec(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(void) {
    struct timespec start_time, end_time;
    struct rusage usage;

    clock_gettime(CLOCK_MONOTONIC, &start_time);

    int n, k;
 
   n=1000000;
    k=999983;

    printf("100000\n999983\n");
  /*
    if (scanf("%d", &n) != 1) return 0;
    if (scanf("%d", &k) != 1) return 0;
*/ 
    if (n < 2 || k <= 0) {
        printf("0\n");
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        getrusage(RUSAGE_SELF, &usage);

        printf("Execution time: %.9f sec\n", time_diff_sec(start_time, end_time));
        printf("Maximum memory usage: %ld KB\n", usage.ru_maxrss);
        return 0;
    }

    
    unsigned char *is_prime = (unsigned char *)malloc((size_t)(n + 1));
    if (!is_prime) {
        return 1;
    }

    memset(is_prime, 1, (size_t)(n + 1));
    is_prime[0] = 0;
    is_prime[1] = 0;

    for (int i = 2; (long long)i * i <= n; i++) {
        if (is_prime[i]) {
            for (int j = i * i; j <= n; j += i) {
                is_prime[j] = 0;
            }
        }
    }

    int B = 0;
    while ((1U << B) <= (unsigned)n) {
        B++;
    }

    int mask_count = 1 << B;

    long long *f = (long long *)calloc((size_t)mask_count, sizeof(long long));
    if (!f) {
        free(is_prime);
        return 1;
    }

    for (int x = 2; x <= n; x++) {
        if (is_prime[x]) {
            f[x] = 1;
        }
    }

    //tranformation over submasks 
    for (int bit = 0; bit < B; bit++) {
        for (int mask = 0; mask < mask_count; mask++) {
            if (mask & (1 << bit)) {
                f[mask] += f[mask ^ (1 << bit)];
            }
        }
    }

    for (int mask = 0; mask < mask_count; mask++) {
        f[mask] = mod_pow(f[mask] % MOD, k);
    }

    //recall exact OR counts 
    for (int bit = 0; bit < B; bit++) {
        for (int mask = 0; mask < mask_count; mask++) {
            if (mask & (1 << bit)) {
                f[mask] -= f[mask ^ (1 << bit)];
                f[mask] %= MOD;
                if (f[mask] < 0) {
                    f[mask] += MOD;
                }
            }
        }
    }

    long long answer = 0;
    for (int x = 2; x <= n; x++) {
        if (is_prime[x]) {
            answer += f[x];
            if (answer >= MOD) {
                answer -= MOD;
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    getrusage(RUSAGE_SELF, &usage);

    printf("%lld\n", answer);
    printf("Execution time: %.9f sec\n", time_diff_sec(start_time, end_time));
    printf("Maximum memory usage: %ld B\n", usage.ru_maxrss);

    free(f);
    free(is_prime);
    return 0;
}
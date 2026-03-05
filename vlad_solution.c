#include <stdio.h>
#include <stdlib.h>


unsigned long long modPow(unsigned long long base, unsigned long long exp) {
    unsigned long long res = 1;
    base %= 1000000007;
    while (exp > 0) {
        if (exp % 2) res = (res * base) % 1000000007;
        base = (base * base) % 1000000007;
        exp /= 2;
    }
    return res;
}


// Improved sieve prime generator
// (Checking only odd numbers)
unsigned long long* sieve_gen(int n, int **primes, int *size) {
    *size = 0;
    if (n < 2) return 0;
    unsigned long long *sieve = (unsigned long long*)calloc(n + 1, sizeof(unsigned long long));
    sieve[2] = 1;
    (*size)++;
    *primes = (int*)malloc(1 * sizeof(int));
    (*primes)[0] = 2;
    for (int i = 3; i <= n; i += 2) sieve[i] = 1;
    for (int i = 3; (long long)i * i <= n; i += 2)
        if (sieve[i]) {
            for (int j = i * i; j <= n; j += 2 * i)
                sieve[j] = 0;
        }
    for (int i = 3; i <= n; i += 2) {
        if (sieve[i]) (*size)++;
    }
    *primes = realloc(*primes, (*size) * sizeof(int));
    (*size) = 1;
    for (int i = 3; i <= n; i++) {
        if (sieve[i]) (*primes)[(*size)++] = i;
    }
    return sieve;
}

int main() {
    unsigned long long sum = 0;
    int n=1000000, k=999983;
    // int n=1000, k=10;
    //scanf("%d %d", &n, &k);
    int *primes = NULL;
    int prime_count = 0;
    unsigned long long *primeCountPerMask = sieve_gen(n, &primes, &prime_count);
    // for (int i = 1; i < prime_count; i++) {
    //     unsigned long long int repetitions = 0;
    //     for (int j = 0; j < i; j++) {
    //         if ((primes[i] | primes[j]) == primes[i]) {
    //             printf("%d|%d\n", primes[i], primes[j]);
    //             primeCountPerMask[primes[i]]++;
    //             repetitions += primeCountPerMask[primes[j]];
    //         }
    //     }
    //     primeCountPerMask[primes[i]] = modPow(primeCountPerMask[primes[i]], k)-repetitions;
    //     primeCountPerMask[primes[i]] = modPow(primeCountPerMask[primes[i]]%MOD, k);
    //     sum += primeCountPerMask[primes[i]];
    //     printf("%d => %llu\n", primes[i], primeCountPerMask[primes[i]]);
    // }

    // Bits needed
    int bit_count = 0;
    while ((1 << bit_count) <= n) {
        bit_count++;
    }
    int mask_count = 1 << bit_count;

    // To count how many primes are per mask
    primeCountPerMask = realloc(primeCountPerMask, mask_count * sizeof(unsigned long long));
    // Zeta transform (sum over submasks)
    for (int bit = 0; bit < bit_count; bit++) {
        for (int mask = 0; mask < mask_count; mask++) {
            if (mask & (1 << bit)) {
                primeCountPerMask[mask] += primeCountPerMask[mask ^ (1 << bit)];
            }
        }
    }
    // Count all possible sequences (^k)
    for (int mask = 0; mask < mask_count; mask++) {
        primeCountPerMask[mask] = modPow(primeCountPerMask[mask], k);
    }
    // Mobius inversion to get exact counts
    for (int bit = 0; bit < bit_count; bit++) {
        for (int mask = 0; mask < mask_count; mask++) {
            if (mask & (1 << bit)) {
                // Subtract if the mask doesn't have the bit
                if (primeCountPerMask[mask] >= primeCountPerMask[mask ^ (1 << bit)]) {
                    primeCountPerMask[mask] -= primeCountPerMask[mask ^ (1 << bit)];
                } else {
                    primeCountPerMask[mask] = primeCountPerMask[mask] + 1000000007 - primeCountPerMask[mask ^ (1 << bit)];
                }
            }
        }
    }
    // printf("%d\n", prime_count);

    // for (int i = 0; i <= n; i++) {
    //     sum += primeCountPerMask[i];
    // }

    // Sum the results for primes
    for (int i = 0; i < prime_count; i++) {
        sum = (sum + primeCountPerMask[primes[i]]) % 1000000007;
    }

    printf("%llu\n", sum);
    free(primes);
    free(primeCountPerMask);
    return 0;
}
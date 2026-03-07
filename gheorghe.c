#include <stdio.h>
#include <stdlib.h>

#define MOD 1000000007

int* ciuru(int n) {
    int *ciur = (int*)malloc((size_t)(n + 1) * sizeof(int));
    if (ciur == NULL) {
        return NULL;
    }

    for (int i = 0; i <= n; i++) ciur[i] = 1;
    ciur[0] = ciur[1] = 0;

    for (int i = 2; i * i <= n; i++) {
        if (ciur[i]) {
            for (int j = i * i; j <= n; j += i) {
                ciur[j] = 0;
            }
        }
    }
    return ciur;
}

long long modPow(long long base, long long exp) {
    long long result = 1;
    base %= MOD;
    while (exp > 0) {
        if (exp % 2) result = (result * base) % MOD;
        base = (base * base) % MOD;
        exp /= 2;
    }
    return result;
}

int main(void) {
    int n = 1000000;
    long long k = 999983;

    int *ciur = ciuru(n);

    int bitCount = 0;
    while ((1 << bitCount) <= n) {
        bitCount++;
    }

    int maskCount = 1 << bitCount;

    long long *subsetPrimeCount = (long long*)calloc((size_t)maskCount, sizeof(long long));
    long long *exactOrTuples = (long long*)calloc((size_t)maskCount, sizeof(long long));

    for (int value = 2; value <= n; value++) {
        if (ciur[value]) {
            subsetPrimeCount[value] = 1;
        }
    }

    for (int bit = 0; bit < bitCount; bit++) {
        for (int mask = 0; mask < maskCount; mask++) {
            if (mask & (1 << bit)) {
                subsetPrimeCount[mask] += subsetPrimeCount[mask ^ (1 << bit)];
            }
        }
    }

    for (int mask = 0; mask < maskCount; mask++) {
        exactOrTuples[mask] = modPow(subsetPrimeCount[mask] % MOD, k);
    }

    for (int bit = 0; bit < bitCount; bit++) {
        for (int mask = 0; mask < maskCount; mask++) {
            if (mask & (1 << bit)) {
                exactOrTuples[mask] -= exactOrTuples[mask ^ (1 << bit)];
                if (exactOrTuples[mask] < 0) {
                    exactOrTuples[mask] += MOD;
                }
            }
        }
    }

    long long answer = 0;
    for (int value = 2; value <= n; value++) {
        if (ciur[value]) {
            answer += exactOrTuples[value];
            if (answer >= MOD) {
                answer -= MOD;
            }
        }
    }

    printf("%lld\n", answer);

    free(ciur);
    free(subsetPrimeCount);
    free(exactOrTuples);
    return 0;
}
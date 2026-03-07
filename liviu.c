#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MOD 1000000007


// answer variable for both dfs and bitmask solution
static long long ans=0;

// generate prime using Sieve of Eathphenes
void genPrime(int arrPrime[], int n) {
    arrPrime[0]=0;
    arrPrime[1]=0;
    for(int i=2; i <= n; i++) arrPrime[i]=1;
    for (int i = 2; i * i <= n; i++) {
        if (arrPrime[i]) {
            for (int j = i * i; j <= n; j += i)
                arrPrime[j] = 0;
        }
    }
}

// backtracking with dfs solution that generates all 
// the combinations of k primes and checks if their bitwise OR is prime
// (0,0)
// (2,0) (3,0) (5,0) 
// (2,2) (2,3) (2,5) (3,2) (3,3) (3,5) (5,2) (5,3)
void dfs(int sum, int n, int n2, int k, int c, int* arrPrime, int* arrPrime2) {
    // if the sum exceeds n or we have chosen more than k primes, return
    if(sum > n || c > k) {
        return;
    }
    // if we have chosen exactly k primes, check if the sum is prime and if so, increment the answer
    if(c == k) {
        if(arrPrime[sum]) {
            ans++;
            //for(int i=0; i < k; i++)
            //    printf("%d ", arr[i]);
            //printf("\n");
        }
        
        return;
    }
    // iiterate through the primes and choose the next prime to add to the sum
    for(int i=0; i < n2; i++) {
        // add the prime to the sum and continue the dfs
        dfs(sum | arrPrime2[i], n, n2, k, c + 1, arrPrime, arrPrime2);
    }
    return;
}



//modular exponentiation
long long mod_pow(long long base, long long exp) {
    long long ans = 1;
    base %= 1000000007;
    while (exp > 0) {
        if (exp % 2 == 1) {
            ans = (ans * base) % 1000000007;
        }
        base = (base * base) % 1000000007;
        exp /= 2;
    }
    return ans;
}


int main() {

    int n=1000000, k=999983;
    //int n=5, k=2;
    int arrPrime[n + 1], arrPrime2[n];

    // generate the prime numbers and store them in arrPrime based on their index the 
    // values being bool 1 for prime and 0 for not basically boolean array
        genPrime(arrPrime, n);


    // store the primes in arrPrime2
    int idx=0;
    for(int i=2; i <= n; i++) {
        if(arrPrime[i]) {
            arrPrime2[idx++]=i;
        }
    }

    // smallest integer such that 2^bitLimit > n
    int bitLimit=0;
    while((1 << bitLimit) <= n) bitLimit++;


    // mask limit is 2^bitLimit because we will be using bitmasking to represent subsets of primes
    int maskLimit=1 << bitLimit;

    //int arr[k];
    //for(int i=0; i < k; i++) arr[i]=0;
    //dfs(0, n, idx, k, 0, arrPrime, arrPrime2);
    //printf("%d\n", ans);

    // initially 1 for masks that are prime and ≤ n, else 0.
    int* prime_mask=(int*)calloc(maskLimit, sizeof(int));
    // number of primes whose bits are subset of mask
    int* cnt=(int*)calloc(maskLimit, sizeof(int));
    // sum over supersets of (-1)^popcount(prime)
    long long* sup=(long long*)calloc(maskLimit, sizeof(long long)); /

    // for each prime p ≤ n, set prime_mask[p] = 1 and sup[p] = (-1)^(number of bits in p)
    for(int i=2; i <= n; ++i) {
        if(arrPrime[i]) {
            prime_mask[i]=1;
            int bits=__builtin_popcount(i); // counts the number of bits that are 1
            sup[i]=(bits % 2 == 0)?1:MOD - 1;
        }
    }

    // Copy prime_mask into cnt
    for(int i=0; i < maskLimit; i++) cnt[i]=prime_mask[i];

    // zeta transform  calculates for each mask, the number of primes whose bits are subset of mask
    for(int b=0; b < bitLimit; b++) {
        for(int mask=0; mask < maskLimit; mask++) {
            if(mask & (1 << b)) {
                // add the count from the mask without this bit
                cnt[mask]+=cnt[mask ^ (1 << b)];
            }
        }
    }

    // calculate for every mask, the sum over supersets of (-1)^popcount(prime), (all primes that are supersets of the mask)
    for(int b=0; b < bitLimit; b++) {
        for(int mask=0; mask < maskLimit; mask++) {
            if(!(mask & (1 << b))) {
                // add the value from the mask with this bit set
                sup[mask]=(sup[mask] + sup[mask | (1 << b)]) % MOD;
            }
        }
    }

    // calculate the final answer by combining the cnt and sup arrays with the inclusion-exclusion principle
    for(int mask=0; mask < maskLimit; mask++) {
        int c=cnt[mask];
        if(c == 0) continue; // in case there are no primes that are subsets of this mask, skip it
        long long ways=mod_pow(c, k); // c^k ways to choose k primes from the c primes that are subsets of this mask
        long long h=sup[mask]; // sum over supersets of (-1)^popcount(prime) for this mask

        // if the number of bits in the mask is odd, multiply h by -1 (which is equivalent to adding MOD and taking modulo)
        if(__builtin_popcount(mask) & 1) h=(MOD - h) % MOD;

        // add the mask to the answer
        ans=(ans + ways * h) % MOD;
    }

    printf("%lld\n", ans);
}
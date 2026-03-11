#define _DARWIN_C_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define PROGRAM_COUNT 4
#define TEST_COUNT 5
#define REPEAT_COUNT 3

#define INPUT_FILE "bench_input.txt"

typedef struct {
    const char *path;
    const char *name;
} Program;

typedef struct {
    double time_sec;
    double memory_kb;
    int exit_code;
} RunResult;

typedef struct {
    double min_time, max_time, sum_time;
    double min_mem, max_mem, sum_mem;
    int failures;
    int success_count;
} Stats;

static double timespec_diff_sec(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}

static void generate_test_input(int test_id, int *n, int *k) {
    switch (test_id) {
        case 0:
            *n = 100;
            *k = 2;
            break;
        case 1:
            *n = 1000;
            *k = 10;
            break;
        case 2:
            *n = 10000;
            *k = 50;
            break;
        case 3:
            *n = 100000;
            *k = 500;
            break;
        default:
            *n = 1000000;
            *k = 999983;
            break;
    }
}

static int write_input_file(int n, int k) {
    FILE *f = fopen(INPUT_FILE, "w");
    if (!f) {
        perror("fopen");
        return -1;
    }
    fprintf(f, "%d\n%d\n", n, k);
    fclose(f);
    return 0;
}

static double rss_to_kb(long rss_raw) {
    /* pe macOS, ru_maxrss este în bytes */
    return (double)rss_raw / 1024.0;
}

static int run_program(const Program *prog, int n, int k, RunResult *res) {
    struct timespec start, end;
    struct rusage usage;
    int status = 0;

    if (write_input_file(n, k) != 0)
        return -1;

    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("clock_gettime start");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        int in_fd = open(INPUT_FILE, O_RDONLY);
        int out_fd = open("/dev/null", O_WRONLY);

        if (in_fd < 0 || out_fd < 0) {
            perror("open");
            _exit(1);
        }

        if (dup2(in_fd, STDIN_FILENO) < 0) {
            perror("dup2 stdin");
            _exit(1);
        }

        if (dup2(out_fd, STDOUT_FILENO) < 0) {
            perror("dup2 stdout");
            _exit(1);
        }

        if (dup2(out_fd, STDERR_FILENO) < 0) {
            perror("dup2 stderr");
            _exit(1);
        }

        close(in_fd);
        close(out_fd);

        execl(prog->path, prog->path, (char *)NULL);
        perror("execl");
        _exit(1);
    }

    if (wait4(pid, &status, 0, &usage) < 0) {
        perror("wait4");
        return -1;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
        perror("clock_gettime end");
        return -1;
    }

    res->time_sec = timespec_diff_sec(start, end);
    res->memory_kb = rss_to_kb(usage.ru_maxrss);

    if (WIFEXITED(status))
        res->exit_code = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        res->exit_code = 128 + WTERMSIG(status);
    else
        res->exit_code = -1;

    return 0;
}

static void init_stats(Stats *s) {
    s->min_time = 1e100;
    s->max_time = 0.0;
    s->sum_time = 0.0;

    s->min_mem = 1e100;
    s->max_mem = 0.0;
    s->sum_mem = 0.0;

    s->failures = 0;
    s->success_count = 0;
}

static void add_stats(Stats *s, const RunResult *r) {
    if (r->exit_code != 0) {
        s->failures++;
        return;
    }

    if (r->time_sec < s->min_time) s->min_time = r->time_sec;
    if (r->time_sec > s->max_time) s->max_time = r->time_sec;
    s->sum_time += r->time_sec;

    if (r->memory_kb < s->min_mem) s->min_mem = r->memory_kb;
    if (r->memory_kb > s->max_mem) s->max_mem = r->memory_kb;
    s->sum_mem += r->memory_kb;

    s->success_count++;
}

static void print_stats(const Stats *s) {
    if (s->success_count == 0) {
        printf("    No successful runs.\n");
        printf("    Failures: %d\n", s->failures);
        return;
    }

    printf("    Time   -> min: %.9f sec | avg: %.9f sec | max: %.9f sec\n",
           s->min_time, s->sum_time / s->success_count, s->max_time);

    printf("    Memory -> min: %.2f KB | avg: %.2f KB | max: %.2f KB\n",
           s->min_mem, s->sum_mem / s->success_count, s->max_mem);

    printf("    Successes: %d | Failures: %d\n", s->success_count, s->failures);
}

int main(void) {
    Program programs[PROGRAM_COUNT] = {
        {"./denghi", "Gheorghilas Daniel"},
        {"./gheorghe", "Gheorghe"},
        {"./liviu", "Liviu"},
        {"./vlad_solution", "Vlad"}
    };

    printf("=====================================\n");
    printf("BENCHMARK: TIME AND MEMORY ONLY\n");
    printf("=====================================\n\n");

    for (int t = 0; t < TEST_COUNT; t++) {
        int n, k;
        generate_test_input(t, &n, &k);

        printf("=====================================\n");
        printf("TEST %d | n = %d | k = %d\n", t + 1, n, k);
        printf("=====================================\n\n");

        for (int i = 0; i < PROGRAM_COUNT; i++) {
            Stats stats;
            init_stats(&stats);

            printf("Program: %s (%s)\n", programs[i].path, programs[i].name);

            for (int r = 0; r < REPEAT_COUNT; r++) {
                RunResult result;

                if (run_program(&programs[i], n, k, &result) != 0) {
                    printf("  Run %d -> benchmark failed\n", r + 1);
                    stats.failures++;
                    continue;
                }

                printf("  Run %d -> time: %.9f sec | memory: %.2f KB | exit: %d\n",
                       r + 1, result.time_sec, result.memory_kb, result.exit_code);

                add_stats(&stats, &result);
            }

            print_stats(&stats);
            printf("\n");
        }
    }

    return 0;
}
// C++ Sleep Implementation - Minimal and Optimized
// No standard library dependencies - pure Linux kernel syscalls only

#define SYS_write 1
#define SYS_nanosleep 35
#define SYS_exit 60

// timespec structure matching Linux kernel
struct timespec {
    long tv_sec;
    long tv_nsec;
};

// Direct syscall wrappers using inline assembly
static inline long syscall1(long number, long arg1) {
    long result;
    __asm__ __volatile__(
        "syscall"
        : "=a" (result)
        : "a" (number), "D" (arg1)
        : "rcx", "r11", "memory"
    );
    return result;
}

static inline long syscall2(long number, long arg1, long arg2) {
    long result;
    __asm__ __volatile__(
        "syscall"
        : "=a" (result)
        : "a" (number), "D" (arg1), "S" (arg2)
        : "rcx", "r11", "memory"
    );
    return result;
}

static inline long syscall3(long number, long arg1, long arg2, long arg3) {
    long result;
    __asm__ __volatile__(
        "syscall"
        : "=a" (result)
        : "a" (number), "D" (arg1), "S" (arg2), "d" (arg3)
        : "rcx", "r11", "memory"
    );
    return result;
}

// Minimal strlen implementation
static inline long strlen(const char* string) {
    const char* cursor = string;
    while (*cursor) {
        cursor++;
    }
    return cursor - string;
}

// Print function using write syscall
static inline void print(const char* string) {
    syscall3(SYS_write, 1, (long)string, strlen(string));
}

// Integer parsing
static inline long parse_int(const char* raw_int) {
    long result = 0;
    const char* cursor = raw_int;
    
    while (*cursor >= '0' && *cursor <= '9') {
        result = result * 10 + (*cursor - '0');
        cursor++;
    }
    
    return result;
}

// Sleep function using nanosleep syscall
static inline void sleep(long seconds) {
    timespec duration = {0};
    duration.tv_sec = seconds;
    
    syscall2(SYS_nanosleep, (long)(&duration), 0);
}

#ifdef __cplusplus
extern "C" {
#endif

// Custom exit function
void exit(int code) {
    syscall1(SYS_exit, code);
    for(;;) {} // Should never reach here
}

#ifdef __cplusplus
}
#endif

// Main function
extern "C" int main(int argc, char* argv[]) {
    if (argc != 2) {
        print("Usage: sleep NUMBER\nPause for NUMBER seconds\n");
        return 1;
    }

    char* raw_seconds = argv[1];
    long seconds = parse_int(raw_seconds);

    print("Sleeping for ");
    print(raw_seconds);
    print(" seconds\n");
    
    sleep(seconds);

    return 0;
}

// Entry point - replaces C runtime startup
extern "C" void _start() __attribute__((naked));

extern "C" void _start() {
    __asm__ __volatile__(
        "xor %ebp, %ebp\n"
        "mov (%rsp), %rdi\n"
        "lea 8(%rsp), %rsi\n"
        "and $-16, %rsp\n"
        "call main\n"
        "mov %rax, %rdi\n"
        "call exit\n"
    );
}

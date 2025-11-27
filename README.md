# malloc42
Custom drop-in, thread-safe implementation of `malloc(3)` and friends
for 42 School projects and general UNIX development.

```
┌─Why another malloc?────────────────────────────────────────────┐
│ • Understand how the system allocator works internally.        │
│ • Learn page management, meta-data packing, and alignment.     ││
│ • Provide the mandatory `show_alloc_mem(3)` debug interface.   │
└────────────────────────────────────────────────────────────────┘
```

malloc42 overrides the standard allocator at runtime (via
`LD_PRELOAD`/`DYLD_INSERT_LIBRARIES`) and offers three segregated
zones (TINY, SMALL, LARGE) with O(1) free-list insertion, coalescing,
and full page reuse.
It is compliant with the 42 Subject and passes the original unit-tests
as well as the extended stress-suite shipped in this repository.

---

## Contents
1. Features
2. Quick Start / Installation
3. Usage & Examples
4. Runtime Configuration
5. Repository Layout
6. Internal API (public headers)
7. Contributing
8. License

---

## 1. Features

| Capability                       | Details

                                     |
| -------------------------------- |
---------------------------------------------------------------------------------------------------------------------------------------------
|
| Segregated Fit allocator         | Three size classes:<br>• **TINY**
≤ 128 B<br>• **SMALL** ≤ 1024 B<br>• **LARGE** > 1024 B (one mmap()
each)                                   |
| Constant-time free-list ops      | Doubly-linked intrusive lists,
pointer-tagging to avoid extra bookkeeping
                                        |
| Page recycling                   | Empty pages are `munmap()`-ed
automatically to return memory to the kernel
                                         |
| Thread Safety                    | One mutex per zone list –
fine-grained locking to keep contention low
                                            |
| `calloc` zero-fill optimisation  | `mmap()`’s implicit zero pages
whenever possible (Linux MAP_ANON / macOS MAP_ANON)
                                        |
| 🔍 `show_alloc_mem{,_ex}`        | Prints human-readable / extended
(hex + ASCII) snapshots of all current allocations
                                      |
| Pluggable statistics             | Optional HTML/CSV report via
`MALLOC42_STATS=on` environment variable
                                          |
| 100 % Valgrind/ASAN clean        | allocator tested under Valgrind &
llvm-asan with zero leaks / invalid accesses
                                     |
| Portable                         | macOS ≥ 10.14 & Linux ≥ 2.27,
clang ≥ 10 or gcc ≥ 9
                                         |

---

## 2. Quick Start / Installation

Prerequisites
• `make` (GNU/bsd)
• `clang` or `gcc` with C11 support
• `pthread` (usually part of libc)

```
git clone https://github.com/apitoise/malloc42.git
cd malloc42
make            # builds libftmalloc.so / libftmalloc.dylib
sudo make install  # optional – installs to /usr/local/lib and header
to /usr/local/include
```

### Injecting the allocator
Run any existing binary with the new allocator without relinking:

Linux:
```bash
LD_PRELOAD=$PWD/libftmalloc.so ./your_program
```
macOS:
```bash
export DYLD_INSERT_LIBRARIES=$PWD/libftmalloc.dylib
./your_program
```

To link _statically_ inside your own program:
```bash
gcc -o myapp myapp.c -I/path/to/malloc42/include -L/path/to/malloc42
-lftmalloc -pthread
```

---

## 3. Usage & Examples

### Basic allocation
```c
#include <stdlib.h>
#include "malloc42.h"      // adds show_alloc_mem()

int main(void)
{
    char *s = malloc(42);
    int  *a = calloc(10, sizeof *a);
    s = realloc(s, 128);
    free(a);
    free(s);

    show_alloc_mem();      // debug snapshot

    return 0;
}
```

Compile & run with allocator pre-loaded:
```bash
LD_PRELOAD=./libftmalloc.so gcc -Wall test.c -o test && ./test
```

### Debug output
```
TINY : 0x7fe4a6aa0000 - 0x7fe4a6ab0000 (64 pages)
   0x7fe4a6aa0020 - 0x7fe4a6aa004a : 42 bytes
SMALL: 0x7fe4a6ab0000 - 0x7fe4a6ad0000 (128 pages)
LARGE: 0x7fe4a6ae0000 - 0x7fe4a6ae1000 : 4096 bytes
Total : 4138 bytes
```

---

## 4. Runtime Configuration

All knobs are environment variables evaluated during `malloc_init()`:

| Variable             | Default | Meaning
                     |
| -------------------- | ------- |
-------------------------------------------------------- |
| `MALLOC42_TINY`      | 128     | upper bound (bytes) of TINY class
                    |
| `MALLOC42_SMALL`     | 1024    | upper bound (bytes) of SMALL class
                    |
| `MALLOC42_VERBOSE`   | 0       | 0=off, 1=errors, 2=info, 3=debug
                    |
| `MALLOC42_STATS`     | off     | `on` ⇒ emit
`malloc42_stats.<pid>.html` at exit          |
| `MALLOC42_COLOR`     | auto    | `on`/`off` terminal ANSI coloring
for `show_alloc_mem`   |

Example:
```bash
MALLOC42_VERBOSE=2 MALLOC42_STATS=on LD_PRELOAD=./libftmalloc.so ./heavy_test
```

---

## 5. Repository Layout

```
malloc42/
├─ include/
│  └─ malloc42.h            # Public header (malloc, free, show_alloc_mem)
├─ src/
│  ├─ malloc.c              # Entry points & zone selection
│  ├─ free.c
│  ├─ realloc.c
│  ├─ calloc.c
│  ├─ zone.c                # mmap/munmap handling
│  ├─ list.c                # intrusive list helpers
│  ├─ show_alloc_mem.c
│  └─ utils.c               # alignment, logging, stats
├─ tests/
│  ├─ basic.c
│  ├─ thread_stress.c
│  └─ leak_sentinel.c
├─ Makefile
└─ README.md                # you are here
```

---

## 6. Internal API (include/malloc42.h)

```
/* Mandatory 42 interface */
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);

/* Debug helpers */
void show_alloc_mem(void);
void show_alloc_mem_ex(void);   /* with hex/ASCII dump */

/* Statistics */
struct malloc42_stats {
    size_t  allocated;          /* total user bytes */
    size_t  overhead;           /* meta-data bytes */
    size_t  mmap_calls;
};
const struct malloc42_stats *malloc42_get_stats(void);
```

All functions are `__attribute__((visibility("default")))` and
override the glibc/Libsystem equivalents at preload time.

---

## 7. Contributing

1. Fork the repo and create your feature branch:
   `git checkout -b feat/my-awesome-idea`
2. Follow the 42 norme (`norminette` must pass).
3. Add tests in `tests/` and run `make test` (requires python3 & pytest).
4. Commit using Conventional Commits style and open a PR.
5. Be nice; we review in <72 h.

Bug reports and performance benchmarks are extremely welcome!

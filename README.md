# procon

## Installation

In CMake:
```cmake
include(FetchContent)
FetchContent_Declare(
    libprocon
    GIT_REPOSITORY https://github.com/multimeric/libprocon.git
)
FetchContent_MakeAvailable(libprocon)
target_link_libraries(<MY TARGET> PUBLIC libprocon)
```

Then in C++:
```C++
#include "procon.h"
```

## Usage

Currently the API consists of one single function: `proc_iter()`, which is a generator function that yields events:

```C++
#include <stdio.h>
#include "procon.h"

int main(){
    // Continually listen for events
    for (auto event: proc_iter()){
        switch(event->what){
            // Whenever a process exits, log it to stdout
            case PROC_EVENT_EXIT:
                {
                    auto exit = event->event_data.exit;
                    std::cout << "PID: " << exit.process_pid << " exited with code " << exit.exit_code << "\n";
                }
            break;

            default:
            break;
        }
    }
    return 0;
}
```

## FAQ

### Why use the process connector API instead of an alternative?

There aren't any alternative Linux APIs for process monitoring that meet the criteria I want:
* Don't use polling
* Can be run without root
* Doesn't require unusual Linux kernel or system configuration
* Returns the exit code and signals
* Doesn't have to spawn the watched processes itself

[`waitid()` and friends](https://man7.org/linux/man-pages/man2/waitpid.2.html) fail the last criteria: they can only be used on child processes.
[Process accounting requires root to specifically enable it](https://man7.org/linux/man-pages/man8/accton.8.html).

### Why C++?

C already has a process connector library: the kernel's `cn_proc`. Rust is possibly a better language for this, but [I had some `bindgen` issues](https://github.com/yorodm/cnproc-rs/issues/9), so moved to C++.

### Which Linux kernels support this?

[`cn_proc` was enabled for non-root users in Linux 6.6](https://patchwork.kernel.org/project/netdevbpf/patch/20230719201821.495037-6-anjali.k.kulkarni@oracle.com/) (Thank you to the author and Oracle for supporting this!). This was released in October 2023. Technically you can use this library on earlier kernels, but if you have root available, there are better tools than this.

### Why use generators?

This was the neatest API I could design.
Using a single "receive" function would mean creating and destroying the socket each invocation, whereas a generator can retain its state.
I could export an array of functions for subscribing and then reading, but this isn't providing much abstraction and isn't very user friendly.
Using an iterator means more boilerplate and less elegant usage, especially if you don't want want to listen forever.
Finally, using a generator means easy conversion to async/multithreaded support, without breaking the API in the future.

### Why require such a high C++ standard?

In short, I wanted an elegant API and elegant implementation.
C++ 20 gives me generators (see above), designated initializers etc.
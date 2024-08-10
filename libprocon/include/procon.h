// By including a custom copy of the cn_proc header instead of the system one,
// this can compile on Linux versions 6.6 when the cn_proc was enabled for non-root users (bfdfdc2f3b7073571ce40b6d19193002b0126b6a)
// up until 6.10, when this was fixed (06e785aeb9ea8a43d0a3967c1ba6e69d758e82d4)
// We include the header in our own header so that users only need a single header
#include "cn_proc.h"
#include <coro/coro.hpp>
/// @brief Continually yields process events
/// @return A generator of proc event pointers.
coro::generator<proc_event*> proc_iter();
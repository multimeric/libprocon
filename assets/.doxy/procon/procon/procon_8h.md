

# File procon.h



[**FileList**](files.md) **>** [**include**](dir_6c35e630e6a271fb2148d779da44ad7c.md) **>** [**procon.h**](procon_8h.md)

[Go to the source code of this file](procon_8h_source.md)



* `#include "cn_proc.h"`
* `#include <coro/coro.hpp>`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  coro::generator&lt; proc\_event \* &gt; | [**proc\_iter**](#function-proc_iter) () <br>_Continually yields process events._  |




























## Public Functions Documentation




### function proc\_iter 

_Continually yields process events._ 
```C++
coro::generator< proc_event * > proc_iter () 
```





**Returns:**

A generator of proc event pointers. 





        

<hr>

------------------------------
The documentation for this class was generated from the following file `libprocon/include/procon.h`


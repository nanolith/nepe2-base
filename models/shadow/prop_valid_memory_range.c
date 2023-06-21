#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

bool prop_valid_memory_range(const void* data, size_t size)
{
    const uint8_t* ub = (const uint8_t*)data;

    if (size > 0)
    {
        return
            ub[0] == ub[0]
         && ub[size - 1] == ub[size - 1];
    }
}

#include <rcpr/resource/protected.h>

status FN_DECL_MUST_CHECK
RCPR_SYM(resource_release)(RCPR_SYM(resource)* r)
{
    return r->release(r);
}

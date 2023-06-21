#include <rcpr/resource/protected.h>

void
RCPR_SYM(resource_init)(
    RCPR_SYM(resource)* r, RCPR_SYM(resource_release_fn) release)
{
    RCPR_MODEL_ASSERT(NULL != r);
    RCPR_MODEL_ASSERT(NULL != release);

    r->release = release;
}

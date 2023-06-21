CBMC?=cbmc

ALL:
	$(CBMC) --bounds-check --pointer-check --memory-leak-check \
	--div-by-zero-check --signed-overflow-check --unsigned-overflow-check \
    --pointer-overflow-check --conversion-check --undefined-shift-check \
    --enum-range-check --pointer-primitive-check --malloc-may-fail \
    --malloc-fail-null --string-abstraction --havoc-undefined-functions \
    --trace --stop-on-fail -DCBMC --drop-unused-functions \
	--unwind 10 --unwinding-assertions \
	-I ../include -I ../build/include -I $(RCPR_INCLUDEDIR) -I . \
	../models/shadow/nepe2/secure_buffer/secure_buffer_struct_tag_init.c \
	../models/shadow/nepe2/secure_buffer/prop_secure_buffer_valid.c \
	../models/shadow/rcpr/allocator_allocate.c \
	../models/shadow/rcpr/allocator_reclaim.c \
	../models/shadow/rcpr/allocator_resource_handle.c \
	../models/shadow/rcpr/allocator_resource_handle.c \
	../models/shadow/rcpr/malloc_allocator_create.c \
	../models/shadow/rcpr/prop_allocator_valid.c \
	../models/shadow/rcpr/resource_init.c \
	../models/shadow/rcpr/resource_release.c \
	../models/shadow/prop_valid_memory_range.c \
	../src/secure_buffer/secure_buffer_create.c \
	../src/secure_buffer/secure_buffer_data.c \
	../src/secure_buffer/secure_buffer_resource_handle.c \
	../src/secure_buffer/secure_buffer_resource_release.c \
	../models/secure_buffer_create_main.c

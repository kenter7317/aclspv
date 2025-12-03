#include <aclspv/obj.h>

ACLSPV_ABI_IMPL
void	aclspv_obj_stop(const h_aclspv_obj_t hs) {
	LLVMDisposeModule(hs);
}

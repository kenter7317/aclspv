#include "./ctx.h"

#include <llvm-c/Core.h>
#include <string.h>
#include <assert.h>

#include <aclspv/argknd.h>
#include <aclspv/md.h>

#define md_kind_str aclspv_md_argknd

#define ZSTR(a)	0

ae2f_retnew static const char* get_arg_kind(const LLVMValueRef param) {
    const LLVMTypeRef type = LLVMTypeOf(param);

 
    if (LLVMGetTypeKind(type) == LLVMPointerTypeKind) {

        const LLVMTypeRef pointee_type = LLVMGetElementType(type);
	assert(pointee_type);

	/* is `void*`? */
	if(LLVMPointerTypeIsOpaque(type)) {
		return ACLSPV_ARGKND_POD;
	}

        if (LLVMGetTypeKind(pointee_type) == LLVMStructTypeKind) {
		const char* ae2f_restrict const name = LLVMGetStructName(pointee_type);

		if (name) {
			if (strstr(name, "image2d_t") || strstr(name, "image3d_t")) {
				return strstr(name, "write_only") ?
					ACLSPV_ARGKND_WO_IMG : ACLSPV_ARGKND_RO_IMG;
			}
			if (strstr(name, "sampler_t")) {
				return ACLSPV_ARGKND_SMPLR;
			}
		} else {
			assert(0);
		}
	}
    }


    return ACLSPV_ARGKND_POD;
}

	ae2f_noexcept ACLSPV_ABI_IMPL e_fn_aclspv_pass
aclspv_pass_arg_anal(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX	
		)
{
	const LLVMContextRef C = LLVMGetModuleContext(M);
	const unsigned kind_id =
		LLVMGetMDKindIDInContext(C, md_kind_str, sizeof(md_kind_str) - 1);

	LLVMValueRef F, FNxt;

	for (F = LLVMGetFirstFunction(M); F; F = FNxt) {
		FNxt = LLVMGetNextFunction(F);

		if (LLVMGetFunctionCallConv(F) != LLVMSPIRKERNELCallConv) {
			continue;
		}

#define kern_fn F
		{
			const unsigned nprms = LLVMCountParams(kern_fn);
			unsigned j;

			unless (nprms) {
				LLVMGlobalSetMetadata(
						kern_fn,
						kind_id,
						LLVMMDNodeInContext2(C, ae2f_NIL, 0)
						);
				continue;
			}

			_aclspv_grow_vec(
					_aclspv_malloc,
					_aclspv_free,
					CTX->m_v0,
					(size_t)(sizeof(LLVMMetadataRef) * nprms)
					);

			_aclspv_grow_vec(
					_aclspv_malloc,
					_aclspv_free,
					CTX->m_v1,
					(size_t)(sizeof(LLVMValueRef) * nprms)
					);
#define arg_kind_mds	ae2f_static_cast(LLVMMetadataRef* ae2f_restrict, CTX->m_v0.m_p)
#define prms		ae2f_static_cast(LLVMValueRef* ae2f_restrict, CTX->m_v1.m_p)

			unless (arg_kind_mds && prms)
				return FN_ACLSPV_PASS_ALLOC_FAILED;

			assert(nprms * sizeof(LLVMValueRef) == CTX->m_v1.m_sz);

			LLVMGetParams(kern_fn, prms);


			for (j = nprms; j--; ) {
				const char* ae2f_restrict kind_str;
				assert(j < nprms);
				kind_str = get_arg_kind(prms[j]);
				arg_kind_mds[j] =
					LLVMMDStringInContext2(C, kind_str, strlen(kind_str));
			}

			LLVMGlobalSetMetadata(
					kern_fn,
					kind_id,
					LLVMMDNodeInContext2(C, arg_kind_mds, (size_t)nprms)
					);
		}
#undef kern_fn
#undef arg_kind_mds
#undef prms
	}

	return FN_ACLSPV_PASS_OK;
}

#include "./ctx.h"

#include <llvm-c/Core.h>
#include <string.h>
#include <aclspv/md.h>

#define md_kind_str	aclspv_md_args
	ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass
aclspv_pass_add_kern_metadata(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
	const LLVMContextRef C = LLVMGetModuleContext(M);
	const unsigned kind_id =
		LLVMGetMDKindIDInContext(C, md_kind_str, sizeof(md_kind_str) - 1);

	LLVMValueRef	F, FNxt;

	(void)CTX;


	for (F = LLVMGetFirstFunction(M); F; F = FNxt) {
		FNxt = LLVMGetNextFunction(F);

		if (LLVMGetFunctionCallConv(F) != LLVMSPIRKERNELCallConv) {
			continue;
		}

#define kern_fn	F
		{
			const unsigned	nprms = LLVMCountParams(kern_fn);
			unsigned	j;

			unless (nprms) {
				LLVMGlobalSetMetadata(
						kern_fn,
						kind_id,
						LLVMMDNodeInContext2(C, ae2f_NIL, 0)
						);
				continue;
			}

			_aclspv_grow_vec(
					_aclspv_malloc
					, _aclspv_free
					, CTX->m_v0
					, (size_t)(sizeof(LLVMMetadataRef) * nprms)
					);

			_aclspv_grow_vec(
					_aclspv_malloc
					, _aclspv_free
					, CTX->m_v1
					, (size_t)(sizeof(LLVMValueRef) * nprms)
					);

#define arg_type_mds	ae2f_static_cast(LLVMMetadataRef* ae2f_restrict,	CTX->m_v0.m_p)
#define prms		ae2f_static_cast(LLVMValueRef* ae2f_restrict,		CTX->m_v1.m_p)

			unless (arg_type_mds)
				return FN_ACLSPV_PASS_ALLOC_FAILED;
			unless (prms) {
				return FN_ACLSPV_PASS_ALLOC_FAILED;
			}

			LLVMGetParams(kern_fn, prms);

			for (j = nprms; j--; ) {
				const LLVMTypeRef param_type = LLVMTypeOf(prms[j]);
				char* const type_str = LLVMPrintTypeToString(param_type);

				arg_type_mds[j] =
					LLVMMDStringInContext2(C, type_str, strlen(type_str));
				LLVMDisposeMessage(type_str);
			}

			LLVMGlobalSetMetadata(
					kern_fn,
					kind_id,
					LLVMMDNodeInContext2(C, arg_type_mds, (size_t)nprms)
					);
		}
	}

	return FN_ACLSPV_PASS_OK;
}

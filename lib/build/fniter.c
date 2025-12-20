#include <build.h>
#include "./wrdemit.h"
#include <pass/md.h>
#include <assert.h>
#include "./entp.h"

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass	aclspv_build_fniter(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	LLVMValueRef	func;
	aclspv_id_t	i_kernel, num_kernels = 0;
	i_kernel = 0;

	for (func = LLVMGetFirstFunction(M); func; func = LLVMGetNextFunction(func)) {
		if (LLVMGetMetadata(func, LLVMGetMDKindID(
						ACLSPV_MD_PIPELINE_LAYOUT
						, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1))

				&& LLVMGetFunctionCallConv(func) == LLVMSPIRKERNELCallConv
		   )
		{
			++num_kernels;
		}
	}

	CTX->m_fnlist.m_num_entp = num_kernels;
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free
			, CTX->m_fnlist.m_entp, (size_t)(sizeof(lib_build_entp_t) * num_kernels)
			);


	for (func = LLVMGetFirstFunction(M); func; func = LLVMGetNextFunction(func)) {
		if (LLVMGetMetadata(func, LLVMGetMDKindID(
						ACLSPV_MD_PIPELINE_LAYOUT
						, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1))

				&& LLVMGetFunctionCallConv(func) == LLVMSPIRKERNELCallConv
		   )
		{
			assert(i_kernel < num_kernels);
			((lib_build_entp_t* ae2f_restrict)(CTX)->m_fnlist.m_entp.m_p)[i_kernel].m_fn = func;
			((lib_build_entp_t* ae2f_restrict)(CTX)->m_fnlist.m_entp.m_p)[i_kernel].m_id
				= CTX->m_id + i_kernel;

			++i_kernel;
		}
	}

	assert(i_kernel == num_kernels);
	CTX->m_id += num_kernels;

#if	0
	unless(num_kernels)
		return FN_ACLSPV_PASS_OK;
#endif

	return FN_ACLSPV_PASS_OK;
}

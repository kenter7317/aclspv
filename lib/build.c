#include "./ctx.h"
#include <aclspv/build.h>


/**
 * @fn		aclspv_build_spv_emit
 * @brief	
 * @param	M
 * @param	CTX_RET	`CTX_RET->m_v0` will be considered as return value
 * */
ae2f_inline static e_fn_aclspv_pass aclspv_build_spv_emit(
		const LLVMModuleRef	M,
		h_aclspv_pass_ctx	CTX_RET
		);

ACLSPV_ABI_IMPL ae2f_retnew uint32_t* aclspv_build(
		h_aclspv_lnker_t			h_lnk,
		e_aclspv_build* ae2f_restrict const	wr_opt,
		e_fn_aclspv_pass* ae2f_restrict	const	wr_pass_opt,
		e_aclspv_passes* ae2f_restrict const	wr_pass_wh_opt,
		uintptr_t* ae2f_restrict const		rwr_ret_size
		) 
{
	struct x_aclspv_pass_ctx contxt = {
		{ae2f_NIL, 0},
		{ae2f_NIL, 0},
		{ae2f_NIL, 0}
	};

	e_aclspv_build		r_build = ACLSPV_BUILD_OK;
	e_aclspv_passes		r_wh = ACLSPV_PASSES_OK;
	e_fn_aclspv_pass	r_fn = FN_ACLSPV_PASS_OK;
	uint32_t* ae2f_restrict	ret = ae2f_NIL;
	uintptr_t		ret_size = 0;

	unless(h_lnk) {
		r_build = ACLSPV_BUILD_ARG_NIL;
		goto END;
	}

	r_wh = aclspv_runall_module_passes(
			h_lnk->m_module
			, &contxt
			, &r_fn
			);

	if(r_wh || wr_pass_opt) {
		r_build = ACLSPV_BUILD_ERR_FROM_PASS;
		goto END;
	}

	if((r_fn = aclspv_build_spv_emit(
					h_lnk->m_module
					, &contxt
					))
			)
	{
		r_build = ACLSPV_BUILD_ERR_FROM_PASS;
		r_wh = ACLSPV_PASSES_SPV_EMIT;
		goto END;
	} else {
		ret = contxt.m_v0.m_p;
		ret_size = (uintptr_t)contxt.m_v0.m_sz / sizeof(uint32_t);

		contxt.m_v0.m_p = ae2f_NIL;
		contxt.m_v0.m_sz = 0;
	}

END:
	_aclspv_stop_vec(_aclspv_free, contxt.m_v0);
	_aclspv_stop_vec(_aclspv_free, contxt.m_v1);
	_aclspv_stop_vec(_aclspv_free, contxt.m_v2);

	if(wr_pass_wh_opt) {
		*wr_pass_wh_opt = r_wh;
	}

	if(wr_opt) {
		*wr_opt = r_build;
	}
	
	if(rwr_ret_size) {
		*rwr_ret_size = ret_size;
	}

	if(wr_pass_opt) {
		*wr_pass_opt = r_fn;
	}

	return ret;
}


ae2f_inline static e_fn_aclspv_pass aclspv_build_spv_emit(
		const LLVMModuleRef	M,
		h_aclspv_pass_ctx	CTX_RET
		) 
{
	IGNORE(M);
	IGNORE(CTX_RET);

	return FN_ACLSPV_PASS_OK;
}

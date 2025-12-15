#include "./ctx.h"
#include <aclspv/pass.h>
#include <aclspv/build.h>

ACLSPV_ABI_IMPL ae2f_retnew aclspv_wrd_t* aclspv_build(
		h_aclspv_lnker_t			h_lnk,
		e_aclspv_build* ae2f_restrict const	wr_opt,
		e_fn_aclspv_pass* ae2f_restrict	const	wr_pass_opt,
		e_aclspv_passes* ae2f_restrict const	wr_pass_wh_opt,
		uintptr_t* ae2f_restrict const		rwr_ret_count
		) 
{
	struct x_aclspv_pass_ctx contxt = {
		{ae2f_NIL, 0},
		{ae2f_NIL, 0},
		{ae2f_NIL, 0},
		{ae2f_NIL, 0}
	};

	e_aclspv_build		r_build = ACLSPV_BUILD_OK;
	e_aclspv_passes		r_wh = ACLSPV_PASSES_OK;
	e_fn_aclspv_pass	r_fn = FN_ACLSPV_PASS_OK;

	unless(h_lnk) {
		r_build = ACLSPV_BUILD_ARG_NIL;
		goto END;
	}

	r_wh = aclspv_runall_module_passes(
			h_lnk->m_module
			, &contxt
			, &r_fn
			);

	if(r_wh || r_fn) {
		r_build = ACLSPV_BUILD_ERR_FROM_PASS;
		goto END;
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
	
	if(rwr_ret_count) {
		*rwr_ret_count = (uintptr_t)(contxt).m_ret.m_sz / sizeof(aclspv_wrd_t);
	}

	if(wr_pass_opt) {
		*wr_pass_opt = r_fn;
	}

	return contxt.m_ret.m_p;
}

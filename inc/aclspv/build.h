#ifndef	aclspv_build_h
#define	aclspv_build_h

#include "./pass.h"
#include "./lnker.h"
#include "./spvty.h"

/**
 * @enum	e_aclspv_build
 * @brief	state number for `aclspv_build`
 * */
typedef enum {
	ACLSPV_BUILD_OK,
	ACLSPV_BUILD_ARG_NIL,
	ACLSPV_BUILD_ERR_FROM_PASS
} e_aclspv_build;

ae2f_extern ACLSPV_ABI_DECL ae2f_retnew
aclspv_wrd_t* aclspv_build(
		h_aclspv_lnker_t			h_lnk,
		e_aclspv_build* ae2f_restrict const	wr_opt,
		e_fn_aclspv_pass* ae2f_restrict	const	wr_pass_opt,
		e_aclspv_passes* ae2f_restrict const	wr_pass_wh_opt,
		uintptr_t* ae2f_restrict const		rwr_ret_count
		);

#endif

#include <ae2f/Keys.h>
#include <clang-c/Index.h>
#include <assert.h>

#include <util/ctx.h>
#include <util/wrdemit.h>
#include <util/is_kernel.h>
#include <util/fn.h>
#include <util/entp.h>

#include <ae2f/c90/StdBool.h>
#include <spirv/1.0/spirv.h>


static enum CXChildVisitResult emit_decl_glob_obj(CXCursor h_cur, CXCursor h_cur_parent, CXClientData _h_ctx) {
#define h_ctx	((x_aclspv_ctx* ae2f_restrict)_h_ctx)
	assert(h_ctx); 

	if(h_cur.kind == CXCursor_CompoundStmt && h_cur_parent.kind == CXCursor_FunctionDecl) {

		if(util_is_kernel(h_cur_parent)) {
			((lib_build_entp_t* ae2f_restrict)h_ctx->m_fnlist.m_entp.m_p)[h_ctx->m_fnlist.m_num_fn]
				.m_fn = h_cur_parent;
			((lib_build_entp_t* ae2f_restrict)h_ctx->m_fnlist.m_entp.m_p)[h_ctx->m_fnlist.m_num_fn]
				.m_id = h_ctx->m_id + h_ctx->m_fnlist.m_num_entp;

			((lib_build_entp_t* ae2f_restrict)h_ctx->m_fnlist.m_entp.m_p)[h_ctx->m_fnlist.m_num_fn]
				.m_id = h_ctx->m_id + h_ctx->m_fnlist.m_num_entp;

			++h_ctx->m_fnlist.m_num_entp;
		} else {
			((lib_build_fn_t* ae2f_restrict)h_ctx->m_fnlist.m_fn.m_p)[h_ctx->m_fnlist.m_num_fn]
				.m_fn = h_cur_parent;
			((lib_build_fn_t* ae2f_restrict)h_ctx->m_fnlist.m_fn.m_p)[h_ctx->m_fnlist.m_num_fn]
				.m_id = h_ctx->m_id 
				+ (aclspv_wrd_t)((aclspv_wrd_t)h_ctx->m_fnlist.m_entp.m_sz / sizeof(lib_build_entp_t))
				+ h_ctx->m_fnlist.m_num_fn;

			++h_ctx->m_fnlist.m_num_fn;
		}

		return CXChildVisit_Continue;
	}

#undef	h_ctx
	return CXChildVisit_Recurse;
}

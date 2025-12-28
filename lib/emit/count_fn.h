#include <ae2f/Keys.h>
#include <clang-c/Index.h>
#include <assert.h>

#include <util/ctx.h>
#include <util/wrdemit.h>
#include <util/is_kernel.h>

#include <ae2f/c90/StdBool.h>
#include <spirv/1.0/spirv.h>


static enum CXChildVisitResult emit_count_fn(CXCursor h_cur, CXCursor h_cur_parent, CXClientData _h_ctx) {
#define h_ctx	((x_aclspv_ctx* ae2f_restrict)_h_ctx)
	assert(h_ctx);

	unless(h_cur.kind == CXCursor_CompoundStmt && h_cur_parent.kind == CXCursor_FunctionDecl)
		return CXChildVisit_Recurse;

	if(util_is_kernel(h_cur_parent)) {
		const CXString NAME = clang_getCursorSpelling(h_cur_parent);
		aclspv_wrdcount_t	POS = h_ctx->m_count.m_entp;
		h_ctx->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

		/** OpEntryPoint */
		unless((h_ctx->m_count.m_entp = emit_opcode(
						&h_ctx->m_section.m_entp
						, h_ctx->m_count.m_entp
						, SpvOpEntryPoint, 0)))
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_entp = emit_wrd(
						&h_ctx->m_section.m_entp
						, h_ctx->m_count.m_entp
						, SpvExecutionModelGLCompute)))
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_entp = emit_wrd(
						&h_ctx->m_section.m_entp
						, h_ctx->m_count.m_entp
						, h_ctx->m_id + h_ctx->m_fnlist.m_num_entp)))
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_entp = emit_str(
						&h_ctx->m_section.m_entp
						, h_ctx->m_count.m_entp
						, NAME.data))) 
			goto LBL_ABRT;

		set_oprnd_count_for_opcode(get_wrd_of_vec(&h_ctx->m_section.m_entp)[POS], h_ctx->m_count.m_entp - POS - 1);

		/** OpExecMode */
		unless((h_ctx->m_count.m_execmode = 
					emit_opcode(
						&h_ctx->m_section.m_execmode
						, h_ctx->m_count.m_execmode
						, SpvOpExecutionMode
						, 5)))	
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_execmode = 
					emit_wrd(&h_ctx->m_section.m_execmode
						, h_ctx->m_count.m_execmode
						, h_ctx->m_id + h_ctx->m_fnlist.m_num_entp)))			
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_execmode = 
					emit_wrd(&h_ctx->m_section.m_execmode
						, h_ctx->m_count.m_execmode
						, SpvExecutionModeLocalSize)))
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_execmode = 
					emit_wrd(&h_ctx->m_section.m_execmode
						, h_ctx->m_count.m_execmode
						, 1)))
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_execmode = 
					emit_wrd(&h_ctx->m_section.m_execmode
						, h_ctx->m_count.m_execmode
						, 1)))
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_execmode = 
					emit_wrd(&h_ctx->m_section.m_execmode
						, h_ctx->m_count.m_execmode
						, 1)))
			goto LBL_ABRT;

#if !defined(NDEBUG) || !NDEBUG
		POS = h_ctx->m_count.m_name;
		unless((h_ctx->m_count.m_name = emit_opcode(&h_ctx->m_section.m_name, h_ctx->m_count.m_name, SpvOpName, 0)))			goto LBL_ABRT;
		unless((h_ctx->m_count.m_name = emit_wrd(&h_ctx->m_section.m_name, h_ctx->m_count.m_name, h_ctx->m_id + h_ctx->m_fnlist.m_num_entp)))				goto LBL_ABRT;
		unless((h_ctx->m_count.m_name = emit_str(&h_ctx->m_section.m_name, h_ctx->m_count.m_name, NAME.data)))	goto LBL_ABRT;
		set_oprnd_count_for_opcode(get_wrd_of_vec(&h_ctx->m_section.m_name)[POS], h_ctx->m_count.m_name - POS - 1);
#endif

		++h_ctx->m_fnlist.m_num_entp;

		h_ctx->m_state = ACLSPV_COMPILE_OK;
LBL_ABRT:
		clang_disposeString(NAME);
	} else {
		++h_ctx->m_fnlist.m_num_fn;
	}

	return CXChildVisit_Continue;

	return CXChildVisit_Break;



#undef	h_ctx
}

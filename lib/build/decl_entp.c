#include <build.h>
#include "./wrdemit.h"
#include "./entp.h"

#include <spirv/1.0/spirv.h>

#define	BADALLOC	FN_ACLSPV_PASS_ALLOC_FAILED

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass aclspv_build_decl_entp(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	aclspv_wrd_t	i;
	IGNORE(M);

	for(i = CTX->m_fnlist.m_num_entp; i--;) {
		const lib_build_entp_t	entp
			= ((lib_build_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[i];
		const char* ae2f_restrict	
			name = LLVMGetValueName(entp.m_fn);
#define m_ret		m_section.m_entp
#define	ret_count	CTX->m_count.m_entp
		spvsz_t pos = ret_count;

		/** OpEntryPoint */
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpEntryPoint, 0)))
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvExecutionModelGLCompute)))
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, entp.m_id)))
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_str(&CTX->m_ret, ret_count, name))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;

		set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[pos], ret_count - pos - 1);

#undef	m_ret
#undef	ret_count
#define	m_ret		m_section.m_execmode
#define	ret_count	CTX->m_count.m_execmode

		/** OpExecMode */
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpExecutionMode, 5)))	return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, entp.m_id)))			return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvExecutionModeLocalSize)))	return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1)))				return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1)))				return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1)))				return BADALLOC;


#if !defined(NDEBUG) || !NDEBUG
#undef	m_ret
#undef	ret_count
#define	m_ret		m_section.m_name
#define	ret_count	CTX->m_count.m_name
		pos = ret_count;
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpName, 0)))			return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, entp.m_id)))			return BADALLOC;
		unless((ret_count = emit_str(&CTX->m_ret, ret_count, LLVMGetValueName(entp.m_fn))))	return BADALLOC;
		set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[pos], ret_count - pos - 1);
#endif
	}

	return FN_ACLSPV_PASS_OK;
}

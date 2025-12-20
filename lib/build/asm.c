#include <build.h>
#include "./wrdemit.h"

#include <spirv/1.0/spirv.h>


#include "./id.h"
#include "./entp.h"


ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass	aclspv_build_asm(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
#define		ret_count	CTX->m_count.m_entpdef
#define		m_ret		m_section.m_entpdef
#define		num_kernels	CTX->m_fnlist.m_num_entp
#define		kernel_nodes	((lib_build_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)

	aclspv_wrdcount_t	i;

	/* Emit function stubs */
	for (i = num_kernels; i--; ) {
		aclspv_wrd_t func_id = kernel_nodes[i].m_id; 
		aclspv_wrd_t lbl_id = CTX->m_id++;

		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpFunction, 4))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_VOID))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, func_id))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvFunctionControlMaskNone))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_FN_VOID))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;

		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpLabel, 1))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, lbl_id))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;

		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpReturn, 0))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpFunctionEnd, 0))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
	}

#undef	m_ret
#undef	ret_count
#undef	num_kernels
#undef	kernel_nodes

	IGNORE(M);

	CTX->m_retcount
		= CTX->m_count.m_capability
		+ CTX->m_count.m_ext
		+ CTX->m_count.m_memmodel
		+ CTX->m_count.m_entp
		+ CTX->m_count.m_execmode
		+ CTX->m_count.m_name
		+ CTX->m_count.m_decorate
		+ CTX->m_count.m_types
		+ CTX->m_count.m_vars
		+ CTX->m_count.m_entpdef
		+ 5;

	_aclspv_grow_vec(malloc, _free, CTX->m_ret
			, (size_t)count_to_sz(CTX->m_retcount)
			);

	unless(CTX->m_ret.m_p)		return FN_ACLSPV_PASS_ALLOC_FAILED;

	get_wrd_of_vec(&CTX->m_ret)[0] = SpvMagicNumber;
	get_wrd_of_vec(&CTX->m_ret)[1] = SpvVersion;
	get_wrd_of_vec(&CTX->m_ret)[2] = 0;
	get_wrd_of_vec(&CTX->m_ret)[3] = CTX->m_retcount;
	get_wrd_of_vec(&CTX->m_ret)[4] = 0;

#define	retbuf ((aclspv_wrd_t* ae2f_restrict)(CTX->m_ret.m_p))

#define	cpypad	retbuf + 5
#define	cpysz	count_to_sz(CTX->m_count.m_capability)

	assert(CTX->m_count.m_capability);
	assert((0[(aclspv_wrd_t* ae2f_restrict)lib_build_ctx_section_capability(*CTX).m_p] & SpvOpCapability) == SpvOpCapability);
	assert(0[(aclspv_wrd_t* ae2f_restrict)lib_build_ctx_section_capability(*CTX).m_p] == opcode_to_wrd(SpvOpCapability, 1));

	memcpy(cpypad, lib_build_ctx_section_capability(*CTX).m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)
#define	cpysz	count_to_sz(CTX->m_count.m_ext)
	memcpy(cpypad, lib_build_ctx_section_ext(*CTX).m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)
#define	cpysz	count_to_sz(CTX->m_count.m_memmodel)
	memcpy(cpypad, lib_build_ctx_section_memmodel(*CTX).m_p, (size_t)cpysz);


#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)
#define	cpysz	count_to_sz(CTX->m_count.m_entp)
	memcpy(cpypad, CTX->m_section.m_entp.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)
#define	cpysz	count_to_sz(CTX->m_count.m_execmode)
	memcpy(cpypad, (CTX)->m_section.m_execmode.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)		\
	+ (CTX->m_count.m_execmode)
#define	cpysz	count_to_sz(CTX->m_count.m_name)
	memcpy(cpypad, (CTX)->m_section.m_name.m_p, (size_t)cpysz);


#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)		\
	+ (CTX->m_count.m_execmode)		\
	+ (CTX->m_count.m_name)
#define	cpysz	count_to_sz(CTX->m_count.m_decorate)
	memcpy(cpypad, (CTX)->m_section.m_decorate.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)		\
	+ (CTX->m_count.m_execmode)		\
	+ (CTX->m_count.m_name)		\
	+ (CTX->m_count.m_decorate)
#define	cpysz	count_to_sz(CTX->m_count.m_types)
	memcpy(cpypad, (CTX)->m_section.m_types.m_p, (size_t)cpysz);



#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)		\
	+ (CTX->m_count.m_execmode)		\
	+ (CTX->m_count.m_name)		\
	+ (CTX->m_count.m_decorate)		\
	+ (CTX->m_count.m_types)
#define	cpysz	count_to_sz(CTX->m_count.m_vars)
	memcpy(cpypad, (CTX)->m_section.m_vars.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)		\
	+ (CTX->m_count.m_execmode)		\
	+ (CTX->m_count.m_name)		\
	+ (CTX->m_count.m_decorate)		\
	+ (CTX->m_count.m_types)		\
	+ (CTX->m_count.m_vars)
#define	cpysz	count_to_sz(CTX->m_count.m_entpdef)
	memcpy(cpypad, (CTX)->m_section.m_entpdef.m_p, (size_t)cpysz);

#undef	retbuf

	assert(CTX->m_ret.m_p);

	return FN_ACLSPV_PASS_OK;
}

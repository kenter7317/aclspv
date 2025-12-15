#include "./ctx.h"

#include <ae2f/c90/StdInt.h>
#include <ae2f/c90/StdBool.h>

#include <llvm-c/Core.h>
#include <spirv/unified1/spirv.h>
#include <assert.h>
#include <string.h>

#include <spirv-tools/libspirv.h>
#include <aclspv/spvty.h>

/** 
 * @def 	sz_to_count 
 * @brief	byte size to word count
 * */
#define sz_to_count(c_sz)	((size_t)(((size_t)(c_sz)) / ((size_t)sizeof(aclspv_wrd_t))))

/**
 * @def		count_to_sz
 * @brief	word count to byte size
 * */
#define count_to_sz(c_count)	((size_t)(((size_t)(c_count)) * (size_t)(sizeof(aclspv_wrd_t))))

/**
 * @def		get_wrd_of_vec
 * @brief	get word buffer from vector
 * */
#define get_wrd_of_vec(vec)	((aclspv_wrd_t* ae2f_restrict)((vec)->m_p))

#define _free(a, b)	free(a)

/**
 * @fn		emit_wrd
 * @brief	try emitting `c_wrd` on `h_vec`
 * @return	new c_wrdcount available. 0 when failed allocating.
 * */
static ae2f_inline size_t emit_wrd(x_aclspv_vec* ae2f_restrict const h_wrds, const size_t c_wrdcount, const aclspv_wrd_t c_wrd)
{
	if(count_to_sz(c_wrdcount + 1) > h_wrds->m_sz)
		_aclspv_grow_vec_with_copy(
				malloc, _free, _aclspv_memcpy 
				, L_new, *h_wrds
				, count_to_sz((c_wrdcount + 1) << 1)
				);

	unless(h_wrds->m_p) return 0;
	if(count_to_sz(c_wrdcount) >= h_wrds->m_sz)
		return 0;

	get_wrd_of_vec(h_wrds)[c_wrdcount] = c_wrd;
	return c_wrdcount + 1;
}


#define	mk_noprnds(c_num_opprm)	\
	((((aclspv_wrd_t)((c_num_opprm) + 1)) << 16) & (aclspv_wrd_t)ACLSPV_MASK_NOPRNDS)

#define	opcode_to_wrd(c_opcode, c_num_opprm)	\
	((aclspv_wrd_t)(((aclspv_wrd_t)(c_opcode) & ACLSPV_MASK_OPCODE) | mk_noprnds(c_num_opprm)))
/**
 * @def		emit_opcode
 * @brief	try emit opcode with num_opprm
 * */
#define	emit_opcode(h_wrds, c_wrdcount, c_opcode, c_num_opprm_opt)	\
	(emit_wrd(h_wrds, c_wrdcount, opcode_to_wrd(c_opcode, c_num_opprm_opt)))

#define set_oprnd_count_for_opcode(cr_wrd, c_num_opprm)	\
	(cr_wrd) = opcode_to_wrd((cr_wrd & ACLSPV_MASK_OPCODE), ((aclspv_num_opprm_t)(c_num_opprm)))

/**
 * @fn		emit_str
 * @brief	emit string with word size padded.
 * */
ae2f_inline static size_t emit_str(
		x_aclspv_vec* ae2f_restrict const h_wrds, 
		const size_t c_wrdcount, 
		const char* ae2f_restrict const rd_str
		)
{
	/** string len: null included. */
	const size_t	len = strlen(rd_str) + 1;
	const size_t	pad_wrds = (!!((len) & 3)) + ((len) >> 2);

	_aclspv_grow_vec_with_copy(malloc, _free, _aclspv_memcpy, L_new
			, *h_wrds, count_to_sz(pad_wrds + c_wrdcount));

	unless(h_wrds->m_p) return 0;
	assert(count_to_sz(pad_wrds + c_wrdcount) <= h_wrds->m_sz);

	memset(&get_wrd_of_vec(h_wrds)[c_wrdcount], 0, count_to_sz(pad_wrds));
	_aclspv_memcpy(&get_wrd_of_vec(h_wrds)[c_wrdcount], rd_str, len);

	return c_wrdcount + pad_wrds;
}

IMPL_PASS_RET aclspv_pass_mkspv(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
	size_t	ret_count = 5;
	size_t	pos_tmp0;

	e_fn_aclspv_pass	status = FN_ACLSPV_PASS_OK;

	assert(M);
	assert(CTX);

	/** header */
	_aclspv_grow_vec(malloc, _free, CTX->m_ret, count_to_sz(5));
	unless(CTX->m_ret.m_p) return FN_ACLSPV_PASS_ALLOC_FAILED;
	unless((CTX->m_ret.m_sz) == count_to_sz(5)) {
		status = FN_ACLSPV_PASS_MET_INVAL;
		goto FNSTATISBAD;
	}

	get_wrd_of_vec(&CTX->m_ret)[0] = SpvMagicNumber;
	get_wrd_of_vec(&CTX->m_ret)[1] = SpvVersion;
	get_wrd_of_vec(&CTX->m_ret)[2] = 0;
	get_wrd_of_vec(&CTX->m_ret)[3] = 0;	/** bound */
	get_wrd_of_vec(&CTX->m_ret)[4] = 0;


	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpCapability, 1))) {
		status = FN_ACLSPV_PASS_ALLOC_FAILED;
		goto FNSTATISBAD;
	}

	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvCapabilityShader))) {
		status = FN_ACLSPV_PASS_ALLOC_FAILED;
		goto FNSTATISBAD;
	}

	pos_tmp0 = ret_count;
	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpExtension, 0))) {
		status = FN_ACLSPV_PASS_ALLOC_FAILED;
		goto FNSTATISBAD;
	}

	unless((ret_count = emit_str(&CTX->m_ret, ret_count, "SPV_KHR_storage_buffer_storage_class"))) {
		status = FN_ACLSPV_PASS_ALLOC_FAILED;
		goto FNSTATISBAD;
	}

	set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[pos_tmp0], (aclspv_wrd_t)(ret_count - pos_tmp0 - 1));

	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpMemoryModel, 2))) {
		status = FN_ACLSPV_PASS_ALLOC_FAILED;
		goto FNSTATISBAD;
	}

	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvAddressingModelLogical))) {
		status = FN_ACLSPV_PASS_ALLOC_FAILED;
		goto FNSTATISBAD;
	}

	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvMemoryModelGLSL450))) {
		status = FN_ACLSPV_PASS_ALLOC_FAILED;
		goto FNSTATISBAD;
	}


	goto FNSTATISGOOD;

FNSTATISBAD:
	_aclspv_stop_vec(_free, CTX->m_ret);
	_aclspv_init_vec(CTX->m_ret);
	return status;

FNSTATISGOOD:
	if(CTX->m_ret.m_sz < count_to_sz(ret_count) || (ret_count) > (size_t)UINT32_MAX || !CTX->m_ret.m_p) {
		status = FN_ACLSPV_PASS_TOO_BIG;
		goto FNSTATISBAD;
	} else {
		/** bound */
		get_wrd_of_vec(&CTX->m_ret)[3] = ((aclspv_wrd_t)ret_count);
		CTX->m_ret.m_sz = count_to_sz((ret_count));
	}

	return FN_ACLSPV_PASS_OK;
}

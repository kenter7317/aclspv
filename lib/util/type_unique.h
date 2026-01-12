/** @file type_unique.h */

#ifndef util_type_unique_h
#define util_type_unique_h

#include <ae2f/Keys.h>
#include <clang-c/Index.h>
#include <aclspv/spvty.h>
#include "./vec.auto.h"
#include <assert.h>

/**
 * @class	util_type_unique
 * @brief	unique types when `ID_DEFAULT_` and `constant` is not enough
 * */
typedef struct {
	aclspv_id_t	m_id;
	CXType		m_type;
} util_type_unique;

/**
 * @returns	the index for `c_type`
 * @returns	`c_num_uniques` when not found
 * @see		util_type_unique
 * */
ae2f_inline static aclspv_wrd_t	find_type_unique(
		const aclspv_wrd_t c_num_uniques, 
		const util_type_unique* ae2f_restrict rd_uniques,
		const CXType		c_type
		)
{
	aclspv_wrd_t	IDX = c_num_uniques;
	const CXType	CANONICAL = clang_getCanonicalType(c_type);
	if(rd_uniques) while(IDX--) {
		if(clang_equalTypes(CANONICAL, rd_uniques[IDX].m_type))
			break;
	}

	if(IDX < c_num_uniques)
		return IDX;

	return c_num_uniques;
}

/**
 * @returns	the index for `c_type`
 * @returns	`c_num_uniques` when not found so made new room and it succeed.
 * @returns	`c_num_uniques + 1` when error occurred.
 * @see		util_type_unique
 * */
ae2f_inline static aclspv_wrd_t	mk_type_unique(
		const aclspv_wrd_t			c_num_uniques,
		x_aclspv_vec* ae2f_restrict const	h_uniques,
		const CXType				c_type
		)
{
	aclspv_wrd_t	IDX		= 0;

	ae2f_expected_but_else(h_uniques) {
		assert(0 && "h_uniques is null");
		return	c_num_uniques + 1;
	}

	ae2f_unexpected_but_if(h_uniques->m_sz < (size_t)(sizeof(util_type_unique) * c_num_uniques)) {
		assert(0 && "vector is too small - invalid c_num_uniques");
		return	c_num_uniques + 1;
	}

	if((IDX = find_type_unique(c_num_uniques, h_uniques->m_p, c_type)) != c_num_uniques)
		return IDX;

	_aclspv_grow_vec_with_copy(_aclspv_malloc, _aclspv_free, _aclspv_memcpy, L_new, *h_uniques
			, (size_t)(c_num_uniques + 1) * (size_t)sizeof(c_num_uniques));

	ae2f_expected_but_else(h_uniques->m_p) {
		return	c_num_uniques + 1;
	}

	c_num_uniques[(util_type_unique* ae2f_restrict)h_uniques->m_p].m_type	= c_type;
	c_num_uniques[(util_type_unique* ae2f_restrict)h_uniques->m_p].m_id	= 0;
	return c_num_uniques;
}

#endif 

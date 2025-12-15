/**
 * @file	scale.h
 * @brief	Kind of single linked list where only last one could be resized.
 * */

#ifndef pass_scale_h
#define pass_scale_h

#include "./ctx.h"


typedef struct x_scale x_scale;
typedef x_scale* ae2f_restrict h_scale_t;

struct x_scale {
	size_t			m_nxt;
	size_t			m_buf;
	size_t			m_sz;
};

#define	SCALE_HEADER_SIZE	\
	((size_t)(sizeof(x_scale) + sizeof(size_t)))

#define	get_scale_header_from_vec(h_alloc)	((size_t* ae2f_restrict)((h_alloc)->m_p))
#define get_first_scale_from_vec(h_alloc)	((h_scale_t)(get_scale_header_from_vec(h_alloc) + 1))

#define	get_last_scale_from_vec(h_alloc)	\
	((h_scale_t)((*get_scale_header_from_vec(h_alloc)) + ((char* ae2f_restrict)((h_alloc)->m_p))))

#define	get_buf_from_scale(h_alloc, c_scale)	\
	(void* ae2f_restrict)(((char* ae2f_restrict)(h_alloc)->m_p) + ((c_scale).m_buf))

#define	get_nxt_from_scale(h_alloc, c_scale)	\
	(void* ae2f_restrict)(((char* ae2f_restrict)(h_alloc)->m_p) + ((c_scale).m_nxt))

ae2f_inline ae2f_retnew static x_scale* init_scale(
		x_aclspv_vec* ae2f_restrict const	h_alloc,
		const size_t				c_init_sz
		)
{
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, *h_alloc, c_init_sz + SCALE_HEADER_SIZE);
	*get_scale_header_from_vec(h_alloc) = sizeof(size_t);

	unless(h_alloc->m_p)
		return ae2f_NIL;

	get_first_scale_from_vec(h_alloc)->m_buf = SCALE_HEADER_SIZE;
	get_first_scale_from_vec(h_alloc)->m_sz = c_init_sz;
	get_first_scale_from_vec(h_alloc)->m_nxt = 0;

	return get_first_scale_from_vec(h_alloc);
}

ae2f_inline ae2f_ccconst static x_scale* get_scale_from_vec(
		x_aclspv_vec* ae2f_restrict const	h_alloc,
		const size_t				c_idx
		)
{
	size_t i = c_idx;
	x_scale* ae2f_restrict ret = get_first_scale_from_vec(h_alloc);

	unless(ret && h_alloc->m_sz < SCALE_HEADER_SIZE)
		return ae2f_NIL;

	while(i--) {
		unless(ret->m_nxt)
			return ae2f_NIL;

		ret = get_nxt_from_scale(h_alloc, *ret);
	}

	return ret;
}

ae2f_inline	static int_fast8_t grow_last_scale(
		x_aclspv_vec* ae2f_restrict const	h_alloc,
		const size_t				c_newsize
		)
{
	if(get_last_scale_from_vec(h_alloc)->m_nxt)
		return 1;

	_aclspv_grow_vec_with_copy(
			_aclspv_malloc, _aclspv_free, _aclspv_memcpy
			, L_new
			, *h_alloc
			, c_newsize + get_last_scale_from_vec(h_alloc)->m_buf
			);

	return !h_alloc->m_p;
}

ae2f_retnew ae2f_inline static x_scale* mk_scale_from_vec(
		x_aclspv_vec* ae2f_restrict const	h_alloc,
		const size_t				c_init_size_opt
		)
{
	h_scale_t lst = get_last_scale_from_vec(h_alloc), nxt;
	_aclspv_grow_vec_with_copy(
			_aclspv_malloc, _aclspv_free, _aclspv_memcpy, L_new
			, *h_alloc
			, lst->m_buf + lst->m_sz + c_init_size_opt + (size_t)sizeof(x_scale)
			);

	lst->m_nxt = lst->m_buf + lst->m_sz;

	*get_scale_header_from_vec(h_alloc)
		= lst->m_nxt;

	nxt = get_nxt_from_scale(h_alloc, *lst);

	nxt->m_buf = lst->m_nxt + (size_t)sizeof(x_scale);
	nxt->m_sz = c_init_size_opt;
	nxt->m_nxt = 0;

	return nxt;
}

#endif

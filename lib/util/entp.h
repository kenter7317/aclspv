#ifndef __lib_build_entp
#define __lib_build_entp

#include <aclspv/spvty.h>
#include <clang-c/Index.h>

typedef struct {
	aclspv_id_t	m_id;

	struct {
		aclspv_id_t m_push_struct;
		aclspv_id_t m_push_ptr;
		aclspv_id_t m_push_var;
		aclspv_id_t m_arrty;
		aclspv_id_t m_arrsz;
	} m_push_ids;

	CXCursor	m_fn;
} lib_build_entp_t;

#endif

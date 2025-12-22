#ifndef __lib_build_bind_h
#define __lib_build_bind_h

#include <aclspv/spvty.h>
#include <llvm-c/Core.h>

typedef struct {
	aclspv_id_t		m_var_id;
	aclspv_wrd_t		m_set;
	aclspv_wrdcount_t	m_binding;
	aclspv_wrdcount_t	m_arg_idx;
	aclspv_wrdcount_t	m_entp_idx;
	aclspv_id_t		m_struct_id;
	aclspv_id_t		m_ptr_struct_id;
	aclspv_wrd_t		m_storage_class;  /* SpvStorageClass* */
} lib_build_bind;

#endif

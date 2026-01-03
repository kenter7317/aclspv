#ifndef util_bind_h
#define util_bind_h

#include <aclspv/spvty.h>
#include <llvm-c/Core.h>

typedef union {
	struct {
		aclspv_wrd_t		m_storage_class;
		aclspv_id_t		m_var_id;

		aclspv_wrdcount_t	m_arg_idx;

		aclspv_wrdcount_t	m_entp_idx;
		aclspv_wrd_t		m_set;

		aclspv_wrdcount_t	m_binding;
		aclspv_id_t		m_struct_id;
		aclspv_id_t		m_ptr_struct_id;
	}	m_bindable;

	struct {
		aclspv_wrd_t		m_storage_class;
		aclspv_id_t		m_var_id;
		aclspv_id_t		m_arg_idx;
		aclspv_wrdcount_t	m_entp_idx;

		aclspv_id_t		m_arr_count_id;
		aclspv_id_t		m_arr_type_id;
	}	m_work;
} util_bind;

#endif

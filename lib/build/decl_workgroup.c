#include <build.h>
#include <assert.h>

#include <pass/md.h>
#include <pass/argknd.h>

#include <spirv/1.0/spirv.h>

#include "./wrdemit.h"
#include "./entp.h"
#include "./bind.h"
#include "./scale.h"
#include "./id.h"

#define var_infos	((lib_build_bind* ae2f_restrict)CTX->m_vecid_vars.m_p)
#define interface_ids_mgr	get_last_scale_from_vec(&CTX->m_scale_vars)
#define	interface_ids		((aclspv_wrd_t* ae2f_restrict)get_buf_from_scale(&CTX->m_scale_vars, *interface_ids_mgr))

#define	__vec			((lib_build_entp_t * ae2f_restrict)CTX->m_fnlist.m_entp.m_p)

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass	aclspv_build_decl_workgroup(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	const LLVMContextRef C = LLVMGetModuleContext(M);
	const unsigned arg_kind_md_id = LLVMGetMDKindIDInContext(C, ACLSPV_MD_ARGKND,
			sizeof(ACLSPV_MD_ARGKND)-1);
	aclspv_wrdcount_t i_fn = CTX->m_fnlist.m_num_entp;
	aclspv_wrdcount_t num_workgroup_vars = 0;

	/* Count workgroup variables first */
	for(i_fn = 0; i_fn < CTX->m_fnlist.m_num_entp; ++i_fn) {
		const LLVMValueRef kernel = ((lib_build_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[i_fn].m_fn;
		const LLVMValueRef arg_kinds_node = LLVMGetMetadata(kernel, arg_kind_md_id);
		
		if (!arg_kinds_node) continue;
		
		{
			const unsigned num_args = LLVMGetMDNodeNumOperands(arg_kinds_node);
			unsigned i_arg;
			
			for(i_arg = 0; i_arg < num_args; ++i_arg) {
				LLVMValueRef arg_kind_md = LLVMGetOperand(arg_kinds_node, i_arg);
				if (arg_kind_md) {
					unsigned len;
					const char* kind_str = LLVMGetMDString(arg_kind_md, &len);
					if (kind_str && strcmp(kind_str, ACLSPV_ARGKND_LOC) == 0) {
						++num_workgroup_vars;
					}
				}
			}
		}
	}

	if (num_workgroup_vars == 0) {
		return FN_ACLSPV_PASS_OK;
	}

	/* Grow vector for workgroup variables */
	if (CTX->m_vecid_vars.m_sz <= ((size_t)sizeof(lib_build_bind) * (size_t)(CTX->m_vecid_vars.m_sz / sizeof(lib_build_bind) + num_workgroup_vars))) {
		_aclspv_grow_vec_with_copy(
				_aclspv_malloc, _aclspv_free, _aclspv_memcpy, L_new
				, CTX->m_vecid_vars
				, CTX->m_vecid_vars.m_sz 
				? CTX->m_vecid_vars.m_sz << 1 
				: (size_t)(sizeof(lib_build_bind) << 3)
				);
		unless(var_infos)
			return FN_ACLSPV_PASS_ALLOC_FAILED;
	}

	/* Emit workgroup variable declarations */
	for(i_fn = 0; i_fn < CTX->m_fnlist.m_num_entp; ++i_fn) {
		const LLVMValueRef kernel = ((lib_build_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[i_fn].m_fn;
		const LLVMValueRef arg_kinds_node = LLVMGetMetadata(kernel, arg_kind_md_id);
		
		if (!arg_kinds_node) continue;
		
		{
			const unsigned num_args = LLVMGetMDNodeNumOperands(arg_kinds_node);
			unsigned i_arg;
			
			for(i_arg = 0; i_arg < num_args; ++i_arg) {
				LLVMValueRef arg_kind_md = LLVMGetOperand(arg_kinds_node, i_arg);
				if (arg_kind_md) {
					unsigned len;
					const char* kind_str = LLVMGetMDString(arg_kind_md, &len);
					if (kind_str && strcmp(kind_str, ACLSPV_ARGKND_LOC) == 0) {
						lib_build_bind* info = &var_infos[CTX->m_vecid_vars.m_sz / sizeof(lib_build_bind)];
						aclspv_wrd_t ptr_type_id;
						
						/* Generate type for workgroup variable */
						ptr_type_id = CTX->m_id++;
						info->m_var_id = CTX->m_id++;
						info->m_struct_id = ID_DEFAULT_INT32;  /* Use existing int type */
						info->m_ptr_struct_id = ptr_type_id;
						info->m_arg_idx = i_arg;
						info->m_entp_idx = i_fn;
						info->m_storage_class = SpvStorageClassWorkgroup;
						
						/* Use existing int32 type to avoid duplicates */
						
#define		ret_count	CTX->m_count.m_types
#define		m_ret		m_section.m_types

						/* OpTypePointer Workgroup */
						unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypePointer, 3))) 
							return FN_ACLSPV_PASS_ALLOC_FAILED;
						unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ptr_type_id))) 
							return FN_ACLSPV_PASS_ALLOC_FAILED;
						unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvStorageClassWorkgroup))) 
							return FN_ACLSPV_PASS_ALLOC_FAILED;
						unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_INT32))) 
							return FN_ACLSPV_PASS_ALLOC_FAILED;

#undef		ret_count
#undef		m_ret

#define		ret_count	CTX->m_count.m_vars
#define		m_ret		m_section.m_vars

						/* OpVariable */
						unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpVariable, 3))) 
							return FN_ACLSPV_PASS_ALLOC_FAILED;
						unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ptr_type_id))) 
							return FN_ACLSPV_PASS_ALLOC_FAILED;
						unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->m_var_id))) 
							return FN_ACLSPV_PASS_ALLOC_FAILED;
						unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvStorageClassWorkgroup)))
							return FN_ACLSPV_PASS_ALLOC_FAILED;

#undef		ret_count
#undef		m_ret

						/* Add to interface */
						{
							aclspv_wrdcount_t interface_count = (aclspv_wrdcount_t)(get_last_scale_from_vec(&CTX->m_scale_vars)->m_sz / sizeof(aclspv_wrd_t));
							if(grow_last_scale(&CTX->m_scale_vars, (size_t)count_to_sz(interface_count + 1))) {
								return FN_ACLSPV_PASS_ALLOC_FAILED;
							}
							interface_ids[interface_count] = info->m_var_id;
						}
						
						CTX->m_vecid_vars.m_sz += sizeof(lib_build_bind);
					}
				}
			}
		}
	}

	return FN_ACLSPV_PASS_OK;
}

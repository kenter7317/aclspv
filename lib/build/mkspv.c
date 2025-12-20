#if 0

#include "pass/md.h"
#include "pass/argknd.h"


#include <aclspv/build.h>

#include <ae2f/c90/StdInt.h>
#include <ae2f/c90/StdBool.h>

#include <llvm-c/Core.h>
#include <spirv/1.0/spirv.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <spirv-tools/libspirv.h>
#include <aclspv/spvty.h>

#include "./wrdemit.h"
#include "./scale.h"
#include "./id.h"

typedef struct {
	aclspv_wrd_t	var_id;
	aclspv_wrd_t	set;
	aclspv_wrd_t	binding;
	aclspv_wrd_t	arg_idx;
	LLVMValueRef	kernel_func;
	aclspv_wrd_t	struct_id;
	aclspv_wrd_t	ptr_struct_id;
} s_var_info;



typedef struct {
	aclspv_wrd_t	m_id;
	LLVMValueRef	m_fn;
} func_with_id;

IMPL_PASS_RET aclspv_build_mkspv(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
	spvsz_t	ret_count = 5;
	spvsz_t	pos_tmp0;
	unsigned	num_kernels;

	const LLVMContextRef C = LLVMGetModuleContext(M);
	const unsigned layout_md_id = C ? LLVMGetMDKindIDInContext(C, ACLSPV_MD_PIPELINE_LAYOUT,
			sizeof(ACLSPV_MD_PIPELINE_LAYOUT)-1) : 0;

	/** 
	 * @var		tynxt_id 
	 * @brief	next id
	 * @details	use `next_id++` to get new id
	 * */
	aclspv_wrd_t next_id = ID_DEFAULT_END;

	e_fn_aclspv_pass	status = FN_ACLSPV_PASS_ALLOC_FAILED;

	assert(M);
	assert(CTX);


	/** header */
	_aclspv_grow_vec(malloc, _free, CTX->m_ret, count_to_sz(5));
	unless(CTX->m_ret.m_p)
		goto LBL_FNSTATISBAD;
	unless((CTX->m_ret.m_sz) == count_to_sz(5)) {
		status = FN_ACLSPV_PASS_MET_INVAL;
		goto LBL_FNSTATISBAD;
	}
	unless(C) goto LBL_FNSTATISBAD;
	unless(layout_md_id) {
		status = FN_ACLSPV_PASS_GET_FAILED;
		goto LBL_FNSTATISBAD;
	}

	get_wrd_of_vec(&CTX->m_ret)[0] = SpvMagicNumber;
	get_wrd_of_vec(&CTX->m_ret)[1] = SpvVersion;
	get_wrd_of_vec(&CTX->m_ret)[2] = 0;
	get_wrd_of_vec(&CTX->m_ret)[3] = 0;	/** bound */
	get_wrd_of_vec(&CTX->m_ret)[4] = 0;

	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpCapability, 1))) {
		goto LBL_FNSTATISBAD;
	}

	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvCapabilityShader))) {
		goto LBL_FNSTATISBAD;
	}

	pos_tmp0 = ret_count;
	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpExtension, 0))) {
		goto LBL_FNSTATISBAD;
	}

	unless((ret_count = emit_str(&CTX->m_ret, ret_count, "SPV_KHR_storage_buffer_storage_class"))) {
		goto LBL_FNSTATISBAD;
	}

	set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[pos_tmp0], (aclspv_wrd_t)(ret_count - pos_tmp0 - 1));

	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpMemoryModel, 2))) {
		goto LBL_FNSTATISBAD;
	}

	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvAddressingModelLogical))) {
		goto LBL_FNSTATISBAD;
	}

	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvMemoryModelGLSL450))) {
		goto LBL_FNSTATISBAD;
	}

	{
		unsigned i = 0;
		size_t	num_vars = 0;

		{
			LLVMValueRef	func;
			for (func = LLVMGetFirstFunction(M); func; func = LLVMGetNextFunction(func)) {
				if (LLVMGetMetadata(func, LLVMGetMDKindID(
								ACLSPV_MD_PIPELINE_LAYOUT
								, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1
								))) {  /* Has pipelayout? -> kernel */
					i++;
				}
			}
		}

		unless (i) {
			goto LBL_NO_KRNL;
		}


		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v0, (size_t)(sizeof(LLVMValueRef) * i));

#define kernel_nodes	((func_with_id* ae2f_restrict)CTX->m_v0.m_p)
#define interface_ids_mgr	get_last_scale_from_vec(&CTX->m_v1)
#define	interface_ids		((aclspv_wrd_t* ae2f_restrict)get_buf_from_scale(&CTX->m_v1, *interface_ids_mgr))

		unless (kernel_nodes && init_scale(&CTX->m_v1, 0)) {
			goto LBL_FNSTATISBAD;
		}

		assert(get_first_scale_from_vec(&CTX->m_v1) == get_last_scale_from_vec(&CTX->m_v1));

		{
			LLVMValueRef func;
			i = 0;
			for (func = LLVMGetFirstFunction(M); func; func = LLVMGetNextFunction(func)) {
				if (LLVMGetMetadata(func, LLVMGetMDKindID(
								ACLSPV_MD_PIPELINE_LAYOUT
								, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1))

						&& LLVMGetFunctionCallConv(func) == LLVMSPIRKERNELCallConv
				   )
				{
					kernel_nodes[i++].m_fn = func;
				}
			}

			num_kernels = i;
			unless(num_kernels) goto LBL_NO_KRNL;
#define	num_kernels	((const unsigned)num_kernels)
		}

#define var_infos	((s_var_info* ae2f_restrict)CTX->m_v2.m_p)
		_aclspv_grow_vec(malloc, _free, CTX->m_v2, (size_t)(sizeof(s_var_info) * num_kernels * 4)); /* Initial alloc */
		unless(var_infos) goto LBL_FNSTATISBAD;

		for(i = num_kernels; i-- ;) {
			const LLVMValueRef kernel_func = kernel_nodes[i].m_fn;
			const LLVMValueRef layout_md = LLVMGetMetadata(kernel_func, layout_md_id);
			size_t	interface_count = 0;
			int num_sets;
			int i_set;
			LLVMValueRef set_layouts_node;

			if (!layout_md || !LLVMIsAMDNode(layout_md) || LLVMGetNumOperands(layout_md) < 2) {
				goto LBL_NEXT_KRNL;
			}

			set_layouts_node = LLVMGetOperand(layout_md, 1);
			if (!set_layouts_node || !LLVMIsAMDNode(set_layouts_node)) {
				goto LBL_NEXT_KRNL;
			}

			num_sets = LLVMGetNumOperands(set_layouts_node);
			/* Iterate over descriptor sets */
			for (i_set = 0; i_set < num_sets; ++i_set) {
				const LLVMValueRef set_layout_node = LLVMGetOperand(set_layouts_node, (unsigned)i_set);
				LLVMValueRef	set_num_val, bindings_node;
				aclspv_wrd_t	set;
				int	i_binding;

				if (!set_layout_node || !LLVMIsAMDNode(set_layout_node) || LLVMGetNumOperands(set_layout_node) < 2) {
					continue;
				}

				set_num_val = LLVMGetOperand(set_layout_node, 0);
				set = (aclspv_wrd_t)LLVMConstIntGetZExtValue(set_num_val);

				bindings_node = LLVMGetOperand(set_layout_node, 1);
				if (!bindings_node || !LLVMIsAMDNode(bindings_node)) {
					continue;
				}

				/* Iterate over bindings */
				for (i_binding = LLVMGetNumOperands(bindings_node); i_binding-- > 0; ) {
					const LLVMValueRef binding_node = LLVMGetOperand(bindings_node, (unsigned)i_binding);
					LLVMValueRef	arg_idx_val, binding_val;
					s_var_info* ae2f_restrict	info;

					if (!binding_node || !LLVMIsAMDNode(binding_node) || LLVMGetNumOperands(binding_node) < 5) {
						continue;
					}

					if (CTX->m_v2.m_sz <= ((size_t)sizeof(s_var_info) * num_vars)) {
						_aclspv_grow_vec_with_copy(malloc, _free, _aclspv_memcpy, L_new, CTX->m_v2, CTX->m_v2.m_sz << 1);
						unless(var_infos) goto LBL_FNSTATISBAD;
					}
					info = &var_infos[num_vars];

					arg_idx_val = LLVMGetOperand(binding_node, 0);
					binding_val = LLVMGetOperand(binding_node, 1);

					info->var_id = next_id++;
					info->struct_id = next_id++;
					info->ptr_struct_id = next_id++;
					info->arg_idx = (aclspv_wrd_t)LLVMConstIntGetZExtValue(arg_idx_val);
					info->binding = (aclspv_wrd_t)LLVMConstIntGetZExtValue(binding_val);

					assert(info->arg_idx == info->binding);

					info->set = set;
					info->kernel_func = kernel_func;

					/** store id */
					if(grow_last_scale(&CTX->m_v1, count_to_sz(interface_count + 1))) {
						goto LBL_FNSTATISBAD;
					}
					interface_ids[interface_count++] = info->var_id;
					num_vars++;
				}
			}

LBL_NEXT_KRNL:
			assert(get_last_scale_from_vec(&CTX->m_v1)->m_sz >= count_to_sz(interface_count));
			get_last_scale_from_vec(&CTX->m_v1)->m_sz = count_to_sz(interface_count);

			unless(mk_scale_from_vec(&CTX->m_v1, 0)) {
				goto LBL_FNSTATISBAD;
			}
		}

		for(i = num_kernels; i--;) {
			LLVMValueRef	kernel_func = kernel_nodes[i].m_fn;
			const char* ae2f_restrict	name = LLVMGetValueName(kernel_func);
			const aclspv_wrd_t	func_id	= next_id++;
			h_scale_t	scale = get_scale_from_vec(&CTX->m_v1, (size_t)i);
			spvsz_t entry_pos = ret_count;

			kernel_nodes[i].m_id = func_id;

			unless(scale)	{
				status = FN_ACLSPV_PASS_GET_FAILED;
				goto LBL_FNSTATISBAD;
			}

			/** OpEntryPoint */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpEntryPoint, 0))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvExecutionModelGLCompute))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, func_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_str(&CTX->m_ret, ret_count, name))) goto LBL_FNSTATISBAD;	

			set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[entry_pos], ret_count - entry_pos - 1);
		}

		/** OpExecutionModel */
		for(i = num_kernels; i--; ) {
			/** TODO: implement attribute __attribute__((execmodel(x, y, z))) */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpExecutionMode, 5))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, kernel_nodes[i].m_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvExecutionModeLocalSize))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1))) goto LBL_FNSTATISBAD;
		}

		/* OpName for function */
#if !defined(NDEBUG) || !NDEBUG
		for(i = num_kernels; i--; ) {
			const size_t name_pos = ret_count;
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpName, 0))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, kernel_nodes[i].m_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_str(&CTX->m_ret, ret_count, LLVMGetValueName(kernel_nodes[i].m_fn)))) goto LBL_FNSTATISBAD;
			set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[name_pos], ret_count - name_pos - 1);
		}
#endif

		/** OpName for Decorators */
#if		!defined(NDEBUG) || !NDEBUG
		for(i = (unsigned)num_vars; i--; ) {
			char	var_name[64];

			size_t	name_pos;
			snprintf(var_name, sizeof(var_name), "%s.arg%u", LLVMGetValueName(var_infos[i].kernel_func), (uint32_t)var_infos[i].arg_idx);
			name_pos = ret_count;
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpName, 0))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, var_infos[i].var_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_str(&CTX->m_ret, ret_count, var_name))) goto LBL_FNSTATISBAD;
			set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[name_pos], ret_count - name_pos - 1);
		}
#endif

		/* Emit decorations and names */
		for (i = (unsigned)num_vars; i--; ) {
			const s_var_info* ae2f_restrict const info = &var_infos[i];
			/* OpDecorate %struct Block */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpDecorate, 2))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->struct_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationBlock))) goto LBL_FNSTATISBAD;

			/* OpMemberDecorate */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpMemberDecorate, 4))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->struct_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 0))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationOffset))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 0))) goto LBL_FNSTATISBAD;

			/** Decorations::Descriptorsets */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpDecorate, 3))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->var_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationDescriptorSet))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->set))) goto LBL_FNSTATISBAD;

			/** Decorations::Bind */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpDecorate, 3))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->var_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationBinding))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->binding))) goto LBL_FNSTATISBAD;
		}

		/* Emit types for variables */

		/** OpTypeVoid */
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypeVoid, 1))) 
			goto LBL_FNSTATISBAD;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_VOID))) 
			goto LBL_FNSTATISBAD;


		/** OpTypeInt */
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypeInt, 3))) 
			goto LBL_FNSTATISBAD;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_INT32))) 
			goto LBL_FNSTATISBAD;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 32))) 
			goto LBL_FNSTATISBAD;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1))) 
			goto LBL_FNSTATISBAD;  /* signed */

		/** OpTypeFunction %void () */
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypeFunction, 2)))
			goto LBL_FNSTATISBAD;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_FN_VOID)))
			goto LBL_FNSTATISBAD;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_VOID)))
			goto LBL_FNSTATISBAD;

		for (i = 0; i < (unsigned)num_vars; i++) {
			const s_var_info* ae2f_restrict const info = &var_infos[i];

			/* OpTypeStruct */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypeStruct, 2))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->struct_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_INT32))) goto LBL_FNSTATISBAD;

			/* OpTypePointer */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypePointer, 3))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->ptr_struct_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvStorageClassStorageBuffer))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->struct_id))) goto LBL_FNSTATISBAD;
		}

		/* Emit variables */
		for (i = 0; i < (unsigned)num_vars; i++) {
			const s_var_info* ae2f_restrict const info = &var_infos[i];
			/* OpVariable */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpVariable, 3))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->ptr_struct_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->var_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvStorageClassStorageBuffer))) 
				goto LBL_FNSTATISBAD;
		}

		/* Emit function stubs */
		for (i = num_kernels; i--; ) {
			aclspv_wrd_t func_id = kernel_nodes[i].m_id; 
			aclspv_wrd_t lbl_id = next_id++;

			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpFunction, 4))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_VOID))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, func_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvFunctionControlMaskNone))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_FN_VOID))) goto LBL_FNSTATISBAD;

			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpLabel, 1))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, lbl_id))) goto LBL_FNSTATISBAD;

			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpReturn, 0))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpFunctionEnd, 0))) goto LBL_FNSTATISBAD;
		}
LBL_NO_KRNL:

		goto LBL_FNSTATISGOOD;

LBL_FNSTATISBAD:
		_aclspv_stop_vec(_free, CTX->m_ret);
		_aclspv_init_vec(CTX->m_ret);
		return status;

LBL_FNSTATISGOOD:
		assert(CTX->m_ret.m_p);
		if(CTX->m_ret.m_sz < count_to_sz(ret_count) || !CTX->m_ret.m_p) {
			status = FN_ACLSPV_PASS_TOO_BIG;
			goto LBL_FNSTATISBAD;
		} else {
			/** bound */
			get_wrd_of_vec(&CTX->m_ret)[3] = ((aclspv_wrd_t)ret_count);
			CTX->m_ret.m_sz = count_to_sz((ret_count));
		}

		return FN_ACLSPV_PASS_OK;
	}
}

#endif

extern int stub(void);

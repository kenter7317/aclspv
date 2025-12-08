#include "./ctx.h"

#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/Types.h>
#include <llvm-c/IRReader.h>

#include <string.h>
#include <assert.h>

#include <aclspv/argknd.h>
#include <aclspv/md.h>

/** From vulkan_core.h */
#define VK_SHADER_STAGE_COMPUTE_BIT 0x00000020
#define VK_DESCRIPTOR_TYPE_SAMPLER 0
#define VK_DESCRIPTOR_TYPE_STORAGE_IMAGE 3
#define VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER 6
#define VK_DESCRIPTOR_TYPE_STORAGE_BUFFER 7

#define ACLSPV_MD_PIPELINE_LAYOUT "aclspv.pipelayout"


/** custom descriptor -> vulkan descriptor */
static unsigned get_descriptor_type(const char* kind) {
	if (strcmp(kind, ACLSPV_ARGKND_SMPLR) == 0)
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	if (strcmp(kind, ACLSPV_ARGKND_RO_IMG) == 0 || strcmp(kind, ACLSPV_ARGKND_WO_IMG) == 0)
		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	if (strcmp(kind, ACLSPV_ARGKND_POD_UBO) == 0 || strcmp(kind, ACLSPV_ARGKND_BUFF_UBO) == 0)
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	if (strcmp(kind, ACLSPV_ARGKND_BUFF) == 0)
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	return (unsigned)-1; /** Invalid */
}

IMPL_PASS_RET aclspv_pass_assgn_pipelayout(
		const LLVMModuleRef	M,
		h_aclspv_pass_ctx	CTX
		) {
	const LLVMContextRef C = LLVMGetModuleContext(M);
	const LLVMTargetDataRef TD = LLVMGetModuleDataLayout(M);

	const unsigned arg_kind_md_id = LLVMGetMDKindIDInContext(
			C, ACLSPV_MD_ARGKND
			, sizeof(ACLSPV_MD_ARGKND) - 1
			);
	const unsigned pipelayout_md_id = LLVMGetMDKindIDInContext(
			C, ACLSPV_MD_PIPELINE_LAYOUT
			, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1
			);

	unsigned i;

	LLVMMetadataRef pc_ranges_node;
	LLVMMetadataRef set_layouts_node;

	LLVMValueRef F, FNxt;

	for (F = LLVMGetFirstFunction(M); F; F = FNxt) {
		const LLVMValueRef	arg_kinds_node = LLVMGetMetadata(F, arg_kind_md_id);
		const unsigned nprms = LLVMGetMDNodeNumOperands(arg_kinds_node);
		uint32_t push_constant_size = 0;
		unsigned num_descriptors = 0;

		FNxt = LLVMGetNextFunction(F);

		if (LLVMGetFunctionCallConv(F) != LLVMSPIRKERNELCallConv) {
			continue;
		}

		unless(arg_kinds_node) return FN_ACLSPV_PASS_FAILED_FND_ARGKND;

		if (nprms == 0) continue;


		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v0, (size_t)(nprms * sizeof(LLVMValueRef)));
#define arg_kinds_node_opers	((LLVMValueRef* ae2f_restrict)CTX->m_v0.m_p)

		LLVMGetMDNodeOperands(arg_kinds_node, arg_kinds_node_opers);

		for (i = nprms; i--; ) {
			const LLVMValueRef op = arg_kinds_node_opers[i];
			unsigned len;
			const char* ae2f_restrict const kind_str = LLVMGetMDString(op, &len);


			if (strcmp(kind_str, ACLSPV_ARGKND_POD_PSHCONST) == 0) {
				push_constant_size += (uint32_t)LLVMStoreSizeOfType(TD, LLVMTypeOf(LLVMGetParam(F, i)));
			} else if (strcmp(kind_str, ACLSPV_ARGKND_LOC) != 0) {
				num_descriptors++;
			}
		}

		/** Create Push Constant Range Metadata */

		if (push_constant_size > 0) {
			LLVMMetadataRef pc_range_ops[3], pc_range_node;

			pc_range_ops[0] = LLVMValueAsMetadata(
					LLVMConstInt(
						LLVMInt32TypeInContext(C)
						, (unsigned long)VK_SHADER_STAGE_COMPUTE_BIT
						, (LLVMBool)0
						)
					);
			pc_range_ops[1] = LLVMValueAsMetadata(LLVMConstInt(LLVMInt32TypeInContext(C), 0, 0)); 
			pc_range_ops[2] = LLVMValueAsMetadata(LLVMConstInt(LLVMInt32TypeInContext(C), push_constant_size, 0));

			pc_range_node = LLVMMDNodeInContext2(C, pc_range_ops, 3);
			pc_ranges_node = LLVMMDNodeInContext2(C, &pc_range_node, 1);
		} else {
			pc_ranges_node = LLVMMDNodeInContext2(C, ae2f_NIL, 0);
		}

#if 1
		{
			/** Create Descriptor Set Layout Metadata */
			LLVMMetadataRef set_layout_ops[2];
			LLVMMetadataRef	set_layout_node;

			if (num_descriptors > 0) {
				unsigned current_binding = 0;

				_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v1, (size_t)(sizeof(LLVMMetadataRef) * num_descriptors));
#define	bindings	ae2f_static_cast(LLVMMetadataRef*, CTX->m_v1.m_p)

				for (i = nprms; i--; ) {
					const LLVMValueRef op = arg_kinds_node_opers[i]; 
					unsigned len;
					const char* ae2f_restrict const kind_str = LLVMGetMDString(op, &len);

					const unsigned desc_type = get_descriptor_type(kind_str);
					LLVMMetadataRef binding_ops[4];

					if (strcmp(kind_str, ACLSPV_ARGKND_POD_PSHCONST) == 0 || strcmp(kind_str, ACLSPV_ARGKND_LOC) == 0) {
						continue;
					}

					assert(desc_type != (unsigned)-1);

					binding_ops[0] = LLVMValueAsMetadata(LLVMConstInt(LLVMInt32TypeInContext(C), current_binding, 0));
					binding_ops[1] = LLVMValueAsMetadata(LLVMConstInt(LLVMInt32TypeInContext(C), desc_type, 0));
					binding_ops[2] = LLVMValueAsMetadata(LLVMConstInt(LLVMInt32TypeInContext(C), 1, 0)); /** count */
					binding_ops[3] = LLVMValueAsMetadata(LLVMConstInt(LLVMInt32TypeInContext(C), VK_SHADER_STAGE_COMPUTE_BIT, 0));

					bindings[current_binding++] = (LLVMMetadataRef)LLVMMDNodeInContext2(C, binding_ops, 4);
				}

				/** set number */
				set_layout_ops[0] = LLVMValueAsMetadata(LLVMConstInt(LLVMInt32TypeInContext(C), 0, 0)); 

				/** bindings node */
				set_layout_ops[1] = LLVMMDNodeInContext2(C, bindings, (size_t)num_descriptors);
				set_layout_node = LLVMMDNodeInContext2(C, set_layout_ops, 2);
				set_layouts_node = LLVMMDNodeInContext2(C, &set_layout_node, 1);
			} else {
				set_layouts_node = LLVMMDNodeInContext2(C, ae2f_NIL, 0);
			}
		}
#undef	bindings
#undef	arg_kinds_node_opers

#endif

		/** Create final pipeline layout metadata */
		{
			LLVMMetadataRef layout_ops[2];
			layout_ops[0] = pc_ranges_node;
			layout_ops[1] = set_layouts_node;

			LLVMSetMetadata(F, pipelayout_md_id, LLVMMetadataAsValue(C, LLVMMDNodeInContext2(C, layout_ops, 2)));
		}
	}

	return FN_ACLSPV_PASS_OK;
}


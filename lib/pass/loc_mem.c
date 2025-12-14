/**
 * @file	loc_mem.c
 * @brief	implementation for `aclspv_pass_loc_mem`.
 *
 * Strategy: Promote __local allocas to Workgroup globals.
 * This enables direct mapping to SPIR-V OpVariable in Workgroup storage class.
 */

#include "./ctx.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <ae2f/c90/Limits.h>
#include "./md.h"


/** Address space for SPIR-V's Workgroup storage class */
#define SPIRV_ADDR_SPACE_WORKGROUP 3

IMPL_PASS_RET aclspv_pass_loc_mem(
		const LLVMModuleRef M,
		const h_aclspv_pass_ctx CTX
		) 
{
	LLVMNamedMDNodeRef kernels_md = LLVMGetNamedMetadata(M, ACLSPV_MD_OCL_KERNELS, sizeof(ACLSPV_MD_OCL_KERNELS) - 1);
	unsigned count = 0;

	const unsigned num_kernels = LLVMGetNamedMetadataNumOperands(M, ACLSPV_MD_OCL_KERNELS);
	unsigned i = 0;

	unless (kernels_md) {
		return FN_ACLSPV_PASS_OK;
	}

#define	allocas_to_promote	((LLVMValueRef* ae2f_restrict)CTX->m_v0.m_p)
#define allocas_to_promote_cap	((size_t)((CTX->m_v0.m_sz) / ((size_t)sizeof(LLVMValueRef))))

#define kernel_nodes		((LLVMValueRef* ae2f_restrict)CTX->m_v1.m_p)

	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v1, (size_t)(sizeof(LLVMValueRef) * num_kernels));
	unless (kernel_nodes) {
		return FN_ACLSPV_PASS_ALLOC_FAILED;
	}

	LLVMGetNamedMetadataOperands(M, ACLSPV_MD_OCL_KERNELS, kernel_nodes);

	for (i = 0; i < num_kernels; ++i) {
		LLVMValueRef node = kernel_nodes[i];
		unsigned num_node_ops = LLVMGetMDNodeNumOperands(node);


		if (num_node_ops == 0) {
			continue;
		}
#define	node_ops_md		((LLVMValueRef* ae2f_restrict)CTX->m_v2.m_p)
#define	kernel_func		node_ops_md[0]
		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v2, (size_t)(sizeof(LLVMValueRef) * num_node_ops));
		unless (node_ops_md) {
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		}

		LLVMGetMDNodeOperands(node, node_ops_md);

		if (kernel_func && LLVMIsAFunction(kernel_func) && !LLVMIsDeclaration(kernel_func)) {
			LLVMBasicBlockRef entry_bb = LLVMGetEntryBasicBlock(kernel_func);
			LLVMValueRef	I;
			for (I = LLVMGetFirstInstruction(entry_bb); I; I = LLVMGetNextInstruction(I)) {
				unless(LLVMIsAAllocaInst(I)) continue;
				unless(LLVMIsConstant(LLVMGetOperand(I, 0))) continue;

				if(count == UINT_MAX)
					return FN_ACLSPV_PASS_TOO_BIG;

				/** 
				 * Promote static allocas from the entry block.
				 * This assumes any static alloca in a kernel's entry block
				 * is intended for __local memory.
				 */
				if ((size_t)count >= allocas_to_promote_cap) {
					size_t new_cap = allocas_to_promote_cap
						? allocas_to_promote_cap << 1
						: 32;

					_aclspv_grow_vec_with_copy(
							_aclspv_malloc, _aclspv_free, _aclspv_memcpy
							, L_new, CTX->m_v0
							, ((size_t)sizeof(LLVMValueRef)) * new_cap
							);

					unless(allocas_to_promote)
						return FN_ACLSPV_PASS_ALLOC_FAILED;
				}

				allocas_to_promote[count++] = I;
			}
		}
	}

	if (count == 0) {
		return FN_ACLSPV_PASS_OK;
	}

	/** Promote the collected allocas */
	for (i = 0; i < count; ++i) {
#define alloca_inst	allocas_to_promote[i]
		const LLVMTypeRef alloc_type = LLVMGetAllocatedType(alloca_inst);

		/** Create a new global variable in the Workgroup address space. */
		LLVMValueRef new_global = LLVMAddGlobalInAddressSpace(M, alloc_type, "promoted_local", SPIRV_ADDR_SPACE_WORKGROUP);
		LLVMSetLinkage(new_global, LLVMInternalLinkage);
		LLVMSetInitializer(new_global, LLVMConstNull(alloc_type));

		/** Replace all uses of the alloca with the new global. */
		LLVMReplaceAllUsesWith(alloca_inst, new_global);

		/** Erase the alloca instruction. */
		LLVMInstructionEraseFromParent(alloca_inst);
	}


	return FN_ACLSPV_PASS_OK;
}


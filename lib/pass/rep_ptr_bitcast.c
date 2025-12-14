/** 
 * @file	rep_ptr_bitcast.c
 * @brief	implementation for `aclspv_pass_rep_ptr_bitcast` on `inc/aclspv/pass.h`
 * */

#include "./ctx.h"
#include <assert.h>
#include <stdlib.h>
#include <llvm-c/Target.h>

IMPL_PASS_RET aclspv_pass_rep_ptr_bitcast(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
	size_t count = 0;

#define bitcast_to_rep		((LLVMValueRef* ae2f_restrict)((CTX)->m_v0.m_p))
#define bitcast_to_rep_cap	((size_t)((CTX->m_v0.m_sz) / ((size_t)sizeof(LLVMValueRef))))

	LLVMValueRef	F;
	const LLVMTargetDataRef data_layout = LLVMGetModuleDataLayout(M);
	const LLVMContextRef context = LLVMGetModuleContext(M);
	const LLVMTypeRef int_ptr_type = 
		context && data_layout ? LLVMIntPtrTypeInContext(context, data_layout) : ae2f_NIL;
	const LLVMBuilderRef builder = 
		context ? LLVMCreateBuilderInContext(context) : ae2f_NIL;

	size_t i;

	unless(data_layout && context && int_ptr_type && builder)	
		goto FAILED_WITH_CLEAN;

	/* Iterate through the module and collect all pointer bitcast instructions. */
	for (F = LLVMGetFirstFunction(M); F; F = LLVMGetNextFunction(F)) {
		LLVMBasicBlockRef	BB;

		if (LLVMIsDeclaration(F)) {
			continue;
		}
		for (BB = LLVMGetFirstBasicBlock(F); BB; BB = LLVMGetNextBasicBlock(BB)) {
			LLVMValueRef I;
			for (I = LLVMGetFirstInstruction(BB); I; I = LLVMGetNextInstruction(I)) {
				if (LLVMGetInstructionOpcode(I) == LLVMBitCast) {
					LLVMValueRef operand = LLVMGetOperand(I, 0);
					LLVMTypeRef op_type = LLVMTypeOf(operand);
					if (LLVMGetTypeKind(op_type) == LLVMPointerTypeKind) {
						/* This is a pointer bitcast. Add it to our list. */

						if (count >= bitcast_to_rep_cap) {
							const size_t newsize = bitcast_to_rep_cap ? bitcast_to_rep_cap << 1 : 16;

							_aclspv_grow_vec_with_copy(
									_aclspv_malloc
									, _aclspv_free
									, _aclspv_memcpy
									, L_new
									, CTX->m_v0
									, (size_t)(newsize * ((size_t)sizeof(LLVMValueRef)))
									);

							unless(bitcast_to_rep) 
								goto FAILED_WITH_ALLOC_FAILED;
						}

						bitcast_to_rep[count++] = I;
					}
				}
			}
		}
	}

	if (count == 0) {
		goto RET_WITH_CLEAN;
	}

	/* Setup for replacement. */

	/* Replace the collected bitcast instructions. */
	for (i = count; i--; ) {
		LLVMValueRef bitcast_inst = bitcast_to_rep[i], src_ptr, int_val, new_ptr;
		LLVMTypeRef dst_type;

		/** Position the builder before the instruction to be replaced. */
		LLVMPositionBuilderBefore(builder, bitcast_inst);

		/** Get the source pointer and the destination type. */
		src_ptr = LLVMGetOperand(bitcast_inst, 0);
		dst_type = LLVMTypeOf(bitcast_inst);

		if(LLVMTypeOf(src_ptr) == dst_type)
			continue;

		/** Create a 'ptrtoint' instruction. */
		int_val = LLVMBuildPtrToInt(builder, src_ptr, int_ptr_type, "ptrtoint");

		/** Create an 'inttoptr' instruction. */
		new_ptr = LLVMBuildIntToPtr(builder, int_val, dst_type, "inttoptr");

		/** Replace all uses of the old bitcast with the new pointer. */
		LLVMReplaceAllUsesWith(bitcast_inst, new_ptr);

		/** Erase the old bitcast instruction from its parent basic block. */
		LLVMInstructionEraseFromParent(bitcast_inst);


		/* New: simplify obvious redundancies */
		if (LLVMGetInstructionOpcode(int_val) == LLVMIntToPtr &&
				LLVMGetInstructionOpcode(LLVMGetOperand(int_val, 0)) == LLVMPtrToInt) {
			const LLVMValueRef inner = LLVMGetOperand(LLVMGetOperand(int_val, 0), 0);
			LLVMReplaceAllUsesWith(int_val, inner);
			LLVMInstructionEraseFromParent(int_val);
		}
		if (LLVMGetInstructionOpcode(new_ptr) == LLVMPtrToInt &&
				LLVMGetInstructionOpcode(LLVMGetOperand(new_ptr, 0)) == LLVMIntToPtr) {
			const LLVMValueRef inner = LLVMGetOperand(LLVMGetOperand(new_ptr, 0), 0);
			LLVMReplaceAllUsesWith(new_ptr, inner);
			LLVMInstructionEraseFromParent(new_ptr);
		}
	}

RET_WITH_CLEAN:

	LLVMDisposeBuilder(builder);
	return FN_ACLSPV_PASS_OK;

FAILED_WITH_CLEAN:
	LLVMDisposeBuilder(builder);
	return FN_ACLSPV_PASS_MET_INVAL;

FAILED_WITH_ALLOC_FAILED:
	LLVMDisposeBuilder(builder);
	return FN_ACLSPV_PASS_ALLOC_FAILED;
}

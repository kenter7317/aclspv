/**
 * @file emit_bltin_decl.c
 * @brief Implementation of the aclspv_pass_emit_bltin_decl pass.
 *
 * This pass finds declarations of OpenCL builtin functions (which have
 * mangled names in LLVM IR) and replaces them with declarations of
 * internal, backend-specific functions. This is a key step in lowering
 * OpenCL builtins to SPIR-V operations.
 */
#include "./ctx.h"
#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <llvm-c/DataTypes.h>

#include <string.h>
#include <assert.h>


/** A struct to map OpenCL mangled names to internal builtin names */
typedef struct {
	const char* ae2f_restrict ocl_mangled_name;
	const char* ae2f_restrict internal_name;
} builtin_mapping;

/**
 * A list of builtins to handle.
 * This list can be extended to support more OpenCL builtins.
 * */ 
static const builtin_mapping builtin_map[] = {
    /* === Work-item builtins === */
    {"_Z13get_global_idj",      "aclspv.get_global_id"},
    {"get_global_id",     	"aclspv.get_global_id"},

    {"_Z12get_local_idj",       "aclspv.get_local_id"},
    {"get_local_id",       	"aclspv.get_local_id"},

    {"_Z12get_group_idj",       "aclspv.get_group_id"},
    {"_Z14get_global_sizej",    "aclspv.get_global_size"},
    {"_Z13get_local_sizej",     "aclspv.get_local_size"},
    {"_Z12get_work_dimv",       "aclspv.get_work_dim"},
    {"_Z16get_num_groupsj",     "aclspv.get_num_groups"},

    /* === Barrier === */
    {"_Z7barrierj",             "aclspv.barrier"},

    /* === 32-bit Integer Atomics (OpenCL 1.2 / SPIR-V 1.0 compatible) === */

    /* atomic_add / atomic_sub (return old value) */
    {"_Z10atomic_addPU3AS1Vii", "aclspv.atomic_add"},   /* volatile global  i32* , i32 */
    {"_Z10atomic_subPU3AS1Vii", "aclspv.atomic_sub"},
    {"_Z10atomic_addPU3AS3Vii", "aclspv.atomic_add"},   /* volatile local   i32* , i32 */
    {"_Z10atomic_subPU3AS3Vii", "aclspv.atomic_sub"},

    /* atomic_xchg (return old value) */
    {"_Z11atomic_xchgPU3AS1Vii", "aclspv.atomic_xchg"},
    {"_Z11atomic_xchgPU3AS3Vii", "aclspv.atomic_xchg"},

    /* atomic_inc / atomic_dec (return old value) */
    {"_Z9atomic_incPU3AS1Vi",   "aclspv.atomic_inc"},
    {"_Z9atomic_decPU3AS1Vi",   "aclspv.atomic_dec"},
    {"_Z9atomic_incPU3AS3Vi",   "aclspv.atomic_inc"},
    {"_Z9atomic_decPU3AS3Vi",   "aclspv.atomic_dec"},

    /* atomic_cmpxchg (return old value) */
    {"_Z14atomic_cmpxchgPU3AS1Viii", "aclspv.atomic_cmpxchg"},
    {"_Z14atomic_cmpxchgPU3AS3Viii", "aclspv.atomic_cmpxchg"},

    /* atomic_min / atomic_max (signed & unsigned) */
    {"_Z10atomic_minPU3AS1Vii", "aclspv.atomic_min"},
    {"_Z10atomic_maxPU3AS1Vii", "aclspv.atomic_max"},
    {"_Z10atomic_minPU3AS3Vii", "aclspv.atomic_min"},
    {"_Z10atomic_maxPU3AS3Vii", "aclspv.atomic_max"},


    /** unsigned */
    {"_Z10atomic_minPU3AS1Vjj", "aclspv.atomic_umin"},
    {"_Z10atomic_maxPU3AS1Vjj", "aclspv.atomic_umax"},
    {"_Z10atomic_minPU3AS3Vjj", "aclspv.atomic_umin"},
    {"_Z10atomic_maxPU3AS3Vjj", "aclspv.atomic_umax"},

    /* atomic_and / atomic_or / atomic_xor */
    {"_Z10atomic_andPU3AS1Vii", "aclspv.atomic_and"},
    {"_Z10atomic_orPU3AS1Vii",  "aclspv.atomic_or"},
    {"_Z10atomic_xorPU3AS1Vii", "aclspv.atomic_xor"},
    {"_Z10atomic_andPU3AS3Vii", "aclspv.atomic_and"},
    {"_Z10atomic_orPU3AS3Vii",  "aclspv.atomic_or"},
    {"_Z10atomic_xorPU3AS3Vii", "aclspv.atomic_xor"},
};

IMPL_PASS_RET aclspv_pass_emit_bltin_decl(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
	const size_t num_builtins = sizeof(builtin_map) / sizeof(builtin_map[0]);
	size_t i = num_builtins;

	assert(M);
	IGNORE(CTX);

	while (i--) {
		const char* ae2f_restrict const ocl_name = builtin_map[i].ocl_mangled_name;
		const char* ae2f_restrict const internal_name = builtin_map[i].internal_name;
		const LLVMValueRef ocl_func = LLVMGetNamedFunction(M, ocl_name);
		LLVMTypeRef	func_type;
		LLVMValueRef	internal_func;

		if (!ocl_func || !LLVMIsDeclaration(ocl_func) || !LLVMIsAFunction(ocl_func) || LLVMGetFirstUse(ocl_func) == NULL) {
			/**
			 * The builtin is not declared, has a body, or is not used.
			 * In any case, there's nothing for us to do for this one.
			 * */
			continue;
		}

		assert(LLVMIsAFunction(ocl_func));

		func_type = (LLVMGlobalGetValueType(LLVMIsAFunction(ocl_func)));
		assert(LLVMGetTypeKind(func_type) == LLVMFunctionTypeKind);

		/** Get or insert the internal representation of the builtin function. */
		internal_func = LLVMGetNamedFunction(M, internal_name);

		if (!internal_func) {
			internal_func = LLVMAddFunction(M, internal_name, func_type);
		} else {

			/** Function already exists, ensure type matches. */
			LLVMTypeRef existing_type = LLVMGlobalGetValueType(internal_func);
			assert(LLVMGetTypeKind(func_type) == LLVMFunctionTypeKind);

			if (existing_type != func_type) {
				/**
				 * Type mismatch, which indicates a problem in the IR or the pass logic.
				 * For now, we skip this to avoid creating invalid IR.
				 * */
				return FN_ACLSPV_PASS_MET_INVAL;
			}
		}

		/** Replace all uses of the OpenCL builtin with our internal one. */
		LLVMReplaceAllUsesWith(ocl_func, internal_func);

		/** The old function declaration is now unused and can be removed. */
		LLVMDeleteFunction(ocl_func);
	}

	return FN_ACLSPV_PASS_OK;
}



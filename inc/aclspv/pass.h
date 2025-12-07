/**
 * @file	pass.h
 * @brief	pass utility function declarations for module
 * */

#ifndef aclspv_pass_h
#define aclspv_pass_h

#include <llvm-c/Core.h>
#include <aclspv/abi.h>
#include <ae2f/Keys.h>
#include <ae2f/cc.h>

/** 
 * @enum	e_fn_aclspv_pass
 * @brief	results of `fn_aclspv_pass_t` like functions
 * */
typedef enum {
	FN_ACLSPV_PASS_OK,
	FN_ACLSPV_PASS_MODULE_NIL,
	FN_ACLSPV_PASS_ALLOC_FAILED
} e_fn_aclspv_pass;

/**
 * @class	x_aclspv_pass_ctx
 * @typedef	a_aclspv_pass_ctx
 * @typedef	h_aclspv_pass_ctx
 * */
typedef struct x_aclspv_pass_ctx a_aclspv_pass_ctx, * ae2f_restrict h_aclspv_pass_ctx;

/**
 * @typedef	fn_aclspv_pass_t
 * @brief	pass function
 * */
typedef e_fn_aclspv_pass fn_aclspv_pass_t(const LLVMModuleRef, const h_aclspv_pass_ctx);

/**
 * @function	aclspv_pass_rewr_kern_fn
 * @details	\n
 * 	- Rewrites kernel signatures into SPIR-V compatible form	\n
 * 	- Lowers kernel parameters into buffers or POD arguments.
 *
 * @function	aclspv_pass_set_spec_const_default_val
 * @details	\n
 * 	- specialisation constants
 *
 * @function	aclspv_pass_cluster_pod_kern_args
 * @details	\n
 * 	- Combines all POD arguments into a single struct.
 *
 * @function	aclspv_pass_alloc_descriptor
 * @details	\n
 * 	- Assigns descriptor sets/bindings for each kernel argument.
 * */
ae2f_extern ACLSPV_ABI_DECL
fn_aclspv_pass_t
aclspv_pass_rewr_kern_fn,
	aclspv_pass_set_spec_const_default_val,
	aclspv_pass_cluster_pod_kern_args,
	aclspv_pass_alloc_descriptor;

/**
 * @function	aclspv_pass_longvec_lower
 * @details	\n
 * 	- Split illegal vector sizes (e.g., i8x3) into legal types.
 *
 * @function	aclspv_pass_spirv_bltin_lower
 * @details	\n
 * 	- Lower OpenCL builtins to portable LLVM IR.
 *
 * @function	aclspv_pass_rep_cl_bltin
 * @details	\n
 * 	- Lowers get_global_id, get_local_id, barriers, etc.
 *
 * @function	aclspv_pass_ubo_trnsfrm
 * @details	\n
 * 	- Converts `__constant` memory (AS 2) into Uniform buffer objects.
 *
 * @function	aclspv_pass_splat_arg
 * @details	\n
 * 	- Vulkan disallows certain aggregates; this expands scalars.
 *
 * @function	aclspv_pass_direct_rsc_access
 * @details	\n
 * 	- Flatten pointer accesses (Vulkan does not allow arbitrary pointer chains).
 *
 * @function	aclspv_pass_simplify_ptr_bitcast
 * @details	\n
 * 	- Removes pointer bitcasts that Vulkan SPIR-V cannot represent.
 *
 * */
ae2f_extern ACLSPV_ABI_DECL
fn_aclspv_pass_t
aclspv_pass_longvec_lower,
	aclspv_pass_spirv_bltin_lower,
	aclspv_pass_rep_cl_bltin,
	aclspv_pass_ubo_trnsfrm,
	aclspv_pass_splat_arg,
	aclspv_pass_direct_rsc_access,
	aclspv_pass_simplify_ptr_bitcast;

/**
 * @function	aclspv_pass_spirv_lower_memset
 * @brief	\n
 * 	- Converts memcpy/memset to loops.
 *
 * @function	aclspv_pass_fixup_struct_cfg
 * @details	\n
 * 	- Vulkan SPIR-V requires structured control-flow.
 *
 * @function	aclspv_pass_sclrse_masked_mem_intrin
 * @details	\n
 * 	- Removes masked load/store intrinsics.
 *
 * @function	aclspv_pass_infer_addr_space
 * @details	\n
 * 	- Converts OpenCL AS 1/2/3/4 → SPIR-V logical storage classes:	\n
 * 	- StorageBuffer / UniformConstant / Workgroup / CrossWorkgroup
 *
 * @function	aclspv_pass_lower_opq_ptr
 * @details	\n
 *	- SPIR-V does not support opaque pointers.
 * */
ae2f_extern ACLSPV_ABI_DECL
fn_aclspv_pass_t
aclspv_pass_spirv_lower_memset,
	aclspv_pass_fixup_struct_cfg,
	aclspv_pass_sclrse_masked_mem_intrin,
	aclspv_pass_infer_addr_space,
	aclspv_pass_lower_opq_ptr;

/**
 * @function	aclspv_pass_inline_func
 * @details	\n
 * 	- Many calling conventions are illegal in SPIR-V.
 *
 * @function	aclspv_pass_srao
 * @details	\n
 * 	- Breaks illegal struct aggregates.
 *
 * @function	aclspv_pass_glob_opt
 * @details	\n
 * 	- Vulkan rejects unused globals.
 *
 * @function	aclspv_pass_glob_dce
 * @details	\n
 * 	- Remove dead globals.
 *
 * @function	aclspv_pass_dead_store_del
 * @details	\n
 * 	- Some stores are illegal.
 *
 * */
ae2f_extern ACLSPV_ABI_DECL
fn_aclspv_pass_t
aclspv_pass_inline_func,
	aclspv_pass_srao,
	aclspv_pass_glob_opt,
	aclspv_pass_glob_dce,
	aclspv_pass_dead_store_del;

/**
 * @function	aclspv_pass_spirv_prepare_fn
 * @details	\n
 * 	- Marks entry points, removes unsupported attributes.
 *
 * @function	aclspv_pass_spirv_lower_bool
 * @details	\n
 * 	- Boolean legalization (SPIR-V requires 32-bit booleans).
 *
 * @function	aclspv_pass_spirv_lower_constexpr
 * @details	\n
 * 	- SPIR-V cannot encode LLVM constant expressions → must be lowered.
 *
 * @function	aclspv_pass_emit_metadata
 * @details	\n
 * 	- Prepare descriptor maps and SPIR-V decorations.
 * */
ae2f_extern ACLSPV_ABI_DECL
fn_aclspv_pass_t
aclspv_pass_spirv_prepare_fn,
	aclspv_pass_spirv_lower_bool,
	aclspv_pass_spirv_lower_constexpr,
	aclspv_pass_emit_metadata;


/**
 * @enum	e_aclspv_passes
 * @brief	the given number for passes
 * */
typedef enum {
	ACLSPV_PASSES_OK,
	ACLSPV_PASSES_REWR_KERN_FN,
	ACLSPV_PASSES_SET_SPEC_CONST_DEFAULT_VAL,
	ACLSPV_PASSES_CLUSTER_POD_KERN_ARGS,
	ACLSPV_PASSES_ALLOC_DESCRIPTOR,
	ACLSPV_PASSES_LONGVEC_LOWER,
	ACLSPV_PASSES_SPIRV_BLTIN_LOWER,
	ACLSPV_PASSES_REP_CL_BLTIN,
	ACLSPV_PASSES_UBO_TRNSFRM,
	ACLSPV_PASSES_DIRECT_RSC_ACCESS,
	ACLSPV_PASSES_SIMPLIFY_PTR_BITCAST,
	ACLSPV_PASSES_SPIRV_LOWER_MEMSET,
	ACLSPV_PASSES_SCLRSE_MASKED_MEM_INTRIN,
	ACLSPV_PASSES_INFER_ADDR_SPACE,
	ACLSPV_PASSES_LOWER_OPQ_PTR,
	ACLSPV_PASSES_SPLAT_ARG,
	ACLSPV_PASSES_INLINE_FUNC,
	ACLSPV_PASSES_SRAO,
	ACLSPV_PASSES_GLOB_OPT,
	ACLSPV_PASSES_GLOB_DCE,
	ACLSPV_PASSES_DEAD_STORE_DEL,
	ACLSPV_PASSES_FIXUP_STRUCT_CFG,
	ACLSPV_PASSES_SPIRV_LOWER_BOOL,
	ACLSPV_PASSES_SPIRV_LOWER_CONSTEXPR,
	ACLSPV_PASSES_SPIRV_PREPARE_FN,
	ACLSPV_PASSES_EMIT_METADATA
} e_aclspv_passes;


/**
 * @function	aclspv_runall_module_passes
 * @brief	run all passes for a module
 * @param	h_module	<HANDLE>	\n
 * a module to run passes
 *
 * @param	wr_res_opt	<WR> <OPT>	\n
 * When not null, 
 *
 * @return	\n
 * `ACLSPV_PASSES_OK` (0) on success.
 * When not, the given number for a function will be returned.
 * */
ae2f_extern ACLSPV_ABI_DECL
e_aclspv_passes	aclspv_runall_module_passes(
		const LLVMModuleRef		h_module, 
		e_fn_aclspv_pass* ae2f_restrict	const wr_res_opt
		);

#endif

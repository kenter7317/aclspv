/** @file compiler.h  @brief compiles */

#ifndef aclspv_compiler_h
#define aclspv_compiler_h

#include <ae2f/Keys.h>
#include <stddef.h>
#include "./abi.h"
#include "./obj.h"
#include <ae2f/LangVer.h>
#include <clang-c/Index.h>

/**
 * @class	x_aclspv_compiler
 * @brief	a structure to define the data of compiler, not-known in C
 * */
typedef struct x_aclspv_compiler x_aclspv_compiler;

#if ae2f_stdcc_v

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Frontend/CompilerInvocation.h>

struct x_aclspv_compiler {
	/** @var m_cc @brief a compiler instance */
	clang::CompilerInstance		m_cc;

	/** @var m_diag_opts a diagnostic options */
	clang::DiagnosticOptions	m_diag_opts;
	clang::TextDiagnosticPrinter	m_txt_diag_prnt;
	clang::CompilerInvocation	m_invoc;

	llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine>	m_diag_engine;

	inline x_aclspv_compiler();
};

#endif



/**
 * @class	h_aclspv_compiler_t
 * @brief	handle for a compiler
 * */
typedef x_aclspv_compiler* h_aclspv_compiler_t;

/**
 * @enum	e_aclspv_init_compiler
 * @brief	result for `aclspv_init_compiler`
 * */
typedef enum {
	/** @brief good */
	ACLSPV_INIT_COMPILER_OK,

	/** argument `i_compiler` was null */
	ACLSPV_INIT_COMPILER_ARG_WAS_NULL
} e_aclspv_init_compiler;

/**
 * @method	aclspv_compiler_init
 * @memberof	h_aclspv_compiler_t
 * @brief	Initialise the instance of the compiler
 *
 * @returns	<NEW:aclspv_stop_compiler>	\n
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_retnew
h_aclspv_compiler_t	aclspv_mk_compiler(void);

/**
 * @method	aclspv_compiler_stop
 * @memberof	h_aclspv_compiler_t
 * @brief	Deinitialise the instance of the compiler
 *
 * @param	d_compiler	<FREE>				\n
 * a valid compiler object to turn off
 * */
ae2f_extern ACLSPV_ABI_DECL
void	aclspv_free_compiler(h_aclspv_compiler_t d_compiler);

/**
 * @method	aclspv_compile
 * @memberof	h_aclspv_compiler_t
 * @brief	compile files and generate an object
 * @returns	<INIT:aclspv_free_obj>
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_retnew
h_aclspv_obj_t	aclspv_compile(
		x_aclspv_compiler* ae2f_restrict const	h_compiler,
		const char* ae2f_restrict const	rd_srcpath,
		const  struct CXUnsavedFile* ae2f_restrict const 	rd_unsaved,
		const size_t						c_unsaved_len,
		const char* ae2f_restrict const * ae2f_restrict const	rd_argv_opt,
		const size_t						c_argc
		);

#endif

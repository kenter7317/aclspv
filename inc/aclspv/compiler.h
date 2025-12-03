/** @file compiler.h  @brief compiles */

#ifndef aclspv_compiler_h
#define aclspv_compiler_h

#include <ae2f/Keys.h>
#include <stddef.h>
#include "./abi.h"
#include <clang-c/CXFile.h>
#include "./obj.h"

/**
 * @class	x_aclspv_compiler
 * @brief	OpenCL -> SPIR-V Compiler Instance
 * @todo	TODO: implement this
 * */
typedef struct {
	void* m_dummy;
} x_aclspv_compiler;

typedef x_aclspv_compiler* ae2f_restrict h_aclspv_compiler_t;

/**
 * @enum	e_aclspv_init_compiler
 * @brief	result for `aclspv_init_compiler`
 * */
typedef enum {
	ACLSPV_INIT_COMPILER_OK,
} e_aclspv_init_compiler;

/**
 * @method	aclspv_compiler_init
 * @memberof	x_aclspv_compiler
 * @brief	Initialise the instance of the compiler
 *
 * @param	i_compiler	<INIT:aclspv_stop_compiler>	\n
 * pointer to a valid compiler object to turn on. 
 * */
ae2f_extern ACLSPV_ABI_DECL
e_aclspv_init_compiler	aclspv_init_compiler(h_aclspv_compiler_t i_compiler);

/**
 * @method	aclspv_compiler_stop
 * @memberof	x_aclspv_compiler
 * @brief	Deinitialise the instance of the compiler
 *
 * @param	s_compiler	<STOP>				\n
 * pointer to a valid compiler object to turn off
 * */
ae2f_extern ACLSPV_ABI_DECL
void	aclspv_stop_compiler(h_aclspv_compiler_t s_compiler);

/**
 * @method	aclspv_compile
 * @memberof	x_aclspv_compiler
 * @brief	compile a files and generate an object
 * @returns	<INIT:aclspv_obj_stop>
 * */
ae2f_extern ACLSPV_ABI_DECL
h_aclspv_obj_t	aclspv_compile(
		h_aclspv_compiler_t	h_compiler,
		CXFile* const ae2f_restrict		rd_h_files,
		const size_t				c_files_len,
		const char* ae2f_restrict const * ae2f_restrict const	rd_compiler_arg_opt,
		const size_t						rd_compiler_arg_len
		);

#endif

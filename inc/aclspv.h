/**
 * @file aclspv.h
 * Compiles OpenCL C kernel code to SPIR-V for Vulkan.
 * */

#ifndef aclspv_h
#define aclspv_h

#include <ae2f/Keys.h>
#include "./aclspv/abi.h"
#include <ae2f/c90/StdInt.h>

/**
 * @enum	e_aclspv_init_global
 * @brief	result value for `aclspv_init_global`
 * */
typedef enum {
	ACLSPV_INIT_GLOBAL_OK
} e_aclspv_init_global;

/**
 * @function	aclspv_init_global
 * @brief	initialise the library's global state
 * */
ae2f_extern ACLSPV_ABI_DECL
e_aclspv_init_global	aclspv_init_global(void);

/**
 * @function	aclspv_stop_global
 * @brief	stops the library's globla state
 * */
ae2f_extern ACLSPV_ABI_DECL
void	aclspv_stop_global(void);

/** 
 * @typedef	aclspv_wrd_t 
 * @brief	Word of spir-v as 32-bit unsigned integer.
 * */
typedef uint32_t aclspv_wrd_t;

#endif

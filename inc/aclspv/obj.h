/** @file obj.h @brief compiled */

#ifndef aclspv_obj_h
#define aclspv_obj_h

#include <llvm-c/Core.h>
#include "./abi.h"
#include <ae2f/Keys.h>

/** 
 * @class	h_aclspv_obj_t
 * @brief	an object compiled by a `x_aclspv_compiler`.
 * */
typedef LLVMModuleRef h_aclspv_obj_t;

/** 
 * @method	aclspv_obj_stop 
 * @memberof	h_aclspv_obj_t
 * @brief	stop the life span of `hs`.
 * @param	hs	<HANDLE> <STOP>
 * */
ae2f_extern ACLSPV_ABI_DECL
void	aclspv_obj_stop(const h_aclspv_obj_t hs);

#endif

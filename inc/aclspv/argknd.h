/**
 * @file	argknd.h
 * @brief	argument kinds name
 * */

#ifndef aclspv_argknd_h
#define aclspv_argknd_h

/**
 * @def		ACLSPV_ARGKND_POD
 * @brief 	argument kind:: POD	(Plain Old Data)
 *
 * @def		ACLSPV_ARGKND_POD_UBO
 * @brief 	argument kind:: POD	(Plain Old Data)
 * @details	as UBO			(Uniform Buffer Object)
 *
 * @def		ACLSPV_ARGKND_POD_PSHCONST
 * @brief 	argument kind:: POD	(Plain Old Data)
 * @details	as push constant
 * */
#define ACLSPV_ARGKND_POD		"pod"
#define ACLSPV_ARGKND_POD_UBO		"pod_ubo"
#define ACLSPV_ARGKND_POD_PSHCONST	"pod_pushconstant"

/**
 * @def		ACLSPV_ARGKND_BUFF
 * @brief	argument kind:: storage buffer
 *
 * @def		ACLSPV_ARGKND_BUFF_UBO
 * @brief	argument kind:: storage buffer
 * @details	as UBO			(Uniform Buffer Object)
 * */
#define	ACLSPV_ARGKND_BUFF	"buffer"
#define	ACLSPV_ARGKND_BUFF_UBO	"buffer_ubo"

/**
 * @def		ACLSPV_ARGKND_RO_IMG
 * @brief	argument kind:: read-only image
 *
 * @def		ACLSPV_ARGKND_WO_IMG
 * @brief	argument kind:: write-only image
 * */
#define	ACLSPV_ARGKND_RO_IMG	"read_only_image"
#define	ACLSPV_ARGKND_WO_IMG	"write_only_image"

/**
 * @def		ACLSPV_ARGKND_SMPLR
 * @brief	argument kind:: sampler
 * */
#define ACLSPV_ARGKND_SMPLR	"sampler"

/**
 * @def		ACLSPV_ARGKND_LOC
 * @brief	argument kind:: local
 * */
#define ACLSPV_ARGKND_LOC	"local"

#endif

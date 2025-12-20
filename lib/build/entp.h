#ifndef __lib_build_entp
#define __lib_build_entp

#include <aclspv/spvty.h>
#include <llvm-c/Core.h>

typedef struct {
	aclspv_wrd_t	m_id;
	LLVMValueRef	m_fn;
} lib_build_entp_t;

#endif

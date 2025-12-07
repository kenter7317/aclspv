#ifndef	aclspv_pass_ctx_h
#define	aclspv_pass_ctx_h

#include <aclspv/pass.h>
#include "./ctx.auto.h"
#include <ae2f/c90/Limits.h>
#include <ae2f/c90/StdBool.h>
#include <ae2f/c90/StdInt.h>

typedef struct {
	void* ae2f_restrict 	
		m_p;
	size_t	m_sz;
} aclspv_vec;

struct x_aclspv_pass_ctx {
	aclspv_vec	m_v0, m_v1;
};

#endif

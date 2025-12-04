#include <aclspv/abi.h>
#undef ON
#undef OFF

#include <aclspv/compiler.h>
#include "./compiler.auto.h"
#include "./compiler.h"

inline x_aclspv_compiler::x_aclspv_compiler() : 
	m_cc(), 
	m_diag_opts(), 
	m_txt_diag_prnt(llvm::errs(), this->m_diag_opts),
	m_diag_engine(new clang::DiagnosticsEngine(
				new clang::DiagnosticIDs(), this->m_diag_opts
				))
{
	assert(this->m_diag_engine.get());
	this->m_cc.getFrontendOpts().Inputs.clear();
	this->m_cc.getFrontendOpts().Inputs.resize(1);
	this->m_cc.setDiagnostics(this->m_diag_engine.get());
}

ae2f_extern ACLSPV_ABI_IMPL ae2f_retnew
h_aclspv_compiler_t	aclspv_mk_compiler(void) {
	return new x_aclspv_compiler();
}

ae2f_extern ACLSPV_ABI_IMPL
void	aclspv_free_compiler(h_aclspv_compiler_t _) {
	delete _;
}


	ae2f_extern ACLSPV_ABI_IMPL ae2f_retnew
h_aclspv_obj_t	aclspv_compile(
		x_aclspv_compiler* ae2f_restrict const	h_compiler,
		const char* ae2f_restrict const	rd_srcpath,
		const  struct CXUnsavedFile* const ae2f_restrict	rd_unsaved,
		const size_t						c_unsaved_len,
		const char* ae2f_restrict const * ae2f_restrict const	rd_argv_opt,
		const size_t						c_argc
		)
{
	size_t				t_sz0;
	clang::EmitLLVMOnlyAction	t_cxaction;
	h_aclspv_obj_t			ret;

	if(rd_argv_opt && c_argc) {
		assert(!"TEST BARRIER");

		std::vector<const char* ae2f_restrict>
			rd_argv(compiler_argc + c_argc + 1);

		rd_argv[0] = rd_srcpath;
		rd_argv[1] = compiler_arg0;
		rd_argv[2] = compiler_arg1;
		rd_argv[3] = compiler_arg2;
		rd_argv[4] = compiler_arg3;
		rd_argv[5] = compiler_arg4;
		rd_argv[6] = compiler_arg5;

		memcpy(rd_argv.data() + c_argc + 1, rd_argv_opt, c_argc * sizeof(void*));

		_aclspv_compile_imp(
				t_sz0, t_cxaction, ret
				, *h_compiler, rd_srcpath
				, rd_unsaved, c_unsaved_len
				, rd_argv.data(), (compiler_argc + c_argc + 1)
				);
	} else {
		const char* ae2f_restrict rd_argv[] = {
			rd_srcpath,
			compiler_arg0,
			compiler_arg1,
			compiler_arg2,
			compiler_arg3,
			compiler_arg4,
			compiler_arg5
		};

		_aclspv_compile_imp(
				t_sz0, t_cxaction, ret
				, *h_compiler, rd_srcpath
				, rd_unsaved, c_unsaved_len
				, rd_argv, (sizeof(rd_argv) / sizeof(rd_argv[0]))
				);
	}

	return ret;
}

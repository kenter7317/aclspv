#undef	__ae2f_MACRO_GENERATED
#define	__ae2f_MACRO_GENERATED	1
#ifndef aclspv_compiler_auto_h
#define aclspv_compiler_auto_h

#include <aclspv/compiler.h>
#undef __ae2f_MACRO_GENERATED
#define __ae2f_MACRO_GENERATED 1
#include <aclspv/obj.h>
#undef __ae2f_MACRO_GENERATED
#define __ae2f_MACRO_GENERATED 1
#include <ae2f/Macro.h>
#undef __ae2f_MACRO_GENERATED
#define __ae2f_MACRO_GENERATED 1

#include <clang/CodeGen/CodeGenAction.h>
#undef __ae2f_MACRO_GENERATED
#define __ae2f_MACRO_GENERATED 1
#include <clang/Lex/PreprocessorOptions.h>
#undef __ae2f_MACRO_GENERATED
#define __ae2f_MACRO_GENERATED 1

#define _aclspv_compile_imp( \
	/** tparam */ \
		 \
 \
	/** param */ \
		/*    ,size_t&      ae2f_restrict */ t_sz0, \
		/*    clang::EmitLLVMOnlyAction&    */ t_cxaction, \
		/*     h_aclspv_obj_t&      */ ret, \
		/*     x_aclspv_compiler&   ae2f_restrict */ h_compiler, \
		/*     constchar*  ae2f_restrict const */ rd_srcpath, \
		/*     const structCXUnsavedFile*  ae2f_restrict */ rd_unsaved, \
		/*     const     size_t */ c_unsaved_len, \
		/*      constchar*  ae2f_restrictconst*  ae2f_restrict const */ rd_argv, \
		/*     const       size_t    */ c_argc \
)  \
{ \
	do { \
		unless(clang::CompilerInvocation::CreateFromArgs( \
					(h_compiler).m_invoc \
					, clang::ArrayRef<const char*>(rd_argv, rd_argv + c_argc) \
					, *((h_compiler).m_diag_engine.get()) \
					)) \
		{ \
			assert(!"CreateFromArgs has failed."); \
			(ret) = ae2f_NIL; \
			break; \
		} \
 \
		(h_compiler).m_cc.getFrontendOpts().Inputs[0] \
			= clang::FrontendInputFile( \
					rd_srcpath \
					, clang::InputKind( \
						clang::Language::OpenCL \
						, clang::InputKind::Format::Source \
						) \
					); \
 \
		for((t_sz0) = (c_unsaved_len); (t_sz0)--; ) { \
			(h_compiler).m_cc \
				.getPreprocessorOpts() \
				.addRemappedFile( \
						(rd_unsaved)[t_sz0].Filename \
						, clang::StringRef((rd_unsaved)[t_sz0].Contents, (rd_unsaved)[t_sz0].Length) \
						); \
		} \
 \
		unless((h_compiler).m_cc.ExecuteAction(t_cxaction)) { \
			assert(!"Failed to run EmitLLVMOnlyAction"); \
			(ret) = ae2f_NIL; \
			break; \
		} \
 \
		(ret) = new x_aclspv_obj((t_cxaction).takeModule()); \
	} while(0); \
}

#endif

#undef	__ae2f_MACRO_GENERATED

#define	__ae2f_MACRO_GENERATED	0


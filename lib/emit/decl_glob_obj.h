#include "aclspv/spvty.h"
#include "util/id.h"
#include "util/iddef.h"
#include <assert.h>
#include <clang-c/CXString.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <util/ctx.h>
#include <util/emitx.h>
#include <util/is_kernel.h>
#include <util/fn.h>
#include <util/entp.h>
#include <util/bind.h>
#include <util/constant.h>
#include <util/scale.h>

#include <ae2f/Keys.h>
#include <ae2f/c90/StdBool.h>
#include <ae2f/c90/StdInt.h>

#include <spirv/1.0/spirv.h>
#include <clang-c/Index.h>

static enum CXChildVisitResult	emit_decl_glob_obj_visit_attr_set(CXCursor h_cur, CXCursor h_parent, CXClientData ae2f_restrict h_data) {
	CXString TEXT;
	enum CXChildVisitResult RES = CXChildVisit_Break;
	char* NEEDLE;
	unsigned SET;

	unless(h_cur.kind == CXCursor_AnnotateAttr) return CXChildVisit_Recurse;
	TEXT = clang_getCursorSpelling(h_cur);

	unless(NEEDLE = strstr(TEXT.data, "aclspv_set")) {
		RES = CXChildVisit_Continue;
		goto LBL_FINI;
	}


	sscanf(NEEDLE, "aclspv_set ( %u )", &SET);

	*((aclspv_wrd_t* ae2f_restrict)h_data) = (aclspv_wrd_t)SET;

LBL_FINI:
	clang_disposeString(TEXT);
	return RES;

	(void)h_parent;
}

static enum CXChildVisitResult	emit_decl_glob_obj_visit_attr_specid(CXCursor h_cur, CXCursor h_parent, CXClientData ae2f_restrict h_data) {
	CXString TEXT;
	enum CXChildVisitResult RES = CXChildVisit_Break;
	char* NEEDLE;
	unsigned SPECID;

	unless(h_cur.kind == CXCursor_AnnotateAttr) return CXChildVisit_Recurse;
	TEXT = clang_getCursorSpelling(h_cur);

	unless(NEEDLE = strstr(TEXT.data, "aclspv_specid")) {
		RES = CXChildVisit_Continue;
		goto LBL_FINI;
	}


	sscanf(NEEDLE, "aclspv_specid ( %u )", &SPECID);

	*((aclspv_wrd_t* ae2f_restrict)h_data) = (aclspv_wrd_t)SPECID;

LBL_FINI:
	clang_disposeString(TEXT);
	return RES;

	(void)h_parent;
}

static enum CXChildVisitResult emit_decl_glob_obj_visit_fetch(CXCursor h_cur, CXCursor h_parent, CXClientData h_data) {
#if	!defined(NDEBUG) || !NDEBUG

	CXString	PARAM_NAME, PARAM_TY_NAME, FUNC_NAME;
#endif
	CXType		PARAM_TY;

#define	PUSH_SIZE	((uintptr_t* ae2f_restrict)h_data)[0]
#define BIND_IDX	((uintptr_t* ae2f_restrict)h_data)[1]
#define CTX		((h_aclspv_ctx_t)((uintptr_t* ae2f_restrict)h_data)[2])
#define	ARG_IDX		((uintptr_t* ae2f_restrict)h_data)[3]
#define	INTERFACE_COUNT	((uintptr_t* ae2f_restrict)h_data)[4]

	unless(h_cur.kind == CXCursor_ParmDecl) {
		return CXChildVisit_Continue;
	}

	PARAM_TY	= clang_getCursorType(h_cur);

	/** non-pointer type */
	unless(PARAM_TY.kind == CXType_Pointer || PARAM_TY.kind == CXType_ConstantArray) {
		PUSH_SIZE += sz_to_count(clang_Type_getSizeOf(PARAM_TY));
		++ARG_IDX;
		return CXChildVisit_Continue;
	}

	PARAM_TY_NAME	= clang_getTypeSpelling(PARAM_TY);


	/** FETCH BIND */

#define VAR_INFOS		(&((util_bind* ae2f_restrict)CTX->m_vecid_vars.m_p)->m_bindable)
#define INTERFACE_IDS_MGR	get_last_scale_from_vec(&CTX->m_scale_vars)
#define	INTERFACE_IDS		((aclspv_wrd_t* ae2f_restrict)get_buf_from_scale(&CTX->m_scale_vars, *INTERFACE_IDS_MGR))

#define	INFO			(VAR_INFOS + BIND_IDX)

	if (CTX->m_vecid_vars.m_sz <= ((size_t)sizeof(util_bind) * (size_t)BIND_IDX)) {
		_aclspv_grow_vec_with_copy(
				_aclspv_malloc, _aclspv_free, _aclspv_memcpy, L_new
				, CTX->m_vecid_vars
				, CTX->m_vecid_vars.m_sz 
				? CTX->m_vecid_vars.m_sz << 1 
				: (size_t)(sizeof(util_bind) << 3)
				);

		unless(VAR_INFOS) {
			CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
			return CXChildVisit_Break;
		}
	}
	INFO->m_arg_idx		= (aclspv_wrd_t)ARG_IDX;
	INFO->m_binding		= (aclspv_wrd_t)BIND_IDX;
	INFO->m_var_id		= CTX->m_id++;
	INFO->m_struct_id	= util_mk_constant_struct_id(1, CTX);
	INFO->m_ptr_struct_id	= util_mk_constant_ptr_storage_id(1, CTX);
	INFO->m_set		= 0;

	if(grow_last_scale(&CTX->m_scale_vars, (size_t)count_to_sz(INTERFACE_COUNT + 1))) {
		CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
		return CXChildVisit_Break;
	}

	INTERFACE_IDS[INTERFACE_COUNT] = INFO->m_var_id;

	/** FETCH BIND END */
#if	!defined(NDEBUG) || !NDEBUG
	FUNC_NAME	= clang_getCursorSpelling(h_parent);
	PARAM_NAME	= clang_getCursorSpelling(h_cur);

	{
		char* ae2f_restrict	NAME_MERGE = malloc(strlen(FUNC_NAME.data) + strlen(PARAM_NAME.data) + 3);
		const aclspv_wrdcount_t	POS = CTX->m_count.m_name;
		unless(NAME_MERGE) {
LBL_ABRT_NALLOC:
			CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
			free(NAME_MERGE);
			clang_disposeString(FUNC_NAME);
			clang_disposeString(PARAM_NAME);
			clang_disposeString(PARAM_TY_NAME);
			return CXChildVisit_Break;
		}

		strcpy(NAME_MERGE, FUNC_NAME.data);
		strcat(NAME_MERGE, "::");
		strcat(NAME_MERGE, PARAM_NAME.data);

#define EMIT_POS	CTX->m_count.m_name
		unless((EMIT_POS = emit_opcode(&CTX->m_section.m_name, EMIT_POS, SpvOpName, 0))) 
			goto LBL_ABRT_NALLOC;
		unless((EMIT_POS = util_emit_wrd(&CTX->m_section.m_name, EMIT_POS, INFO->m_var_id)))
			goto LBL_ABRT_NALLOC;
		unless((EMIT_POS = util_emit_str(&CTX->m_section.m_name, EMIT_POS, NAME_MERGE))) 
			goto LBL_ABRT_NALLOC;
		set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_section.m_name)[POS], EMIT_POS - POS - 1);
#undef	EMIT_POS

		free(NAME_MERGE);
	}
#endif

#if	!defined(NDEBUG) || !NDEBUG
	clang_disposeString(FUNC_NAME);
	clang_disposeString(PARAM_NAME);
#endif
	if(PARAM_TY.kind == CXType_ConstantArray)
		goto LBL_WORKGROUP; /*  clang_disposeString(PARAM_TY_NAME);  is needed */

	if(strstr(PARAM_TY_NAME.data, "__global")) {
		INFO->m_storage_class = SpvStorageClassStorageBuffer;
	} else if (strstr(PARAM_TY_NAME.data, "__constant")) {
		INFO->m_storage_class = SpvStorageClassUniform;
	} else {
		CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
		return CXChildVisit_Break;
	}

	clang_disposeString(PARAM_TY_NAME);
	clang_visitChildren(h_cur, emit_decl_glob_obj_visit_attr_set, &INFO->m_set);

	CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

	/** Decorations::Descriptorsets */
	unless((CTX->m_count.m_decorate = emit_opcode(&CTX->m_section.m_decorate, CTX->m_count.m_decorate, SpvOpDecorate, 3))) 
		return CXChildVisit_Break;
	unless((CTX->m_count.m_decorate = util_emit_wrd(&CTX->m_section.m_decorate, CTX->m_count.m_decorate, INFO->m_var_id))) 
		return CXChildVisit_Break;
	unless((CTX->m_count.m_decorate = util_emit_wrd(&CTX->m_section.m_decorate, CTX->m_count.m_decorate, SpvDecorationDescriptorSet))) 
		return CXChildVisit_Break;
	unless((CTX->m_count.m_decorate = util_emit_wrd(&CTX->m_section.m_decorate, CTX->m_count.m_decorate, INFO->m_set))) 
		return CXChildVisit_Break;

	/** Decorations::Bind */
	unless((CTX->m_count.m_decorate = emit_opcode(&CTX->m_section.m_decorate, CTX->m_count.m_decorate, SpvOpDecorate, 3))) 
		return CXChildVisit_Break;
	unless((CTX->m_count.m_decorate = util_emit_wrd(&CTX->m_section.m_decorate, CTX->m_count.m_decorate, INFO->m_var_id))) 
		return CXChildVisit_Break;
	unless((CTX->m_count.m_decorate = util_emit_wrd(&CTX->m_section.m_decorate, CTX->m_count.m_decorate, SpvDecorationBinding))) 
		return CXChildVisit_Break;
	unless((CTX->m_count.m_decorate = util_emit_wrd(&CTX->m_section.m_decorate, CTX->m_count.m_decorate, INFO->m_binding))) 
		return CXChildVisit_Break;

	if(INFO->m_storage_class == SpvStorageClassUniform) {
		INFO->m_storage_class = SpvStorageClassUniform;
		INFO->m_ptr_struct_id = util_mk_constant_ptr_uniform_id(1, CTX);
	}

	unless(CTX->m_count.m_vars = util_emitx_variable(
			&CTX->m_section.m_vars
			, CTX->m_count.m_vars
			, INFO->m_ptr_struct_id
			, INFO->m_var_id
			, INFO->m_storage_class
			)) return CXChildVisit_Break;


	CTX->m_state = ACLSPV_COMPILE_OK;

	++BIND_IDX;

LBL_WORKGROUP_FINI:
	++ARG_IDX;
	++INTERFACE_COUNT;
	return CXChildVisit_Continue;

LBL_WORKGROUP:
#undef	VAR_INFOS
#define VAR_INFOS		(&((util_bind* ae2f_restrict)CTX->m_vecid_vars.m_p)->m_work)
	unless(strstr(PARAM_TY_NAME.data, "__local")) {
		CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
		return CXChildVisit_Break;
	}
	clang_disposeString(PARAM_TY_NAME);

	{
		aclspv_wrd_t	SPECID_WORK = 0xFFFFFFFF;

		const CXType		ELMTY = clang_getArrayElementType(PARAM_TY);
		const int_least64_t	ELMSZ_ORIG = clang_Type_getSizeOf(ELMTY);
		const aclspv_wrd_t	ELMSZ = ELMSZ_ORIG > UINT32_MAX || ELMSZ_ORIG < 0 ? 0 : (aclspv_wrd_t)ELMSZ_ORIG;
		const aclspv_wrd_t	VAL_ELMSZ = ELMSZ ? util_mk_constant_val_id(
				ELMSZ
				, CTX
				) : 0;
		const aclspv_wrd_t	VAL_2 = util_mk_constant_val_id(
				2
				, CTX
				);

		INFO->m_arg_idx		= (aclspv_wrd_t)ARG_IDX;
		INFO->m_storage_class	= SpvStorageClassWorkgroup;


		ae2f_expected_but_else(VAL_2)		return CXChildVisit_Break;
		ae2f_expected_but_else(VAL_ELMSZ)	return CXChildVisit_Break;


		clang_visitChildren(
				h_cur
				, emit_decl_glob_obj_visit_attr_specid
				, &SPECID_WORK
				);

		ae2f_expected_if(SPECID_WORK != 0xFFFFFFFF) {
			util_constant* CONST_NODE = util_mk_constant_node(SPECID_WORK, CTX);

			CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

			unless(CONST_NODE) {
				return CXChildVisit_Break;
			}

			unless(util_get_default_id(ID_DEFAULT_U32, CTX))
				return CXChildVisit_Break;

			unless(util_get_default_id(ID_DEFAULT_I32, CTX))
				return CXChildVisit_Break;

			unless(CONST_NODE->m_const_spec_id) {
				const aclspv_wrdcount_t RETCOUNT = util_emitx_spec_constant(
						&CTX->m_section.m_types
						, CTX->m_count.m_types
						, ID_DEFAULT_U32
						, CTX->m_id
						, SPECID_WORK
						);

				unless(RETCOUNT) {
					return CXChildVisit_Break;
				}

				CONST_NODE->m_const_spec_id = CTX->m_id++;
				CONST_NODE->m_const_spec_type_id = ID_DEFAULT_U32;
				CTX->m_count.m_types = RETCOUNT;
			} else ae2f_expected_but_else(util_is_default_id_int(CONST_NODE->m_const_spec_type_id)) {
				CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
				return CXChildVisit_Break;
			}

			INFO->m_arr_count_id = CONST_NODE->m_const_spec_id;
		} else {
			const intmax_t ARRSZ_ORIG	= clang_getArraySize(PARAM_TY);
			const aclspv_wrd_t ARRSZ	= ARRSZ_ORIG < 0 || ARRSZ_ORIG > UINT32_MAX ? 0 : (aclspv_wrd_t)ARRSZ_ORIG;
			const aclspv_wrdcount_t NUM_ARR	= ARRSZ ? ARRSZ / ELMSZ : 0;

			ae2f_expected_but_else(NUM_ARR) {
				CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
				return CXChildVisit_Break;
			}

			INFO->m_arr_count_id = util_mk_constant_val_id(NUM_ARR, CTX);
		}

		ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_spec_constant_op(
					&CTX->m_section.m_types
					, CTX->m_count.m_types
					, ID_DEFAULT_I32
					, CTX->m_id
					, SpvOpSNegate
					, INFO->m_arr_count_id
					))
			return CXChildVisit_Break;

		ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_spec_constant_op2(
					&CTX->m_section.m_types
					, CTX->m_count.m_types
					, ID_DEFAULT_I32
					, CTX->m_id + 1
					, SpvOpSDiv
					, CTX->m_id
					, VAL_ELMSZ
					))
			return CXChildVisit_Break;

		ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_spec_constant_op(
					&CTX->m_section.m_types
					, CTX->m_count.m_types
					, ID_DEFAULT_U32
					, CTX->m_id + 2
					, SpvOpSNegate
					, CTX->m_id + 1
					)) return CXChildVisit_Break;

		ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_spec_constant_op2(
					&CTX->m_section.m_types
					, CTX->m_count.m_types
					, ID_DEFAULT_U32
					, CTX->m_id + 3
					, SpvOpShiftLeftLogical
					, CTX->m_id + 2
					, VAL_2
					)) return CXChildVisit_Break;


		
		ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_type_array(
					&CTX->m_section.m_types
					, CTX->m_count.m_types
					, CTX->m_id + 4
					, ID_DEFAULT_U32
					, CTX->m_id + 3
					)) return CXChildVisit_Break;
#if 1
		ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_type_pointer(
					&CTX->m_section.m_types
					, CTX->m_count.m_types
					, CTX->m_id + 5
					, SpvStorageClassWorkgroup /* c_storage_class */
					, CTX->m_id + 4
					)) return CXChildVisit_Break;
#endif

		ae2f_expected_but_else(CTX->m_count.m_vars = util_emitx_variable(
					&CTX->m_section.m_vars
					, CTX->m_count.m_vars
					, CTX->m_id + 5
					, CTX->m_id + 6
					, SpvStorageClassWorkgroup
					)) return CXChildVisit_Break;

		INFO->m_var_id		= CTX->m_id + 6;
		INFO->m_arr_type_id	= CTX->m_id + 4;

		CTX->m_id	+= 7;
		CTX->m_state	 = ACLSPV_COMPILE_OK;

		INTERFACE_IDS[INTERFACE_COUNT] = INFO->m_var_id;

		goto LBL_WORKGROUP_FINI;
	}
#undef	PUSH_SIZE
#undef	BIND_IDX
#undef	CTX
#undef	ARG_IDX
#undef	INTERFACE_COUNT
#undef	VAR_INFOS
#undef	INTERFACE_IDS_MGR
#undef	INTERFACE_IDS
#undef	INFO
}

static enum CXChildVisitResult emit_decl_glob_obj(CXCursor h_cur, CXCursor h_cur_parent, CXClientData _h_ctx) {
#define h_ctx	((x_aclspv_ctx* ae2f_restrict)_h_ctx)
	assert(h_ctx);


	unless(h_cur.kind == CXCursor_CompoundStmt && h_cur_parent.kind == CXCursor_FunctionDecl) 
		return CXChildVisit_Recurse;

	if(util_is_kernel(h_cur_parent)) {
		uintptr_t BUFF[6] = {
			0 /* push constant size */ , 
			0 /* bind index */, 
			0 /* ctx */,
			0 /* argument index */,
			0 /* interface count */,
		};

		const int NPARAMS = clang_Cursor_getNumArguments(h_cur_parent);

		BUFF[4] = h_ctx->m_tmp.m_w2;

		if(NPARAMS < 0) {
			h_ctx->m_state = ACLSPV_COMPILE_MET_INVAL;
			return CXChildVisit_Break;
		}

		((util_entp_t* ae2f_restrict)h_ctx->m_fnlist.m_entp.m_p)[h_ctx->m_tmp.m_w0]
			.m_fn = h_cur;
		((util_entp_t* ae2f_restrict)h_ctx->m_fnlist.m_entp.m_p)[h_ctx->m_tmp.m_w0]
			.m_id = h_ctx->m_tmp.m_w3 + h_ctx->m_fnlist.m_num_entp;
		((util_entp_t* ae2f_restrict)h_ctx->m_fnlist.m_entp.m_p)[h_ctx->m_tmp.m_w0]
			.m_id = h_ctx->m_tmp.m_w3 + h_ctx->m_fnlist.m_num_entp;

		BUFF[2] = (uintptr_t)h_ctx;

		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, h_ctx->m_tmp.m_v0, (size_t)((unsigned)NPARAMS * sizeof(CXType)));
		unless(h_ctx->m_tmp.m_v0.m_p) {
			h_ctx->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
			return CXChildVisit_Break;
		}

		clang_visitChildren(h_cur_parent, emit_decl_glob_obj_visit_fetch, BUFF);
		h_ctx->m_tmp.m_w2 = (aclspv_wrd_t)BUFF[4];
		if(h_ctx->m_state) return CXChildVisit_Break;

		if(BUFF[0]) {
			aclspv_id_t PSH_PTR_ID 	= util_mk_constant_ptr_psh_id((aclspv_wrd_t)BUFF[0], h_ctx);
			aclspv_id_t PSH_VAR_ID	= h_ctx->m_id++;

			unless(PSH_PTR_ID) {
				h_ctx->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
				return CXChildVisit_Break;
			}

			h_ctx->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
			unless((h_ctx->m_count.m_vars = emit_opcode(&h_ctx->m_section.m_vars, h_ctx->m_count.m_vars, SpvOpVariable, 3))) 
				return CXChildVisit_Break;
			unless((h_ctx->m_count.m_vars = util_emit_wrd(&h_ctx->m_section.m_vars, h_ctx->m_count.m_vars, PSH_PTR_ID))) 
				return CXChildVisit_Break;
			unless((h_ctx->m_count.m_vars = util_emit_wrd(&h_ctx->m_section.m_vars, h_ctx->m_count.m_vars, PSH_VAR_ID))) 
				return CXChildVisit_Break;
			unless((h_ctx->m_count.m_vars = util_emit_wrd(&h_ctx->m_section.m_vars, h_ctx->m_count.m_vars, SpvStorageClassPushConstant))) 
				return CXChildVisit_Break;
			h_ctx->m_state = ACLSPV_COMPILE_OK;
		}

		++h_ctx->m_tmp.m_w0;
	} else {
		((lib_build_fn_t* ae2f_restrict)h_ctx->m_fnlist.m_fn.m_p)[h_ctx->m_tmp.m_w1]
			.m_fn = h_cur_parent;
		((lib_build_fn_t* ae2f_restrict)h_ctx->m_fnlist.m_fn.m_p)[h_ctx->m_tmp.m_w1]
			.m_id = h_ctx->m_tmp.m_w3
			+ (aclspv_wrd_t)((aclspv_wrd_t)h_ctx->m_fnlist.m_entp.m_sz / sizeof(util_entp_t))
			+ h_ctx->m_tmp.m_w1;

		++h_ctx->m_tmp.m_w1;
	}

	return CXChildVisit_Continue;

#undef	h_ctx
	return CXChildVisit_Recurse;
}

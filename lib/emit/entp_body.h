#ifndef	lib_emit_entp_body_h
#define	lib_emit_entp_body_h

#include <complex.h>
#include <stdio.h>
#include <aclspv.h>

#include <ae2f/Keys.h>
#include <ae2f/c90/StdInt.h>
#include <ae2f/c90/StdBool.h>

#include <clang-c/Index.h>
#include <clang-c/CXString.h>

#include <util/ctx.h>
#include <util/emitx.h>
#include <util/iddef.h>
#include <util/constant.h>
#include <util/cursor.h>

#include <attr/specid.h>

#include <spirv/unified1/spirv.h>

#include "./expr.h"

static enum CXChildVisitResult emit_entp_body(CXCursor h_cur, CXCursor h_parent, CXClientData rdwr_data) {
#define	CTX		((h_util_ctx_t)rdwr_data)
#define	jmpfail(c_why)	{ (CTX)->m_err = (c_why); goto LBL_FAIL; }
	const enum CXCursorKind KIND	= (h_cur).kind;
	const CXString KINDSPELL	= clang_getCursorKindSpelling(KIND);
	const CXString SPELL		= clang_getCursorSpelling(h_cur);

	(void)h_parent;
	(void)rdwr_data;

	ae2f_expected_but_else(KIND)	{
		CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
		goto LBL_FAIL;
	}

	switch((uintmax_t)KIND) {
		case CXCursor_ReturnStmt:
			ae2f_expected_but_else(CTX->m_count.m_fndef = emit_opcode(
						&CTX->m_section.m_fndef
						, CTX->m_count.m_fndef
						, SpvOpReturn, 0
						)) goto LBL_FAIL;
			CTX->m_has_function_ret = 1;
			goto LBL_DONE;

		case CXCursor_BinaryOperator:
		case CXCursor_CompoundAssignOperator:
		case CXCursor_UnaryOperator:
		case CXCursor_CallExpr:
		case CXCursor_ArraySubscriptExpr:
		case CXCursor_ConditionalOperator:
		case CXCursor_CStyleCastExpr:
		case CXCursor_DeclRefExpr:
		case CXCursor_IntegerLiteral:
		case CXCursor_FloatingLiteral:
		case CXCursor_UnaryExpr:
			{
				aclspv_id_t EXPR_ID = emit_expr(h_cur, h_parent, CTX);
				(void)EXPR_ID;
			}
			goto LBL_RECURSE;

		case CXCursor_DoStmt:
		case CXCursor_IfStmt:

		case CXCursor_ForStmt:
		case CXCursor_WhileStmt:
		case CXCursor_BreakStmt:
		case CXCursor_ContinueStmt:

		case CXCursor_SwitchStmt:
		case CXCursor_CaseStmt:
		case CXCursor_AsmStmt: case CXCursor_MSAsmStmt:

		case CXCursor_CompoundStmt:
		case CXCursor_StmtExpr:
		default:
			break;
	}


	puts(SPELL.data);
	puts(KINDSPELL.data);
	puts("");

	CTX->m_err = ACLSPV_COMPILE_OK;
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Recurse;

	ae2f_unreachable();
LBL_FAIL:
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Break;

	ae2f_unreachable();
LBL_DONE:
	CTX->m_err = ACLSPV_COMPILE_OK;
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Continue;

	ae2f_unreachable();
LBL_RECURSE:
	CTX->m_err = ACLSPV_COMPILE_OK;
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Recurse;

#undef	CTX
#undef	jmpfail
}

#endif

#include <stdio.h>

#include <ae2f/Keys.h>
#include <ae2f/c90/StdInt.h>
#include <ae2f/c90/StdBool.h>

#include <clang-c/Index.h>
#include <clang-c/CXString.h>

#include <util/ctx.h>
#include <util/wrdemit.h>
#include <util/iddef.h>
#include <util/constant.h>

#include <spirv/unified1/spirv.h>

static enum CXChildVisitResult emit_entp_body(CXCursor h_cur, CXCursor h_parent, CXClientData rdwr_data) {
	const enum CXCursorKind KIND	= clang_getCursorKind(h_cur);
	const CXString KINDSPELL	= clang_getCursorKindSpelling(KIND);
	const CXString SPELL		= clang_getCursorSpelling(h_cur);

	(void)h_parent;
	(void)rdwr_data;

	puts(SPELL.data);
	puts(KINDSPELL.data);
	puts("");

	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);

	return CXChildVisit_Recurse;
}

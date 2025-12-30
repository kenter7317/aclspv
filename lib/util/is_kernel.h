#ifndef util_is_kernel_h
#define util_is_kernel_h


#include <clang-c/Index.h>
#include <ae2f/Keys.h>
#include <string.h>


ae2f_inline static unsigned util_is_kernel(const CXCursor h_fndecl) {
	if (clang_getCursorKind(h_fndecl) != CXCursor_FunctionDecl)
		return 0;

	CXSourceRange range = clang_getCursorExtent(h_fndecl);
	CXTranslationUnit tu = clang_Cursor_getTranslationUnit(h_fndecl);

	CXToken *tokens = NULL;
	unsigned nTokens = 0;
	unsigned found = 0;
	unsigned i = 0;

	clang_tokenize(tu, range, &tokens, &nTokens);

	for (; i < nTokens; ++i) {
		CXString spelling = clang_getTokenSpelling(tu, tokens[i]);
		if (strcmp(clang_getCString(spelling), "__kernel") == 0) {
			found = 1;
			clang_disposeString(spelling);
			break;
		}
		clang_disposeString(spelling);
	}

	clang_disposeTokens(tu, tokens, nTokens);
	return found;
}

#endif

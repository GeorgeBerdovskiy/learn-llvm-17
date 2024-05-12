// All the previous phases are glued together by the Calc.cpp
// driver, which we implement as follows: a parameter for the
// input expression is declared, LLVM is initialized, and all the
// phases from the previous sections are called:

// First we include the required header files
#include "CodeGen.hpp"
#include "Parser.hpp"
#include "Sema.hpp"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

// LLVM comes with its own system for declaring command-line
// options. You only need to declare a static variable for each
// option you need. In doing so, the option is registered with
// a global command line parser. The advantage of this approach
// is that each component can add command-line options when needed.

// We declare an option for the input expression
static llvm::cl::opt<std::string>
	Input(llvm::cl::Positional,
		  llvm::cl::desc("<input expression>"),
		  llvm::cl::init(""));

int main(int argc, const char **argv) {
	// Inside the `main()` function, the LLVM libraries are initialized
	// first. You need to call the `ParseCommandLineOptions()` function
	// to handle the options given on the command line. This also handles
	// the printing of help information. In the event of an error, this
	// method exists the application:

	llvm::InitLLVM X(argc, argv);
	llvm::cl::ParseCommandLineOptions(
		argc, argv, "calc - the expression compiler\n");
	
	// Next , we call the lexer and the parser. After the syntactical
	// analysis, we check whether any errors occured. If this is the case,
	// then we exit the compiler with a return code indicating a failure
	Lexer Lex(Input);
	Parser Parser(Lex);
	AST *Tree = Parser.parse();
	if (!Tree || Parser.hasError()) {
		llvm::errs() << "Syntax errors occured\n";
		return 1;
	}

	// We do the same if there was a semantic error
	Sema Semantic;
	if (Semantic.semantic(Tree)) {
		llvm::errs() << "Semantic errors occured\n";
		return 1;
	}

	// As the last step in the driver, the code generator is called
	CodeGen CodeGenerator;
	CodeGenerator.compile(Tree);
	return 0;
}
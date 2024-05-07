#include "Sema.hpp"
#include "llvm/ADT/StringSet.h"

// Note - Technically I shouldn't need to
// import this here... but it complains 🤷‍♂️
#include "llvm/Support/raw_ostream.h"

namespace {
class DeclCheck : public ASTVisitor {
    llvm::StringSet<> Scope;
    bool HasError;
    enum ErrorType { Twice, Not };
    void error(ErrorType ET, llvm::StringRef V) {
        llvm::errs() << "Variable " << V << " "
                     << (ET == Twice ? "already" : "not")
                     << " declared\n";
        HasError = true;
    }
public:
    DeclCheck() : HasError(false) {}
    bool hasError() { return HasError; }
};
}
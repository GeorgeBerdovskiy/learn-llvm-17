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

    virtual void visit(Factor &Node) override {
        if (Node.getKind() == Factor::Ident) {
            // If we are visiting a `Factor` node that
            // holds a variable name, we need to check that
            // the variable name is in the set
            if (Scope.find(Node.getVal()) == Scope.end())
                error(Not, Node.getVal());
        }
    }

    virtual void visit(BinaryOp &Node) override {
        // For a `BinaryOp` node, there is nothing to check
        // other than that both sides exist and are visited
        if (Node.getLeft())
            // Visit the left node
            Node.getLeft() -> accept(*this);
        else
            HasError = true;
        
        if (Node.getRight())
            // Visit the right node
            Node.getRight() -> accept(*this);
        else
            HasError = true;
    }

    virtual void visit(WithDecl &Node) override {
        for (auto I = Node.begin(), E = Node.end(); I != E; ++I) {
            if (!Scope.insert(*I).second)
                error(Twice, *I);
        }
        if (Node.getExpr())
            // Visit the expression
            Node.getExpr() -> accept(*this);
        else
            HasError = true;
    }
};
}

bool Sema::semantic(AST *Tree) {
    if (!Tree)
        return false;
    DeclCheck Check;
    Tree -> accept(Check);
    return Check.hasError();
}

// If the semantic analysis finishes w/o error, then we
// can generate the LLVM IR from the AST

#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;
class Expr;
class Factor;
class BinaryOp;
class WithDecl;

// The visitor pattern needs to know each class to visit
// Because each class also refers to the visitor, we declare
// all the classes at the top of the file.

class ASTVisitor {
public:
  virtual void visit(AST &){};
  virtual void visit(Expr &){};
  virtual void visit(Factor &) = 0;
  virtual void visit(BinaryOp &) = 0;
  virtual void visit(WithDecl &) = 0;
};

class AST {
public:
    virtual ~AST() {}
    virtual void accept(ASTVisitor &V) = 0;
};

class Expr : public AST {
public:
    Expr() {}
};

class Factor : public Expr {
public:
    // In more complex languages, you usually want to have
    // different AST classes, such as a NumberLiteral class
    // for numbers and a VariableAccess class for a reference
    // to a variable
    enum ValueKind { Ident, Number };
private:
    ValueKind Kind;
    llvm::StringRef Val;
public:
    Factor(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val) {}
    ValueKind getKind() { return Kind; }
    llvm::StringRef getVal() { return Val; }
    virtual void accept(ASTVisitor &V) override {
        V.visit(*this);
    }
};

class BinaryOp : public Expr {
public:
    // Note - the BinaryOp class doesn't make distinction
    // between multiplicative and additive operators. The
    // precedence of the operators is implicitly available
    // in the tree structure at this point.
    enum Operator { Plus, Minus, Mul, Div };
private:
    Expr *Left;
    Expr *Right;
    Operator Op;
public:
    BinaryOp(Operator Op, Expr *L, Expr *R)
        : Op(Op), Left(L), Right(R) {}
    Expr *getLeft() { return Left; }
    Expr *getRight() { return Right; }
    Operator getOperator() { return Op; }
    virtual void accept(ASTVisitor &V) override {
        V.visit(*this);
    }
};

class WithDecl : public AST {
    // See more about the `using` keyword
    // here - https://www.geeksforgeeks.org/using-keyword-in-cpp-stl/
    // In this case, we use it to create an alias
    using VarVector =
        llvm::SmallVector<llvm::StringRef, 8>;
    VarVector Vars;
    Expr *E;
public:
    WithDecl(llvm::SmallVector<llvm::StringRef, 8> Vars, Expr *E)
        : Vars(Vars), E(E) {}
    VarVector::const_iterator begin() {
        return Vars.begin();
    }
    VarVector::const_iterator end() {
        return Vars.end();
    }
    Expr *getExpr() { return E; }
    virtual void accept(ASTVisitor &V) override {
        V.visit(*this);
    }
};

#endif
#include "CodeGen.hpp"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm; // Namespace of the LLVM libraries is used for name lookups

namespace {
class ToIRVisitor : public ASTVisitor {
    // First, some private members are declared in the visitor.
    // Each compilation unit is represented in LLVM by the `Module`
    // class and the visitor has a pointer to the module called M.
    Module *M;

    // For easy IR generation, the Builder (of type IRBuilder<>) is used.
    IRBuilder<> Builder;

    // LLVM has a class hierarchy to represent types in IR. You
    // can look up the instances for basic types such as i32 from
    // the LLVM context. These basic types are used very often.
    // To avoid repeated lookups, we cache the needed type instances:
    // VoidTy, Int32Ty, PtrTy, and Int32Zero. 
    Type *VoidTy;
    Type *Int32Ty;
    PointerType *PtrTy;
    Constant *Int32Zero;

    // Current calculated value, which is updated through the
    // tree traversal
    Value *V;

    // Maps a variable name to the value returned from the `calc_read()` func
    StringMap<Value *> nameMap;
public:
    ToIRVisitor(Module *M) : M(M), Builder(M -> getContext()) {
        VoidTy = Type::getVoidTy(M -> getContext());
        Int32Ty = Type::getInt32Ty(M -> getContext());
        PtrTy = PointerType::getUnqual(M -> getContext());
        Int32Zero = ConstantInt::get(Int32Ty, 0, true);
    }

    void run(AST *Tree) {
        // For each function, a `FunctionType` instance must be created.
        // In C++ terminology, this is a function prototype.
        FunctionType *MainFty = FunctionType::get(
            Int32Ty, {Int32Ty, PtrTy}, false);

        // A function itself is defined with a Function instance
        Function *MainFn = Function::Create(
            MainFty, GlobalValue::ExternalLinkage, "main", M);
        
        // Then, we create the BB basic block with the entry label
        // and attach it to the IR builder
        BasicBlock *BB = BasicBlock::Create(M -> getContext(), "entry", MainFn);
        Builder.SetInsertPoint(BB);

        // With this preparation done, the tree traversal can begin
        Tree -> accept(*this);

        // After the tree traversal, the computed value is printed via
        // a call to the `call_write()` function
        // Again, a function prototype (an instance of FunctionType)
        // has to be created. The only parameter is the current value, V
        FunctionType *CalcWriteFnTy = FunctionType::get(VoidTy, {Int32Ty}, false);
        Function *CalcWriteFn = Function::Create(
            CalcWriteFnTy, GlobalValue::ExternalLinkage, "calc_write", M);
        Builder.CreateCall(CalcWriteFnTy, CalcWriteFn, {V});

        // The generation finished by returning 0 from the `main()` function
        Builder.CreateRet(Int32Zero);
    }
};
}
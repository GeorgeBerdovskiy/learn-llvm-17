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

	virtual void visit(WithDecl &Node) override {
		FunctionType *ReadFty = FunctionType::get(Int32Ty, {PtrTy}, false);
		Function *ReadFn = Function::Create(ReadFty, GlobalValue::ExternalLinkage, "calc_read", M);

		// Loop through the variable names
		for (auto I = Node.begin(), E = Node.end(); I != E; ++I) {
			// For each variable, a string with a variable name is created
			StringRef Var = *I;
			Constant *StrText = ConstantDataArray::getString(M -> getContext(), Var);
			GlobalVariable *Str = new GlobalVariable(
				*M, StrText -> getType(),
				/*isConstant=*/true,
				GlobalValue::PrivateLinkage,
				StrText, Twine(Var).concat(".str")
			);

			// Then the IR code to call the `calc_read()` function is created
			// The string created in the prev. step is passed as a parameter
			CallInst *Call = Builder.CreateCall(ReadFty, ReadFn, {Str});

			nameMap[Var] = Call;
		}

		Node.getExpr() -> accept(*this);
	}

	virtual void visit(Factor &Node) override {
		// A Factor node is either a variable name or a number
		if (Node.getKind() == Factor::Ident) {
			// For a variable name, the value is looked up in the
			// mapNames map. For a number, the value is converted to
			// an integer and turned into a constant value
			V = nameMap[Node.getVal()];
		} else {
			// For a number, the value is converted to an integer
			// and turned into a constant value
			int intval;
			Node.getVal().getAsInteger(10, intval);
			V = ConstantInt::get(Int32Ty, intval, true);
		}
	}

	virtual void visit(BinaryOp &Node) override {
		Node.getLeft() -> accept(*this);
		Value *Left = V;
		Node.getRight() -> accept(*this);
		Value *Right = V;
		switch (Node.getOperator()) {
		case BinaryOp::Plus:
			V = Builder.CreateNSWAdd(Left, Right);
			break;
		case BinaryOp::Minus:
			V = Builder.CreateNSWSub(Left, Right);
			break;
		case BinaryOp::Mul:
			V = Builder.CreateNSWMul(Left, Right);
			break;
		case BinaryOp::Div:
			V = Builder.CreateSDiv(Left, Right);
			break;
		}
	}
};
}

// The visitor class is now complete

void CodeGen::compile(AST *Tree) {
	// This method creates the global context
	// and the module, runs the tree traversal, and
	// dumps the generated IR to the console
	LLVMContext Ctx;
	Module *M = new Module("calc.expr", Ctx);
	ToIRVisitor ToIR(M);
	ToIR.run(Tree);
	M -> print(outs(), nullptr);
}

// We have now implemented the frontend of the compiler, from
// reading the source up to generating the IR. Of course, all
// these components must work together on user input, which
// is the task of the COMPILER DRIVER. We also need to implement
// the functions needed at runtime. The missing pieces are the
// DRIVER and the RUNTIME LIBRARY.
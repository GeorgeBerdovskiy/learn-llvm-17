#ifndef TINYLANG_BASIC_DIAGNOSTIC_H
#define TINYLANG_BASIC_DIAGNOSTIC_H
// #include "tinylang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <utility>

// MY OWN CODE
using namespace llvm;
// END MY OWN CODE

namespace tinylang {

// "Enumerations are used to define a set of named integer constants"
// For example, if we just use the single
// DIAG defined in the .def file, it would expand to...
// err_sym_declared,

namespace diag {
enum {
#define DIAG(ID, Level, Msg) ID,
#include "tinylang/Basic/Diagnostic.def"
};
} // namespace diag

// The DiagnosticsEngine class uses a SourceMgr instance to emit
// the messages via the report() method. Messages can have parameters.
// To implement this facility, the variadic-format support provided by
// LLVM is used. The message text and the severity level are retrieved
// with the help of the static method. As a bonus, the number of emitted
// error messages is also counted.

class DiagnosticsEngine {
    static const char *getDiagnosticText(unsigned DiagID);
    static SourceMgr::DiagKind
    getDiagnosticKind(unsigned DiagID);

    SourceMgr &SrcMgr;
    unsigned NumErrors;
public:
    DiagnosticsEngine(SourceMgr &SrcMgr)
        : SrcMgr(SrcMgr), NumErrors(0) {}
    unsigned numErrors() { return NumErrors; }

    template <typename... Args>
    void report(SMLoc Loc, unsigned DiagID,
                Args &&... Arguments) {
        std::string Msg =
            llvm::formatv(getDiagnosticText(DiagID),
                          std::forward<Args>(Arguments)...)
                .str();
        SourceMgr::DiagKind Kind = getDiagnosticKind(DiagID);
        SrcMgr.PrintMessage(Loc, Kind, Msg);
        NumErrors += (Kind == SourceMgr::DK_Error);
  }
};
} // namespace tinylang
#endif
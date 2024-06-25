#include "tinylang/Basic/Diagnostic.h"
using namespace tinylang;
namespace {
const char *DiagnosticText[] = {
#define DIAG(ID, Level, Msg) Msg,
#include "tinylang/Basic/Diagnostic.def"
};

SourceMgr::DiagKind DiagnosticKind[] = {
#define DIAG(ID, Level, Msg) SourceMgr::DK_##Level,
#include "tinylang/Basic/Diagnostic.def"
};
} // namespace

//Not surprisingly, both functions simply index the array
// to return the desired data

const char *
DiagnosticsEngine::getDiagnosticText(unsigned DiagID) {
  return DiagnosticText[DiagID];
}
SourceMgr::DiagKind
DiagnosticsEngine::getDiagnosticKind(unsigned DiagID) {
  return DiagnosticKind[DiagID];
}
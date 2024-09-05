#include <llvm-16/llvm/IR/Instruction.h>
#include <llvm-16/llvm/IR/Instructions.h>
#include <llvm-16/llvm/Support/Casting.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {

class FunctionInfoPass final : public PassInfoMixin<FunctionInfoPass> {
public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
    outs() << "CSCD70 Function Information Pass"
           << "\n";

    /// @todo(CSCD70) Please complete this method.
    for (auto iter = M.begin(); iter != M.end(); ++iter) {
      Function &F = *iter;
      if (F.hasName()) 
        outs() << F.getName()
               << "\n";

      outs() << F.arg_size();
      if (F.isVarArg()) outs() << "+*\n";
      else outs() << "\n";  

      int count_bb = 0;
      int count_inst = 0;
      int callsite_count = 0;

      for (BasicBlock &bb : F) {
        count_bb ++;
        for (Instruction &inst : bb) {
          count_inst ++;
          if (auto *callInst = dyn_cast<CallInst>(&inst)) {
            if (callInst->getCalledFunction() == &F) {
              callsite_count ++; 
            }
          } else if (auto *invokeInst = dyn_cast<InvokeInst>(&inst)) {
            if (invokeInst->getCalledFunction() == &F) {
              callsite_count ++;
            }
          }
        }
      } 

      outs() << callsite_count << "\n";
      outs() << count_bb << "\n";
      outs() << count_inst << "\n";
    } 
    return PreservedAnalyses::all();
  }
}; // class FunctionInfoPass

} // anonymous namespace

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
      .APIVersion = LLVM_PLUGIN_API_VERSION,
      .PluginName = "FunctionInfo",
      .PluginVersion = LLVM_VERSION_STRING,
      .RegisterPassBuilderCallbacks =
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (Name == "function-info") {
                    MPM.addPass(FunctionInfoPass());
                    return true;
                  }
                  return false;
                });
          } // RegisterPassBuilderCallbacks
  };        // struct PassPluginLibraryInfo
}

#pragma once // NOLINT(llvm-header-guard)

#include <llvm-16/llvm/IR/Instruction.h>
#include <llvm/IR/PassManager.h>

class AlgebraicIdentityPass final
    : public llvm::PassInfoMixin<AlgebraicIdentityPass> {
public:
std::vector<llvm::Instruction *> removableInst;

public:
  llvm::PreservedAnalyses run(llvm::Function &,
                              llvm::FunctionAnalysisManager &);
  bool runOnBasicBlock(llvm::BasicBlock&);
  bool runOnFunction(llvm::Function&);
}; // class AlgebraicIdentityPass

class StrengthReductionPass final
    : public llvm::PassInfoMixin<StrengthReductionPass> {
public:
std::vector<llvm::Instruction *> removableInst;

public:
  llvm::PreservedAnalyses run(llvm::Function &,
                              llvm::FunctionAnalysisManager &);
  bool runOnBasicBlock(llvm::BasicBlock&);
  bool runOnFunction(llvm::Function&);
}; // class StrengthReductionPass

class MultiInstOptPass final : public llvm::PassInfoMixin<MultiInstOptPass> {
public:
std::vector<llvm::Instruction *> removableInst;

public:
  llvm::PreservedAnalyses run(llvm::Function &,
                              llvm::FunctionAnalysisManager &);
  bool runOnBasicBlock(llvm::BasicBlock&);
  bool runOnFunction(llvm::Function&);
}; // class MultiInstOptPass

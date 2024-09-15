#include "LocalOpts.h"
#include <llvm-16/llvm/IR/Constants.h>
#include <llvm-16/llvm/IR/InstrTypes.h>
#include <llvm-16/llvm/IR/Instruction.h>
#include <llvm-16/llvm/IR/PassManager.h>
#include <llvm-16/llvm/Support/Casting.h>

using namespace llvm;
bool MultiInstOptPass::runOnBasicBlock(BasicBlock &B) {
  bool isChange = false;

  for (auto &Instr : B) {
    auto *BinOp = dyn_cast<BinaryOperator>(&Instr);
    if (!BinOp) continue;

    auto * Operand_0 = BinOp->getOperand(0);
    auto * Operand_1 = BinOp->getOperand(1);
    unsigned Operator = BinOp->getOpcode();

    if (auto *binOper = dyn_cast<BinaryOperator>(Operand_0)) {
      if ((Operator == Instruction::Add && binOper->getOpcode() == Instruction::Sub) 
       || (Operator == Instruction::Sub && binOper->getOpcode() == Instruction::Add)) {
        if (auto *ConstVal = dyn_cast<ConstantInt>(Operand_1))
          if (auto *SubConstVal = dyn_cast<ConstantInt>(binOper->getOperand(1))) 
            if (ConstVal->getSExtValue() == SubConstVal->getSExtValue()) {
              Instr.replaceAllUsesWith(binOper->getOperand(1));
              removableInst.push_back(&Instr);
              isChange = true;
            }
          
      } 
    } else if (auto *binOper = dyn_cast<BinaryOperator>(Operand_1)) {

    }
  }

  return isChange;
}

bool MultiInstOptPass::runOnFunction(Function &F) {
  bool isChange = false;

  for (auto &BB : F) {
    if (runOnBasicBlock(BB)) isChange = true;
  }

  if (isChange) {
    for (auto *Inst : removableInst) {
      Inst->eraseFromParent();
    }
  }
  return isChange;
}


PreservedAnalyses MultiInstOptPass::run([[maybe_unused]] Function &F,
                                        FunctionAnalysisManager &) {

  /// @todo(CSCD70) Please complete this method.
  if (runOnFunction(F)) {
    return PreservedAnalyses::none();
  }
  return PreservedAnalyses::all();
}

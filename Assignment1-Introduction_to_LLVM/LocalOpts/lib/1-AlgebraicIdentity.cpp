#include "LocalOpts.h"
#include <cstdio>
#include <llvm-16/llvm/IR/Constants.h>
#include <llvm-16/llvm/IR/Instruction.h>
#include <llvm-16/llvm/IR/PassManager.h>
#include <llvm-16/llvm/Support/Casting.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/Support/Casting.h>

using namespace llvm;

PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &) {
  /// @todo(CSCD70) Please complete this method.
  if (runOnFunction(F)) {
    return PreservedAnalyses::none();
  }
  return PreservedAnalyses::all();
}

bool AlgebraicIdentityPass::runOnBasicBlock(BasicBlock &B) {
  bool isChange = false;

  for (auto &inst : B) {
    if (auto *BinOp = dyn_cast<BinaryOperator>(&inst)) {
      Value *oprand0 = BinOp->getOperand(0);
      Value *oprand1 = BinOp->getOperand(1);
      if (!BinOp->getType()->isIntegerTy())
        continue;
      // auto *Const_0 = isa<ConstantInt>(oprand0)
      //                       ? dyn_cast<ConstantInt>(oprand0)
      //                       : oprand0;

      // auto *Const_1 = isa<ConstantInt>(oprand1)
      //                       ? dyn_cast<ConstantInt>(oprand1)
      //                       : oprand1;

      if (BinOp->getOpcode() == Instruction::Mul) {
        if (auto *ConstInt = dyn_cast<ConstantInt>(oprand1)) {
          if (ConstInt->getSExtValue() == 1) {
            inst.replaceAllUsesWith(oprand0);
            removableInst.push_back(&inst);
            isChange = true;
          }
        } else if (auto *ConstInt = dyn_cast<ConstantInt>(oprand0)) {
          if (ConstInt->getSExtValue() == 1) {
            inst.replaceAllUsesWith(oprand1);
            removableInst.push_back(&inst);
            isChange = true;
          }
        }
      } else if (BinOp->getOpcode() == Instruction::Add) {
        if (auto *ConstInt = dyn_cast<ConstantInt>(oprand1)) {
          if (ConstInt->getSExtValue() == 0) {
            inst.replaceAllUsesWith(oprand0);
            removableInst.push_back(&inst);
            isChange = true;
          }
        } else if (auto *ConstInt = dyn_cast<ConstantInt>(oprand0)) {
          if (ConstInt->getSExtValue() == 0) {
            inst.replaceAllUsesWith(oprand1);
            removableInst.push_back(&inst);
            isChange = true;
          }
        }
      } else if (BinOp->getOpcode() == Instruction::Sub) {
        if (auto *ConstInt = dyn_cast<ConstantInt>(oprand1)) {
          if (ConstInt->getSExtValue() == 0) {
            inst.replaceAllUsesWith(oprand0);
            removableInst.push_back(&inst);
            isChange = true;
          }
        } 
      } else if (BinOp->getOpcode() == Instruction::SDiv) {
        if (auto *ConstInt = dyn_cast<ConstantInt>(oprand1)) {
          if (ConstInt->getSExtValue() == 1) {
            inst.replaceAllUsesWith(oprand0);
            removableInst.push_back(&inst);
            isChange = true;
          }
        } 
      }
    }
  }
  return isChange;
}

bool AlgebraicIdentityPass::runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  if (Transformed) {
    for (Instruction * inst : removableInst) {
      inst->eraseFromParent();
    }
  }
  return Transformed;
}
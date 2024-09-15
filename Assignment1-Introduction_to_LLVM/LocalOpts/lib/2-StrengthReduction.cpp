#include "LocalOpts.h"
#include <cmath>
#include <llvm-16/llvm/IR/InstrTypes.h>
#include <llvm-16/llvm/IR/Instruction.h>
#include <llvm-16/llvm/Support/MathExtras.h>
#include <llvm-16/llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

using namespace llvm;

bool StrengthReductionPass::runOnBasicBlock(BasicBlock &B) {
  bool isChange = false;

  for (auto instr = B.begin(); instr != B.end(); ++instr) {
    auto *BinOp = dyn_cast<BinaryOperator>(instr);
    if (!BinOp)
      continue;

    unsigned BinOptor = BinOp->getOpcode();
    if (BinOptor == Instruction::Mul && BinOp->getType()->isIntegerTy()) {

      auto *Operand_0 = instr->getOperand(0);
      auto *Operand_1 = instr->getOperand(1);

      if (auto *ConstOp = dyn_cast<ConstantInt>(Operand_1)) {
        auto sign = ConstOp->getSExtValue() >= 0 ? 1 : -1;
        auto abs_val = std::abs(ConstOp->getSExtValue());
        if ((abs_val & (abs_val - 1)) == 0) {
          int shiftNum = (int)std::log2(abs_val);
          IRBuilder<> builder(BinOp);
          if (sign >= 0) {
            Instruction *newInstr = BinaryOperator::CreateShl(
                Operand_0, ConstantInt::get(BinOp->getType(), shiftNum));
            ReplaceInstWithInst(&B, instr, newInstr);
          } else {
            Instruction *newInstr = BinaryOperator::CreateNeg(builder.CreateShl(
                Operand_0, ConstantInt::get(BinOp->getType(), shiftNum)));
            ReplaceInstWithInst(&B, instr, newInstr);
          }
        }
      } // 我们可以大胆假设这玩意一定在第二个位置
    } else if (BinOptor == Instruction::SDiv && BinOp->getType()->isIntegerTy()) {
      
    }
  }

  return isChange;
}

bool StrengthReductionPass::runOnFunction(Function &F) {
  bool isChange = false;

  for (auto &BB : F) {
    if (runOnBasicBlock(BB))
      isChange = true;
  }

  if (isChange) {
    for (Instruction *inst : removableInst) {
      inst->eraseFromParent();
    }
  }
  return isChange;
}

PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &) {

  /// @todo(CSCD70) Please complete this method.
  if (runOnFunction(F))
    return PreservedAnalyses::none();
  return PreservedAnalyses::all();
}

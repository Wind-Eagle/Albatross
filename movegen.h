#ifndef MOVEGEN_H_
#define MOVEGEN_H_

#include "types.h"

namespace core {
  enum class PromotionPolicy : int8_t {
    kAll,
    kPromotion,
    kNone
  };
  enum class MoveGenType : bool {
    kSimple,
    kCapture
  };
}  // namespace core

#endif  // MOVEGEN_H_

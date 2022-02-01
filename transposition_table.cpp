#include "transposition_table.h"

namespace search {
inline constexpr int32_t TranspositionTable::Data::GetWeight(const uint8_t epoch) const {
  if (!IsValid()) {
    return std::numeric_limits<int32_t>::min();
  }
  const uint8_t age = epoch - epoch_;
  int32_t result = 4 * GetDepth() - age;
  if ((flags_ & 7) == kExact) {
    result += 6;
  }
  if (move_.type_ == core::MoveType::kNull) {
    result -= 4;
  }
  if (flags_ & kPV) {
    result += 2;
  }
  return result;
}
void TranspositionTable::AddData(core::hash_t hash, Data data) {
  const int idx = hash & (size_ - 1);
  const uint8_t epoch = epoch_;
  Entry& entry = table_[idx];
  data.epoch_ = epoch;
  if (entry.data_.load(std::memory_order_relaxed).GetWeight(epoch) > data.GetWeight(epoch)) {
    return;
  }
  hash ^= data.GetAs64();
  entry.hash_.store(hash, std::memory_order_relaxed);
  entry.data_.store(data, std::memory_order_relaxed);
}
TranspositionTable::Data TranspositionTable::GetData(core::hash_t hash) const {
  const int idx = hash & (size_ - 1);
  Entry& entry = table_[idx];
  const Data data = entry.data_.load(std::memory_order_relaxed);
  const core::hash_t data_hash = entry.hash_.load(std::memory_order_relaxed) ^ data.GetAs64();
  if (data_hash != hash) {
    return Data::GetEmpty();
  }
  return data;
}

}  // namespace search
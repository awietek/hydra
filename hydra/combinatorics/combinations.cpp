#include "combinations.h"
#include <lila/utils/logger.h>

namespace hydra {

template <class bit_t>
Combinations<bit_t>::Combinations(int n, int k)
  : n_(n), k_(k),
    size_(combinatorics::binomial(n, k))
{  
  if (k>n)
    lila::Log.err("Error constructing Combinations: k>n");
  else if (k<0)
    lila::Log.err("Error constructing Combinations: k<0");
  else if (n<0)
    lila::Log.err("Error constructing Combinations: n<0");
  else
    {
      bit_t state = ((bit_t)1 << k) - 1;
      begin_ = CombinationsIterator<bit_t>(state, (idx_t)0);
      end_ = CombinationsIterator<bit_t>(state, (idx_t)combinatorics::binomial(n, k));
    }
}

template <class bit_t>
CombinationsIterator<bit_t>::CombinationsIterator(bit_t state, idx_t idx)
  : current_(state) , idx_(idx) {}

template class Combinations<uint16>;
template class Combinations<uint32>;
template class Combinations<uint64>;

template class CombinationsIterator<uint16>;
template class CombinationsIterator<uint32>;
template class CombinationsIterator<uint64>;

} // namespace hydra

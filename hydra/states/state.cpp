#include "state.h"
#include <hydra/states/product_state.h>
#include <hydra/states/random_state.h>

namespace hydra {

template <typename coeff_t>
State<coeff_t>::State(Block const &block)
    : block_(block), vector_(block.size(), arma::fill::zeros) {}

template <typename coeff_t>
State<coeff_t>::State(Block const &block, arma::Col<coeff_t> const &vector)
    : block_(block), vector_(vector) {
  if (block.size() != (idx_t)vector.size()) {
    Log.err("Error creating State: block dimension does not match vector "
            "dimension");
  }
}

template <typename coeff_t>
State<coeff_t>::State(Block const &block, ProductState const &pstate)
    : block_(block), vector_(block.size(), arma::fill::zeros) {
  fill(pstate, *this);
}

template <typename coeff_t>
State<coeff_t>::State(Block const &block, RandomState const &rstate)
    : block_(block), vector_(block.size(), arma::fill::zeros) {
  fill(rstate, *this);
}

template class State<double>;
template class State<complex>;

StateCplx to_cplx(StateReal const &state) {
  return StateCplx(state.block(),
                   arma::conv_to<arma::cx_vec>::from(state.vector()));
}

} // namespace hydra

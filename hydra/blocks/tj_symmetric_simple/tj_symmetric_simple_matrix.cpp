#include "tj_symmetric_simple_matrix.h"

#include <hydra/blocks/tj_symmetric_simple/terms/tj_symmetric_simple_exchange.h>
#include <hydra/blocks/tj_symmetric_simple/terms/tj_symmetric_simple_hopping.h>
#include <hydra/blocks/tj_symmetric_simple/terms/tj_symmetric_simple_ising.h>

#include <hydra/blocks/utils/block_utils.h>

namespace hydra {

template <class bit_t, class GroupAction>
lila::Matrix<double>
MatrixReal(BondList const &bonds, Couplings const &couplings,
           tJSymmetricSimple<bit_t, GroupAction> const &block_in,
           tJSymmetricSimple<bit_t, GroupAction> const &block_out) {
  using namespace terms::tj_symmetric_simple;

  assert(block_in == block_out); // only temporary

  utils::check_symmetric_operator_real(bonds, couplings, block_in.irrep(),
                                       block_out.irrep(),
                                       "construct real tJSymmetricSimple matrix");

  idx_t dim = block_in.size();
  auto mat = lila::Zeros<double>(dim, dim);
  auto fill = [&mat](idx_t idx_out, idx_t idx_in, double val) {
    mat(idx_out, idx_in) += val;
  };

  do_hopping_symmetric<bit_t, double>(bonds, couplings, block_in, fill);
  do_ising_symmetric<bit_t>(bonds, couplings, block_in, fill);
  do_exchange_symmetric<bit_t, double>(bonds, couplings, block_in, fill);

  return mat;
}

template lila::Matrix<double> MatrixReal<uint16_t, PermutationGroupAction>(
    BondList const &bonds, Couplings const &couplings,
    tJSymmetricSimple<uint16_t, PermutationGroupAction> const &block_in,
    tJSymmetricSimple<uint16_t, PermutationGroupAction> const &block_out);
template lila::Matrix<double> MatrixReal<uint32_t, PermutationGroupAction>(
    BondList const &bonds, Couplings const &couplings,
    tJSymmetricSimple<uint32_t, PermutationGroupAction> const &block_in,
    tJSymmetricSimple<uint32_t, PermutationGroupAction> const &block_out);
template lila::Matrix<double> MatrixReal<uint64_t, PermutationGroupAction>(
    BondList const &bonds, Couplings const &couplings,
    tJSymmetricSimple<uint64_t, PermutationGroupAction> const &block_in,
    tJSymmetricSimple<uint64_t, PermutationGroupAction> const &block_out);

template <class bit_t, class GroupAction>
lila::Matrix<complex>
MatrixCplx(BondList const &bonds, Couplings const &couplings,
           tJSymmetricSimple<bit_t, GroupAction> const &block_in,
           tJSymmetricSimple<bit_t, GroupAction> const &block_out) {
  using namespace terms::tj_symmetric_simple;

  assert(block_in == block_out); // only temporary

  idx_t dim = block_in.size();
  auto mat = lila::Zeros<complex>(dim, dim);
  auto fill = [&mat](idx_t idx_out, idx_t idx_in, complex val) {
    mat(idx_out, idx_in) += val;
  };

  do_hopping_symmetric<bit_t, complex>(bonds, couplings, block_in, fill);
  do_ising_symmetric<bit_t>(bonds, couplings, block_in, fill);
  do_exchange_symmetric<bit_t, complex>(bonds, couplings, block_in, fill);

  return mat;
}

template lila::Matrix<complex> MatrixCplx<uint16_t, PermutationGroupAction>(
    BondList const &bonds, Couplings const &couplings,
    tJSymmetricSimple<uint16_t, PermutationGroupAction> const &block_in,
    tJSymmetricSimple<uint16_t, PermutationGroupAction> const &block_out);
template lila::Matrix<complex> MatrixCplx<uint32_t, PermutationGroupAction>(
    BondList const &bonds, Couplings const &couplings,
    tJSymmetricSimple<uint32_t, PermutationGroupAction> const &block_in,
    tJSymmetricSimple<uint32_t, PermutationGroupAction> const &block_out);
template lila::Matrix<complex> MatrixCplx<uint64_t, PermutationGroupAction>(
    BondList const &bonds, Couplings const &couplings,
    tJSymmetricSimple<uint64_t, PermutationGroupAction> const &block_in,
    tJSymmetricSimple<uint64_t, PermutationGroupAction> const &block_out);

} // namespace hydra

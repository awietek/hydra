#pragma once

#include <hydra/indexing/spinhalf_mpi/spinhalf_mpi_indexing_sz.h>

#include <hydra/bitops/bitops.h>
#include <hydra/blocks/spinhalf_mpi/spinhalf_mpi.h>
#include <hydra/combinatorics/combinations.h>
#include <hydra/common.h>
#include <hydra/mpi/logger_mpi.h>
#include <hydra/operators/bondlist.h>
#include <hydra/operators/couplings.h>

namespace hydra::terms {

template <class bit_t, class coeff_t>
void spinhalf_mpi_sz(BondList const &bonds, Couplings const &couplings,
                     indexing::SpinhalfMPIIndexingSz<bit_t> const &indexing,
                     lila::Vector<coeff_t> const &vec_in,
                     lila::Vector<coeff_t> &vec_out) {

  auto clean_bonds = utils::clean_bondlist(bonds, couplings, {"SZ"}, 1);

  for (auto bond : clean_bonds) {

    int s = bond[0];
    bit_t mask = ((bit_t)1 << s);

    // Set values for same/diff
    std::string cpl = bond.coupling();
    coeff_t H = utils::get_coupling<coeff_t>(couplings, cpl);
    coeff_t val_up = H / 2.;
    coeff_t val_dn = -H / 2.;

    int n_postfix_bits = indexing.n_postfix_bits();

    idx_t idx = 0;
    for (auto prefix : indexing.prefixes()) {
      auto postfixes = indexing.postfixes(prefix);

      // site in postfixes
      if (s < n_postfix_bits) {
        for (auto postfix : postfixes) {
          coeff_t val = ((postfix & mask) ? val_up : val_dn);
          vec_out(idx) += val * vec_in(idx);
          ++idx;
        }

      }
      // site in prefixes
      else {
        bit_t prefix_shifted = (prefix << n_postfix_bits);
        coeff_t val = (prefix_shifted & mask) ? val_up : val_dn;

        for (idx_t start = idx; idx < start + (idx_t)postfixes.size(); ++idx) {
          vec_out(idx) += val * vec_in(idx);
        }
      }
    }
  }
}
} // namespace hydra::terms

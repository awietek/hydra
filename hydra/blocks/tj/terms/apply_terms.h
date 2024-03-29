#pragma once

#include <hydra/common.h>

#include <hydra/blocks/tj/compile.h>
#include <hydra/blocks/tj/terms/apply_exchange.h>
#include <hydra/blocks/tj/terms/apply_hopping.h>
#include <hydra/blocks/tj/terms/apply_ising.h>
#include <hydra/blocks/tj/terms/apply_number.h>
#include <hydra/blocks/tj/terms/apply_raise_lower.h>

#include <hydra/utils/print_macro.h>

namespace hydra::tj {

template <typename bit_t, typename coeff_t, bool symmetric, class BasisIn,
          class BasisOut, class Fill>
void apply_terms(BondList const &bonds, BasisIn const &basis_in,
                 BasisOut const &basis_out, Fill &fill) {

  BondList bonds_compiled = tj::compile(bonds);
  for (auto bond : bonds_compiled) {
    std::string type = bond.type();
    if ((type == "ISING") || (type == "TJISING")) {
      tj::apply_ising<bit_t, coeff_t, symmetric>(bond, basis_in, fill);
    } else if ((type == "NUMBERUP") || (type == "NUMBERDN")) {
      tj::apply_number<bit_t, coeff_t, symmetric>(bond, basis_in, fill);
    } else if (type == "EXCHANGE") {
      tj::apply_exchange<bit_t, coeff_t, symmetric>(bond, basis_in, fill);
    } else if ((type == "HOPUP") || (type == "HOPDN")) {
      tj::apply_hopping<bit_t, coeff_t, symmetric>(bond, basis_in, fill);
    } else if ((type == "CDAGUP") || (type == "CUP") || (type == "CDAGDN") ||
               (type == "CDN")) {
      tj::apply_raise_lower<bit_t, coeff_t, symmetric>(bond, basis_in,
                                                       basis_out, fill);
    }
  }
}

} // namespace hydra::tj

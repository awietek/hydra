#include "eigvals_lanczos.h"
#include <hydra/algebra/apply.h>
#include <hydra/algorithms/lanczos/lanczos.h>
#include <hydra/algorithms/lanczos/lanczos_convergence.h>

#include <hydra/states/random_state.h>
#include <hydra/utils/timing.h>

namespace hydra {

eigvals_lanczos_result_t
eigvals_lanczos(BondList const &bonds, block_variant_t const &block,
                int64_t neigvals, double precision, int64_t max_iterations,
                bool force_complex, double deflation_tol,
                int64_t random_seed) try {

  if (neigvals < 1) {
    throw(std::invalid_argument("Argument \"neigvals\" needs to be >= 1"));
  }
  if (!bonds.ishermitian()) {
    throw(std::invalid_argument("Input BondList is not hermitian"));
  }

  bool cplx = bonds.iscomplex() || iscomplex(block) || force_complex;
  State state0(block, !cplx);
  fill(state0, RandomState(random_seed));

  auto converged = [neigvals, precision](Tmatrix const &tmat) -> bool {
    return lanczos::converged_eigenvalues(tmat, neigvals, precision);
  };

  lanczos::lanczos_result_t r;
  int64_t iter = 1;
  // Setup complex Lanczos run
  if (cplx) {
    arma::cx_vec v0 = state0.vectorC(0, false);
    auto mult = [&iter, &bonds, &block](arma::cx_vec const &v,
                                        arma::cx_vec &w) {
      auto ta = rightnow();
      apply(bonds, block, v, block, w);
      Log(1, "Lanczos iteration {}", iter);
      timing(ta, rightnow(), "MVM", 1);
      ++iter;
    };
    auto dot = [](arma::cx_vec const &v, arma::cx_vec const &w) {
      return arma::cdot(v, w);
    };
    auto operation = [](arma::cx_vec const &) {};
    r = lanczos::lanczos(mult, dot, converged, operation, v0, max_iterations,
                         deflation_tol);

    // Setup real Lanczos run
  } else {
    arma::vec v0 = state0.vector(0, false);
    auto mult = [&iter, &bonds, &block](arma::vec const &v, arma::vec &w) {
      auto ta = rightnow();
      apply(bonds, block, v, block, w);
      Log(1, "Lanczos iteration {}", iter);
      timing(ta, rightnow(), "MVM", 1);
      ++iter;
    };
    auto dot = [](arma::vec const &v, arma::vec const &w) {
      return arma::dot(v, w);
    };
    auto operation = [](arma::vec const &) {};
    r = lanczos::lanczos(mult, dot, converged, operation, v0, max_iterations,
                         deflation_tol);
  }

  return {r.alphas, r.betas, r.eigenvalues, r.niterations, r.criterion};
} catch (...) {
  HydraRethrow("Error performing eigenvalue Lanczos algorithm");
  return eigvals_lanczos_result_t();
}
} // namespace hydra
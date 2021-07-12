#pragma once

#include <hydra/linalg/lanczos/lanczos_convergence.h>
#include <hydra/linalg/lanczos/lanczos_generic.h>
#include <hydra/linalg/lanczos/tmatrix.h>

#include <hydra/mpi/stable_dot.h>
#include <hydra/mpi/timing_mpi.h>

#include <lila/all.h>

namespace hydra {

template <class coeff_t, class Block>
Tmatrix<coeff_t>
LanczosEigenvalues(BondList const &bonds, Couplings const &couplings,
                   Block const &block, lila::Vector<coeff_t> &v0,
                   int num_eigenvalue = 0, double precision = 1e-12,
                   int max_iterations = 1000, double deflation_tol = 1e-7) {

  using namespace lila;

  // MPI Lanczos
  if constexpr (is_mpi_block<Block>) {

    int iter = 0;
    auto mult = [&iter, &bonds, &couplings, &block](Vector<coeff_t> const &v,
                                                    Vector<coeff_t> &w) {
      auto ta = rightnow_mpi();
      Apply(bonds, couplings, block, v, block, w);
      timing_mpi(ta, rightnow_mpi(), "MVM", 1);
      ++iter;
    };

    auto stable_dot = [](Vector<coeff_t> const &v,
                         Vector<coeff_t> const &w) -> coeff_t {
      return mpi::StableDot(v, w);
    };

    auto converged = [num_eigenvalue,
                      precision](Tmatrix<coeff_t> const &tmat) -> bool {
      return ConvergedEigenvalues(tmat, num_eigenvalue, precision);
    };

    auto t0 = rightnow_mpi();
    [[maybe_unused]] auto [tmat, vectors] =
        LanczosGeneric(mult, v0, stable_dot, converged, max_iterations,
                       Matrix<coeff_t>(), deflation_tol);
    timing_mpi(t0, rightnow_mpi(), "Lanczos time", 1);
    return tmat;
  }

  // Serial Lanczos
  else {

    int iter = 0;
    auto mult = [&iter, &bonds, &couplings, &block](Vector<coeff_t> const &v,
                                                    Vector<coeff_t> &w) {
      auto ta = rightnow();
      Apply(bonds, couplings, block, v, block, w);
      timing(ta, rightnow(), "MVM", 1);
      ++iter;
    };

    auto dot = [](Vector<coeff_t> const &v,
                  Vector<coeff_t> const &w) -> coeff_t {
      return lila::Dot(v, w);
    };

    auto converged = [num_eigenvalue,
                      precision](Tmatrix<coeff_t> const &tmat) -> bool {
      return ConvergedEigenvalues(tmat, num_eigenvalue, precision);
    };

    auto t0 = rightnow();
    [[maybe_unused]] auto [tmat, vectors] =
        LanczosGeneric(mult, v0, dot, converged, max_iterations,
                       Matrix<coeff_t>(), deflation_tol);
    timing(t0, rightnow(), "Lanczos time", 1);
    return tmat;
  }
}

template <class Block>
Tmatrix<double>
LanczosEigenvaluesReal(BondList const &bonds, Couplings const &couplings,
                       Block const &block, int num_eigenvalue = 0,
                       double precision = 1e-12, int seed = 42,
                       int max_iterations = 1000, double deflation_tol = 1e-7) {

  using namespace lila;

  // use different seeds for different MPI processes
  if constexpr (is_mpi_block<Block>) {
    int mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    seed += 0x01000193 * mpi_rank;
  }

  // Create random starting vector with normal distributed entries
  normal_dist_t<double> dist(0., 1.);
  normal_gen_t<double> gen(dist, seed);
  auto v0 = Random(block.size(), gen);

  // Run Lanczos algorithm
  return LanczosEigenvalues(bonds, couplings, block, v0, num_eigenvalue,
                            precision, max_iterations, deflation_tol);
}

template <class Block>
Tmatrix<complex>
LanczosEigenvaluesCplx(BondList const &bonds, Couplings const &couplings,
                       Block const &block, int num_eigenvalue = 0,
                       double precision = 1e-12, int seed = 42,
                       int max_iterations = 1000, double deflation_tol = 1e-7) {

  using namespace lila;

  // use different seeds for different MPI processes
  if constexpr (is_mpi_block<Block>) {
    int mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    seed += 0x01000193 * mpi_rank;
  }

  // Create random starting vector with normal distributed entries
  normal_dist_t<complex> dist(0., 1.);
  normal_gen_t<complex> gen(dist, seed);
  auto v0 = Random(block.size(), gen);

  // Run Lanczos algorithm
  return LanczosEigenvalues(bonds, couplings, block, v0, num_eigenvalue,
                            precision, max_iterations, deflation_tol);
}

} // namespace hydra
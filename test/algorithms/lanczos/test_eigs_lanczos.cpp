#include "../../catch.hpp"

#include <iostream>

#include "../../blocks/electron/testcases_electron.h"
#include <hydra/algorithms/lanczos/eigs_lanczos.h>
#include <hydra/algebra/apply.h>
#include <hydra/algebra/algebra.h>

#include <hydra/blocks/electron/electron_matrix.h>
#include <hydra/utils/close.h>
#include <hydra/utils/print_macro.h>

using namespace hydra;

TEST_CASE("eigs_lanczos", "[lanczos]") {
  using namespace hydra::testcases::electron;

  BondList bondlist;

  int n_sites = 6;
  int max_num_eigenvalue = 2;

  printf("lanczos_eigenvector_real test ...\n");
  bondlist = freefermion_alltoall(n_sites);
  bondlist["U"] = 5.0;

  for (int nup = 2; nup <= n_sites / 2; ++nup)
    for (int ndn = 2; ndn <= n_sites / 2; ++ndn) {

      // Compute exact evals
      auto block = Electron(n_sites, nup, ndn);
      auto H = matrix(bondlist, block, block);
      arma::vec evals_mat;
      arma::eig_sym(evals_mat, H);

      // Compute evec with Lanczos
      auto res = eigs_lanczos(bondlist, block, max_num_eigenvalue);

      // Compute energy of eigenvector
      for (int num_eigenvalue = 0; num_eigenvalue < max_num_eigenvalue;
           ++num_eigenvalue) {
        auto v = res.eigenvectors.col(num_eigenvalue);
	// HydraPrint(norm(v));
	REQUIRE(close(norm(v), 1.0));

        auto Hv = v;
        apply(bondlist, v, Hv);
        double e = dot(v, Hv);

	// HydraPrint(num_eigenvalue);
	// HydraPrint(e);
	// HydraPrint(evals_mat(num_eigenvalue));
        REQUIRE(close(real(e), evals_mat(num_eigenvalue)));
        REQUIRE(close(imag(e), 0.0));
      }
    }
  printf("Done.\n");

  printf("lanczos_eigenvector_cplx test ...\n");
  bondlist = freefermion_alltoall_complex_updn(n_sites);

  for (int nup = 2; nup <= n_sites / 3; ++nup)
    for (int ndn = 2; ndn <= n_sites / 3; ++ndn) {

      // Create block and matrix for comparison
      auto block = Electron(n_sites, nup, ndn);
      auto H = matrixC(bondlist, block, block);
      arma::vec evals_mat;
      arma::eig_sym(evals_mat, H);

      // Compute evec with Lanczos
      try {
      auto res =
          eigs_lanczos(bondlist, block, max_num_eigenvalue, 1e-12, 1000, true);

      // Compute energy of eigenvector
      for (int num_eigenvalue = 0; num_eigenvalue < max_num_eigenvalue;
           ++num_eigenvalue) {
        auto v = res.eigenvectors.col(num_eigenvalue);
        REQUIRE(close(norm(v), 1.0));
        auto Hv = v;
        apply(bondlist, v, Hv);
        auto e = dotC(v, Hv);
        REQUIRE(close(real(e), evals_mat(num_eigenvalue)));
        REQUIRE(close(imag(e), 0.0));

      }
      } catch(std::exception const& e){
	traceback(e);
      }
    }
  printf("Done.\n");
}
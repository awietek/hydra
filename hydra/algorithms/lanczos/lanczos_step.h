#pragma once

#include "extern/armadillo/armadillo"

#include <hydra/algorithms/lanczos/tmatrix.h>
#include <hydra/algorithms/gram_schmidt/orthogonalize.h>
#include <hydra/common.h>
#include <hydra/utils/logger.h>
#include <hydra/utils/print_macro.h>

namespace hydra {

template <typename coeff_t, class multiply_f, class dot_f>

inline void lanczos_step(arma::Col<coeff_t> &v0, arma::Col<coeff_t> &v1,
                         arma::Col<coeff_t> &w, double &alpha, double &beta,
                         multiply_f mult, dot_f dot) {

  auto norm = [&dot](arma::Col<coeff_t> const &v) {
    return std::sqrt(hydra::real(dot(v, v)));
  };

  mult(v1, w); // MVM
  alpha = hydra::real(dot(v1, w));
  w -= alpha * v1;
  w -= beta * v0;
  v0 = v1;
  v1 = w;
  beta = norm(v1);
}

  template <typename coeff_t, class multiply_f, class dot_f>
inline void lanczos_step_ortho(arma::Col<coeff_t> &v0, arma::Col<coeff_t> &v1,
			       arma::Col<coeff_t> &w, double &alpha, double &beta,
			       multiply_f mult, dot_f dot, arma::Mat<coeff_t> const& V,
			       int64_t iteration) {

  auto norm = [&dot](arma::Col<coeff_t> const &v) {
    return std::sqrt(hydra::real(dot(v, v)));
  };

  mult(v1, w); // MVM
  alpha = hydra::real(dot(v1, w));
  orthogonalize_inplace(w, V, iteration);
  v0 = v1;
  v1 = w;
  beta = norm(v1);
}

  

} // namespace hydra

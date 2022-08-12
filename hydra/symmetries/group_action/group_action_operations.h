#pragma once
#include <lila/external/gsl/span>

#include <hydra/combinatorics/combinations.h>
#include <hydra/symmetries/fermi_sign.h>
#include <hydra/symmetries/representation.h>
#include <vector>

namespace hydra::symmetries {

// Computes stabilizing symmetries of "bits" when group is applied
template <typename bit_t, class GroupAction>
inline std::vector<int> stabilizer_symmetries(bit_t bits,
                                              GroupAction const &group) {
  std::vector<int> stable_syms;
  for (int sym = 0; sym < group.n_symmetries(); ++sym)
    if (group.apply(sym, bits) == bits)
      stable_syms.push_back(sym);
  return stable_syms;
}

// Computes the representative (smallest integer value) of "state"
// in orbit given by group_action
template <typename bit_t, class GroupAction>
inline bit_t representative(bit_t state, GroupAction const &group_action) {
  if (group_action.n_symmetries() == 0)
    return state;

  bit_t rep = std::numeric_limits<bit_t>::max();
  for (int sym = 0; sym < group_action.n_symmetries(); ++sym) {
    bit_t tstate = group_action.apply(sym, state);
    if (tstate < rep) {
      rep = tstate;
    }
  }
  return rep;
}

// Computes the representative using only a specified subset of symmetries
template <typename bit_t, class GroupAction>
inline bit_t representative_subset(bit_t state, GroupAction const &group_action,
                                   gsl::span<int const> syms) {
  if (syms.size() == 0)
    return state;
  bit_t rep = std::numeric_limits<bit_t>::max();
  for (int sym : syms) {
    assert(sym < group_action.n_symmetries());
    bit_t tstate = group_action.apply(sym, state);
    if (tstate < rep) {
      rep = tstate;
    }
  }
  return rep;
}

// Computes the representative of state AND the symmetry that yields it
template <typename bit_t, class GroupAction>
inline std::pair<bit_t, int>
representative_sym(bit_t state, GroupAction const &group_action) {
  if (group_action.n_symmetries() == 0)
    return {state, 0};
  bit_t rep = std::numeric_limits<bit_t>::max();
  int rep_sym = 0;
  for (int sym = 0; sym < group_action.n_symmetries(); ++sym) {
    bit_t tstate = group_action.apply(sym, state);
    if (tstate < rep) {
      rep = tstate;
      rep_sym = sym;
    }
  }
  return {rep, rep_sym};
}

// Computes the representative/symmetry using only a specified subset of
// symmetries
template <typename bit_t, class GroupAction>
inline std::pair<bit_t, int>
representative_sym_subset(bit_t state, GroupAction const &group_action,
                          gsl::span<int const> syms) {
  if (syms.size() == 0)
    return {state, 0};
  bit_t rep = std::numeric_limits<bit_t>::max();
  int rep_sym = 0;
  for (auto sym : syms) {
    assert(sym < group_action.n_symmetries());
    bit_t tstate = group_action.apply(sym, state);
    if (tstate < rep) {
      rep = tstate;
      rep_sym = sym;
    }
  }
  return {rep, rep_sym};
}

// Computes the representative of state and all symmetries that yield it
template <typename bit_t, class GroupAction>
inline std::pair<bit_t, std::vector<int>>
representative_syms(bit_t state, GroupAction const &group_action) {
  if (group_action.n_symmetries() == 0)
    return {state, std::vector<int>()};

  bit_t rep = representative(state, group_action);
  std::vector<int> rep_syms;
  for (int sym = 0; sym < group_action.n_symmetries(); ++sym) {
    bit_t tstate = group_action.apply(sym, state);
    if (tstate == rep) {
      rep_syms.push_back(sym);
    }
  }
  return {rep, rep_syms};
}

// Computes the symmetries, which map origin to target
template <typename bit_t, class GroupAction>
inline std::vector<int> mapping_syms(bit_t origin, bit_t target,
                                     GroupAction const &group_action) {
  std::vector<int> syms;
  for (int sym = 0; sym < group_action.n_symmetries(); ++sym) {
    bit_t tstate = group_action.apply(sym, origin);
    if (tstate == target) {
      syms.push_back(sym);
    }
  }
  return syms;
}

// Computes the norm of a symmetrized state
template <typename bit_t, class GroupAction>
double norm(bit_t state, GroupAction const &group_action,
            Representation const &irrep) {
  complex amplitude = 0.0;
  for (int sym = 0; sym < group_action.n_symmetries(); ++sym) {
    bit_t tstate = group_action.apply(sym, state);
    if (tstate == state) {
      amplitude += irrep.character(sym);
    }
  }
  return std::sqrt(std::abs(amplitude));
}

// Computes the norm of a symmetrized state with fermions
template <typename bit_t, class GroupAction>
inline double norm_fermionic(bit_t state, GroupAction const &group_action,
                             Representation const &irrep) {
  complex amplitude = 0.0;
  int n_sites = group_action.n_sites();
  auto work = fermi_work(n_sites);
  const int *sym_ptr = group_action.permutation_array().data();
  for (int sym = 0; sym < group_action.n_symmetries(); ++sym) {
    bit_t tstate = group_action.apply(sym, state);
    if (tstate == state) {
      if (fermi_bool_of_permutation(state, sym_ptr, work.data())) {
        amplitude -= irrep.character(sym);
      } else {
        amplitude += irrep.character(sym);
      }
    }
    sym_ptr += n_sites;
  }
  return std::sqrt(std::abs(amplitude));
}

// Computes the norm of a symmetrized state with up/dn electrons
template <typename bit_t, class GroupAction>
inline double norm_electron(bit_t ups, bit_t dns,
                            GroupAction const &group_action,
                            Representation const &irrep) {
  assert(group_action.n_symmetries() == irrep.size());
  complex amplitude = 0.0;
  int n_sites = group_action.n_sites();
  auto work = fermi_work(n_sites);
  const int *sym_ptr = group_action.permutation_array().data();
  for (int sym = 0; sym < group_action.n_symmetries(); ++sym) {
    bit_t tups = group_action.apply(sym, ups);

    if (tups == ups) {
      bool fermi_bool_ups =
          fermi_bool_of_permutation(ups, sym_ptr, work.data());

      bit_t tdns = group_action.apply(sym, dns);

      if (tdns == dns) {
        bool fermi_bool_dns =
            fermi_bool_of_permutation(dns, sym_ptr, work.data());

        if (fermi_bool_ups == fermi_bool_dns) {
          amplitude += irrep.character(sym);
        } else {
          amplitude -= irrep.character(sym);
        }
      }
    }
    sym_ptr += n_sites;
  }
  return std::sqrt(std::abs(amplitude));
}

// Computes the norm of a symmetrized state with up/dn electrons (subset of
// syms)
template <typename bit_t, class GroupAction>
inline double
norm_electron_subset(bit_t ups, bit_t dns, GroupAction const &group_action,
                     Representation const &irrep, gsl::span<int const> syms) {
  assert(group_action.n_symmetries() == irrep.size());
  complex amplitude = 0.0;
  int n_sites = group_action.n_sites();
  auto work = fermi_work(n_sites);
  const int *sym_ptr = group_action.permutation_array().data();
  for (int sym : syms) {
    assert(sym < group_action.n_symmetries());

    const int *sym_ptr_current = sym_ptr + sym * n_sites;
    bit_t tups = group_action.apply(sym, ups);

    if (tups == ups) {
      bool fermi_bool_ups =
          fermi_bool_of_permutation(ups, sym_ptr_current, work.data());

      bit_t tdns = group_action.apply(sym, dns);

      if (tdns == dns) {
        bool fermi_bool_dns =
            fermi_bool_of_permutation(dns, sym_ptr_current, work.data());
        if (fermi_bool_ups == fermi_bool_dns) {
          amplitude += irrep.character(sym);
        } else {
          amplitude -= irrep.character(sym);
        }
      }
    }
  }
  return std::sqrt(std::abs(amplitude));
}

} // namespace hydra::symmetries
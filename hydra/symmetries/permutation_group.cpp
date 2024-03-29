#include "permutation_group.h"

#include <cassert>
#include <fstream>
#include <iostream>

#include <hydra/symmetries/operations/symmetry_operations.h>
#include <hydra/utils/logger.h>

namespace hydra {

PermutationGroup::PermutationGroup(std::vector<Permutation> const &permutations)
    : n_sites_(permutations.size() > 0 ? permutations[0].size() : 0),
      n_symmetries_(permutations.size()), permutations_(permutations),
      inverse_(n_symmetries_) {

  // Check whether all permutations have same number of sites
  for (auto p : permutations) {
    if (p.size() != n_sites_) {
      Log.err("Error constructing PermutationGroup: not all Permutations have "
              "the same number of sites");
    }
  }

  // Check whether identity is contained
  if (n_sites_ > 0) {
    auto id = identity_permutation(n_sites_);
    if (std::find(permutations.begin(), permutations.end(), id) ==
        permutations.end()) {
      Log.err("Error constructing PermutationGroup: no identity element found");
    }
  }

  // Check multiplication is complete
  for (auto p1 : permutations) {
    for (auto p2 : permutations) {
      auto p = p1 * p2;
      if (std::find(permutations.begin(), permutations.end(), p) ==
          permutations.end()) {
        Log.err("Error constructing PermutationGroup: group multiplication not "
                "closed");
      }
    }
  }

  // Check if inverse exists
  int idx = 0;
  for (auto p : permutations) {
    auto pinv = hydra::inverse(p);
    auto it = std::find(permutations.begin(), permutations.end(), pinv);
    if (it == permutations.end()) {
      Log.err("Error constructing PermutationGroup: inverse element not found");
    } else {
      int idx_inv = std::distance(permutations.begin(), it);
      inverse_[idx] = idx_inv;
    }
    idx++;
  }
}

PermutationGroup::PermutationGroup(io::FileTomlHandler &&hdl)
    : PermutationGroup(hdl.as<PermutationGroup>()) {}

int PermutationGroup::n_sites() const { return n_sites_; }
int PermutationGroup::n_symmetries() const { return n_symmetries_; }
int PermutationGroup::size() const { return n_symmetries_; }
Permutation const &PermutationGroup::operator[](int sym) const {
  return permutations_[sym];
}
int PermutationGroup::inverse(int sym) const { return inverse_[sym]; }

PermutationGroup
PermutationGroup::subgroup(std::vector<int> const &symmetry_numbers) const {
  std::vector<Permutation> subgroup_permutations;

  for (int n_sym : symmetry_numbers) {

    if ((0 > n_sym) || (n_sym >= n_symmetries_)) {
      Log.err("Error building subgroup of PermutationGroup: "
              "invalid symmetry index");
    }
    subgroup_permutations.push_back(permutations_[n_sym]);
  }

  return PermutationGroup(subgroup_permutations);
}

bool PermutationGroup::operator==(PermutationGroup const &rhs) const {
  return (permutations_ == rhs.permutations_);
}

bool PermutationGroup::operator!=(PermutationGroup const &rhs) const {
  return !operator==(rhs);
}

PermutationGroup::iterator_t PermutationGroup::begin() const {
  return permutations_.begin();
}
PermutationGroup::iterator_t PermutationGroup::end() const {
  return permutations_.end();
}

} // namespace hydra

#include "../catch.hpp"

#include <iostream>

#include <hydra/all.h>
#include <test/blocks/electron/testcases_electron.h>

using namespace hydra;

template <typename bit_t>
void test_fermi_bool_table(PermutationGroup const &group) {
  using combinatorics::Combinations;
  using combinatorics::Subsets;
  using namespace symmetries;

  int n_sites = group.n_sites();
  int n_symmetries = group.n_symmetries();

  for (int npar = 0; npar <= n_sites; ++npar) {

    auto fermi_tbl =
        indexing::FermiTableCombinations<bit_t>(n_sites, npar, group);
    for (int sym = 0; sym < n_symmetries; ++sym) {
      for (bit_t state : Combinations<bit_t>(n_sites, npar)) {
        REQUIRE(fermi_tbl.sign(sym, state) ==
                fermi_bool_of_permutation(state, group[sym]));
      }
    }
  }

  auto fermi_tbl = indexing::FermiTableSubsets<bit_t>(n_sites, group);
  for (int sym = 0; sym < n_symmetries; ++sym) {
    for (bit_t state : Subsets<bit_t>(n_sites)) {
      REQUIRE(fermi_tbl.sign(sym, state) ==
              fermi_bool_of_permutation(state, group[sym]));
    }
  }
}
TEST_CASE("fermi_table", "[symmetries]") {
  hydra::Log("Test fermi_table");
  int max_N = 6;

  for (int n_sites = 0; n_sites <= max_N; ++n_sites) {
    Log("chain N={}", n_sites);
    auto [group, irreps] =
        hydra::testcases::electron::get_cyclic_group_irreps(n_sites);
    test_fermi_bool_table<uint16_t>(group);
    test_fermi_bool_table<uint32_t>(group);
    test_fermi_bool_table<uint64_t>(group);
  }

  Log("triangular 3x3");
  std::string lfile = "data/triangular.9.hop.sublattices.tsl.lat";
  auto permutations = hydra::read_permutations(lfile);
  auto group = PermutationGroup(permutations);
  test_fermi_bool_table<uint16_t>(group);
  test_fermi_bool_table<uint32_t>(group);
  test_fermi_bool_table<uint64_t>(group);

  Log("done");
}

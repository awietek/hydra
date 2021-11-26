#include <hydra/all.h>

int main() {
  using namespace hydra;

  lila::Log.set_verbosity(1);

  int n_sites = 6;
  BondList bondlist;
  for (int s = 0; s < n_sites; ++s) {
    bondlist << Bond("HOP", "T", {s, (s + 1) % n_sites});
    bondlist << Bond("ISING", "JZ", {s, (s + 1) % n_sites});
    bondlist << Bond("EXCHANGE", "JXY", {s, (s + 1) % n_sites});
  }

  // cyclic group as space group
  std::vector<std::vector<int>> permutations;
  for (int sym = 0; sym < n_sites; ++sym) {
    std::vector<int> permutation;
    for (int site = 0; site < n_sites; ++site) {
      int newsite = (site + sym) % n_sites;
      permutation.push_back(newsite);
    }
    permutations.push_back(permutation);
  }
  auto space_group = PermutationGroup(permutations);

  // int n_sites = 16;
  // std::string lfile = std::string("square.") + std::to_string(n_sites) +
  //                     std::string(".tJ.fsl.pbc.lat");
  // auto irrep = read_represenation(lfile, "Gamma.D4.A1");
  // auto permutations = read_permutations(lfile);
  // auto space_group = PermutationGroup(permutations);
  // auto bondlist = read_bondlist(lfile);

  Couplings cpls;
  cpls["T"] = 1.0;
  // cpls["JZ"] = 0.3;
  // cpls["JXY"] = 1.0;

  int n_up = 3;
  int n_dn = 3;
  {

    lila::Vector<double> eigs_sym;
    for (int k = 0; k < n_sites; ++k) {
      lila::Log("k={}", k);
      // Create irrep
      std::vector<complex> chis;
      for (int l = 0; l < n_sites; ++l)
        chis.push_back({std::cos(2 * M_PI * l * k / n_sites),
                        std::sin(2 * M_PI * l * k / n_sites)});
      auto irrep = Representation(chis);
      auto block = tJSymmetric(n_sites, n_up, n_dn, space_group, irrep);
      auto H = MatrixCplx(bondlist, cpls, block, block);
      // LilaPrint(H);
     
      auto eigs = lila::EigenvaluesSym(H);
      LilaPrint(eigs);
      for (auto eig : eigs)
	eigs_sym.push_back(eig);
      
      // lila::Log("--------------------");
      // exit(1);
      
    }
    std::sort(eigs_sym.begin(), eigs_sym.end());

    // lila::tic();
    // double e0 = E0Real(bondlist, cpls, block);
    // lila::toc();

    // lila::Log.out("TJ e0: {}", e0);

    auto block2 = tJ(n_sites, n_up, n_dn);
    auto H2 = MatrixReal(bondlist, cpls, block2, block2);
    // LilaPrint(H2);
    auto eigs_nosym = lila::EigenvaluesSym(H2);
    LilaPrint(eigs_sym);
    LilaPrint(eigs_nosym);
    if (lila::close(eigs_sym, eigs_nosym)) {
      lila::Log("WORKS!");
    } else {
      lila::Log("ERROR!");
    }
    
    // lila::tic();
    // auto block3 = Electron(n_sites, n_up, n_dn);
    // lila::toc("build block");
    // auto H3 = MatrixReal(bondlist, cpls, block3, block3);
    // LilaPrint(H3);
    // LilaPrint(lila::EigenvaluesSym(H3));

    // lila::tic();
    // double e02 = E0Real(bondlist, cpls, block2);
    // lila::toc();

    // lila::Log.out("Spinhalf e0: {}", e02);
  }

  return EXIT_SUCCESS;
}
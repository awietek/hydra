#pragma once

#include <lila/all.h>

#include "common.h"
#include "utils/bitops.h"
#include "utils/iochecks.h"

#include "combinatorics/binomial.h"
#include "combinatorics/subsets.h"
#include "combinatorics/bit_patterns.h"
#include "combinatorics/combinations.h"
#include "combinatorics/up_down_hole.h"

#include "indexing/lintable.h"

#include "models/models.h"
#include "models/spinhalf/spinhalf.h"
#include "models/spinhalf/spinhalf_matrix.h"
#include "models/spinhalf/spinhalf_apply.h"

#include "models/tj/tj.h"
#include "models/tj/tj_matrix.h"
#include "models/tj/tj_apply.h"

#include "models/electron/electron.h"
#include "models/electron/electron_matrix.h"
#include "models/electron/electron_apply.h"
#include "models/electron/electron_symmetric.h"
#include "models/electron/electron_symmetric_matrix.h"
#include "models/electron/electron_symmetric_apply.h"

#include "symmetries/spacegroup.h"
#include "symmetries/spinflip.h"
#include "symmetries/representation.h"
#include "symmetries/spacegroup_operations.h"
#include "symmetries/spacegroup_operator.h"
#include "symmetries/fermi_sign.h"

#include "operators/bond.h"
#include "operators/bondlist.h"
#include "operators/couplings.h"

#include "linalg/sparse_diag.h"
#include "linalg/lanczos/lanczos_generic.h"
#include "linalg/lanczos/tmatrix.h"
#include "linalg/lanczos/lanczos_convergence.h"
#include "linalg/lanczos/lanczos_eigenvalues.h"

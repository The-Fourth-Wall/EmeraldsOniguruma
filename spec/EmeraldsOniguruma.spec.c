#include "../libs/cSpec/export/cSpec.h"
#include "oniguruma/oniguruma.spec.h"

spec_suite({ T_oniguruma(); });

int main(void) { run_spec_suite("all"); }

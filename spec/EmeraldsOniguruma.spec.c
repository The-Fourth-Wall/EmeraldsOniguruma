#include "../libs/cSpec/export/cSpec.h"
#include "oniguruma/oniguruma.spec.h"

int main(void) {
  cspec_run_suite("all", { T_oniguruma(); });
}

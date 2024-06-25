#include "../../libs/cSpec/export/cSpec.h"
#include "../../src/oniguruma/oniguruma.h"

static int matcher(UChar *pattern, UChar *input_string) {
  regex_t *regex;
  OnigErrorInfo error_info;
  int res = onig_new(
    &regex,
    pattern,
    pattern + strlen((char *)pattern),
    ONIG_OPTION_DEFAULT,
    ONIG_ENCODING_ASCII,
    ONIG_SYNTAX_DEFAULT,
    &error_info
  );

  OnigRegion *region = onig_region_new();
  OnigUChar *end     = input_string + strlen((char *)input_string);
  res                = onig_search(
    regex, input_string, end, input_string, end, region, ONIG_OPTION_NONE
  );

  int output = res >= 0 ? region->end[0] : 0;
  onig_region_free(region, 1);
  onig_free(regex);
  return output;
}

module(T_oniguruma, {
  before({
    OnigEncoding encodings[1] = {ONIG_ENCODING_ASCII};
    onig_initialize(encodings, 1);
  });

  after({ onig_end(); });

  describe("#oniguruma", {
    it("matches newlines", {
      UChar *pattern          = (UChar *)"^\\n";
      UChar *input_string     = (UChar *)"\n";
      int end_index           = matcher(pattern, input_string);
      input_string[end_index] = '\0';
      assert_that_int(end_index equals to 1);
      assert_that_charptr((char *)input_string equals to "\n");
    });

    it("matches whitespace", {
      UChar *pattern          = (UChar *)"^\\s+";
      UChar *input_string     = (UChar *)"   ";
      int end_index           = matcher(pattern, input_string);
      input_string[end_index] = '\0';
      assert_that_int(end_index equals to 3);
      assert_that_charptr((char *)input_string equals to "   ");
    });

    it("matches floats", {
      UChar *pattern          = (UChar *)"^\\d+(_\\d+)*\\.\\d+(_\\d+)*";
      UChar *input_string     = (UChar *)"42_42.42_42";
      int end_index           = matcher(pattern, input_string);
      input_string[end_index] = '\0';
      assert_that_int(end_index equals to 11);
      assert_that_charptr((char *)input_string equals to "42_42.42_42");
    });

    it("matches integers", {
      UChar *pattern = (UChar *)"^(?:0[bBoOxX][\\da-fA-F_]+|0|[1-9][\\d_]*)";
      UChar *input_string     = (UChar *)"0x1337beef";
      int end_index           = matcher(pattern, input_string);
      input_string[end_index] = '\0';
      assert_that_int(end_index equals to 10);
      assert_that_charptr((char *)input_string equals to "0x1337beef");
    });

    it("matches identifiers", {
      UChar *pattern          = (UChar *)"^[@$]?[a-zA-Z_][a-zA-Z0-9_]*";
      UChar *input_string     = (UChar *)"$va_riable";
      int end_index           = matcher(pattern, input_string);
      input_string[end_index] = '\0';
      assert_that_int(end_index equals to 10);
      assert_that_charptr((char *)input_string equals to "$va_riable");
    });

    it("matches message symbols", {
      UChar *pattern          = (UChar *)"^[!?+\\-*\\/\\~<>=%|&^;.`]+";
      UChar *input_string     = (UChar *)"><+-";
      int end_index           = matcher(pattern, input_string);
      input_string[end_index] = '\0';
      assert_that_int(end_index equals to 4);
      assert_that_charptr((char *)input_string equals to "><+-");
    });

    it("matches syntax symbols", {
      UChar *pattern          = (UChar *)"^[()\\[\\]{},:#]";
      UChar *input_string     = (UChar *)"[()]";
      int end_index           = matcher(pattern, input_string);
      input_string[end_index] = '\0';
      assert_that_int(end_index equals to 1);
      assert_that_charptr((char *)input_string equals to "[");
    });

    it("matches strings", {
      UChar *pattern          = (UChar *)"^(['\"]).*?\\1";
      UChar *input_string     = (UChar *)"\"some random string\"";
      int end_index           = matcher(pattern, input_string);
      input_string[end_index] = '\0';
      assert_that_int(end_index equals to 20);
      assert_that_charptr((char *)input_string equals to
                          "\"some random string\"");
    });
  });
})

#include <stddef.h>   // for size_t
#include <stdarg.h>   // for va_list
#include <setjmp.h>   // for jmp_buf
#include <cmocka.h>
#include "../src/pg_ais.h"
#include "../src/parse_ais.h"

static void test_valid_input(void **state) {
    const char *input = "!AIVDM,1,1,,A,testmsg*1F";
    ais *result = ais_from_cstring_external(input);
    assert_non_null(result);
    free(result);
}

static void test_invalid_input_prefix(void **state) {
    const char *input = "bad sentence";
    assert_null(ais_from_cstring_external(input));
}

static void test_roundtrip(void **state) {
    const char *input = "!AIVDM,1,1,,A,sample123*00";
    ais *result = ais_from_cstring_external(input);
    assert_non_null(result);
    char *output = ais_to_cstring_external(result);
    assert_string_equal(output, input);
    free(result);
    free(output);
}

static void test_parse_valid_sentence(void **state) {
    const char *s = "!AIVDM,1,1,,A,15Muq60001G?tTpE>Gbk0?wN0<0,0*7D";
    AISMessage msg;
    assert_true(parse_ais_sentence(s, &msg));
    assert_string_equal(msg.talker, "AI");
    assert_string_equal(msg.type, "VDM");
    assert_int_equal(msg.message_id, 1); // Should match payload ID
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_valid_input),
        cmocka_unit_test(test_invalid_input_prefix),
        cmocka_unit_test(test_roundtrip),
        cmocka_unit_test(test_parse_valid_sentence),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}

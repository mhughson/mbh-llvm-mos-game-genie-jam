/**
 * Custom font built for 4x6 letter graphics (3x5 active)
 * The list of letters is based on the `Letter` enum in `text_render.hpp`, so if you
 * want to add new letters, then update that enum accordingly
 *
 * The `_mt` function is used to create a list of metatiles for each of the characters
 * by parsing the string at compile time. Anything outside of the `|` pipe characters are unused.
 */

/* NUL */ R"(
|    |
|    |
|    |
|    |
|    |
|    |
)"_mt,
/* 0 */ R"(
| o  |
|o o |
|o o |
|o o |
| o  |
|    |
)"_mt,
/* 1 */ R"(
| o  |
|oo  |
| o  |
| o  |
|ooo |
|    |
)"_mt,
/* 2 */ R"(
|oo  |
|  o |
| o  |
|o   |
|ooo |
|    |
)"_mt,
/* 3 */ R"(
|ooo |
|  o |
| oo |
|  o |
|ooo |
|    |
)"_mt,
/* 4 */ R"(
|o o |
|o o |
|ooo |
|  o |
|  o |
|    |
)"_mt,
/* 5 */ R"(
|ooo |
|o   |
|ooo |
|  o |
|ooo |
|    |
)"_mt,
/* 6 */ R"(
|ooo |
|o   |
|ooo |
|o o |
|ooo |
|    |
)"_mt,
/* 7 */ R"(
|ooo |
|  o |
|  o |
|  o |
|  o |
|    |
)"_mt,
/* 8 */ R"(
|ooo |
|o o |
|ooo |
|o o |
|ooo |
|    |
)"_mt,
/* 9 */ R"(
|ooo |
|o o |
|ooo |
|  o |
|  o |
|    |
)"_mt,
/* A */ R"(
| o  |
|o o |
|ooo |
|o o |
|o o |
|    |
)"_mt,
/* B */ R"(
|oo  |
|o o |
|oo  |
|o o |
|oo  |
|    |
)"_mt,
/* C */ R"(
| oo |
|o   |
|o   |
|o   |
| oo |
|    |
)"_mt,
/* D */ R"(
|oo  |
|o o |
|o o |
|o o |
|oo  |
|    |
)"_mt,
/* E */ R"(
|ooo |
|o   |
|ooo |
|o   |
|ooo |
|    |
)"_mt,
/* F */ R"(
|ooo |
|o   |
|ooo |
|o   |
|o   |
|    |
)"_mt,
/* G */ R"(
| oo |
|o   |
|ooo |
|o o |
| oo |
|    |
)"_mt,
/* H */ R"(
|o o |
|o o |
|ooo |
|o o |
|o o |
|    |
)"_mt,
/* I */ R"(
|ooo |
| o  |
| o  |
| o  |
|ooo |
|    |
)"_mt,
/* J */ R"(
|  o |
|  o |
|  o |
|o o |
| o  |
|    |
)"_mt,
/* K */ R"(
|o o |
|o o |
|oo  |
|o o |
|o o |
|    |
)"_mt,
/* L */ R"(
|o   |
|o   |
|o   |
|o   |
|ooo |
|    |
)"_mt,
/* M */ R"(
|o o |
|ooo |
|ooo |
|o o |
|o o |
|    |
)"_mt,
/* N */ R"(
|o o |
|ooo |
|ooo |
|ooo |
|o o |
|    |
)"_mt,
/* O */ R"(
| o  |
|o o |
|o o |
|o o |
| o  |
|    |
)"_mt,
/* P */ R"(
|oo  |
|o o |
|oo  |
|o   |
|o   |
|    |
)"_mt,
/* Q */ R"(
| o  |
|o o |
|o o |
|ooo |
| oo |
|    |
)"_mt,
/* R */ R"(
|oo  |
|o o |
|ooo |
|oo  |
|o o |
|    |
)"_mt,
/* S */ R"(
| oo |
|o   |
| o  |
|  o |
|oo  |
|    |
)"_mt,
/* T */ R"(
|ooo |
| o  |
| o  |
| o  |
| o  |
|    |
)"_mt,
/* U */ R"(
|o o |
|o o |
|o o |
|o o |
| oo |
|    |
)"_mt,
/* V */ R"(
|o o |
|o o |
|o o |
| o  |
| o  |
|    |
)"_mt,
/* W */ R"(
|o o |
|o o |
|ooo |
|ooo |
|o o |
|    |
)"_mt,
/* X */ R"(
|o o |
|o o |
| o  |
|o o |
|o o |
|    |
)"_mt,
/* Y */ R"(
|o o |
|o o |
| o  |
| o  |
| o  |
|    |
)"_mt,
/* Z */ R"(
|ooo |
|  o |
| o  |
|o   |
|ooo |
|    |
)"_mt,
/* SPACE */ R"(
|    |
|    |
|    |
|    |
|    |
|    |
)"_mt,

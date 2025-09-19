/**
 * Custom font built for 4x6 letter graphics (3x5 active)
 * The list of letters is based on the `Letter` enum in `text_render.hpp`, so if you
 * want to add new letters, then update that enum accordingly
 *
 * The `_mt_2_3` function is used to create a list of metatiles for each of the characters
 * by parsing the string at compile time. Anything outside of the `|` pipe characters are unused.
 */

/* 0 */ R"(
| o  |
|o o |
|o o |
|o o |
| o  |
|    |
)"_mt_2_3,
/* 1 */ R"(
| o  |
|oo  |
| o  |
| o  |
|ooo |
|    |
)"_mt_2_3,
/* 2 */ R"(
|oo  |
|  o |
| o  |
|o   |
|ooo |
|    |
)"_mt_2_3,
/* 3 */ R"(
|ooo |
|  o |
| oo |
|  o |
|ooo |
|    |
)"_mt_2_3,
/* 4 */ R"(
|o o |
|o o |
|ooo |
|  o |
|  o |
|    |
)"_mt_2_3,
/* 5 */ R"(
|ooo |
|o   |
|ooo |
|  o |
|ooo |
|    |
)"_mt_2_3,
/* 6 */ R"(
|ooo |
|o   |
|ooo |
|o o |
|ooo |
|    |
)"_mt_2_3,
/* 7 */ R"(
|ooo |
|  o |
|  o |
|  o |
|  o |
|    |
)"_mt_2_3,
/* 8 */ R"(
|ooo |
|o o |
|ooo |
|o o |
|ooo |
|    |
)"_mt_2_3,
/* 9 */ R"(
|ooo |
|o o |
|ooo |
|  o |
|  o |
|    |
)"_mt_2_3,
/* A */ R"(
| o  |
|o o |
|ooo |
|o o |
|o o |
|    |
)"_mt_2_3,
/* B */ R"(
|oo  |
|o o |
|oo  |
|o o |
|oo  |
|    |
)"_mt_2_3,
/* C */ R"(
| oo |
|o   |
|o   |
|o   |
| oo |
|    |
)"_mt_2_3,
/* D */ R"(
|oo  |
|o o |
|o o |
|o o |
|oo  |
|    |
)"_mt_2_3,
/* E */ R"(
|ooo |
|o   |
|ooo |
|o   |
|ooo |
|    |
)"_mt_2_3,
/* F */ R"(
|ooo |
|o   |
|ooo |
|o   |
|o   |
|    |
)"_mt_2_3,
/* G */ R"(
| oo |
|o   |
|ooo |
|o o |
| oo |
|    |
)"_mt_2_3,
/* H */ R"(
|o o |
|o o |
|ooo |
|o o |
|o o |
|    |
)"_mt_2_3,
/* I */ R"(
|ooo |
| o  |
| o  |
| o  |
|ooo |
|    |
)"_mt_2_3,
/* J */ R"(
|  o |
|  o |
|  o |
|o o |
| o  |
|    |
)"_mt_2_3,
/* K */ R"(
|o o |
|o o |
|oo  |
|o o |
|o o |
|    |
)"_mt_2_3,
/* L */ R"(
|o   |
|o   |
|o   |
|o   |
|ooo |
|    |
)"_mt_2_3,
/* M */ R"(
|o o |
|ooo |
|ooo |
|o o |
|o o |
|    |
)"_mt_2_3,
/* N */ R"(
|o o |
|ooo |
|ooo |
|ooo |
|o o |
|    |
)"_mt_2_3,
/* O */ R"(
| o  |
|o o |
|o o |
|o o |
| o  |
|    |
)"_mt_2_3,
/* P */ R"(
|oo  |
|o o |
|oo  |
|o   |
|o   |
|    |
)"_mt_2_3,
/* Q */ R"(
| o  |
|o o |
|o o |
|ooo |
| oo |
|    |
)"_mt_2_3,
/* R */ R"(
|oo  |
|o o |
|ooo |
|oo  |
|o o |
|    |
)"_mt_2_3,
/* S */ R"(
| oo |
|o   |
| o  |
|  o |
|oo  |
|    |
)"_mt_2_3,
/* T */ R"(
|ooo |
| o  |
| o  |
| o  |
| o  |
|    |
)"_mt_2_3,
/* U */ R"(
|o o |
|o o |
|o o |
|o o |
| oo |
|    |
)"_mt_2_3,
/* V */ R"(
|o o |
|o o |
|o o |
| o  |
| o  |
|    |
)"_mt_2_3,
/* W */ R"(
|o o |
|o o |
|ooo |
|ooo |
|o o |
|    |
)"_mt_2_3,
/* X */ R"(
|o o |
|o o |
| o  |
|o o |
|o o |
|    |
)"_mt_2_3,
/* Y */ R"(
|o o |
|o o |
| o  |
| o  |
| o  |
|    |
)"_mt_2_3,
/* Z */ R"(
|ooo |
|  o |
| o  |
|o   |
|ooo |
|    |
)"_mt_2_3,
/* SPACE */ R"(
|    |
|    |
|    |
|    |
|    |
|    |
)"_mt_2_3,

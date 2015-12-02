// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <locale.h>
#include <stddef.h>
#include <limits.h>

// Minimal nonconfigurable locale "support".

struct lconv *localeconv(void)
{
	static struct lconv locale = {
		.decimal_point = ".",
		.thousands_sep = "",
		.grouping = "",
		.int_curr_symbol = "",
		.currency_symbol = "",
		.mon_decimal_point = "",
		.mon_thousands_sep = "",
		.mon_grouping = "",
		.positive_sign = "",
		.negative_sign = "",
		.frac_digits = CHAR_MAX,
		.p_cs_precedes = CHAR_MAX,
		.n_cs_precedes = CHAR_MAX,
		.p_sep_by_space = CHAR_MAX,
		.n_sep_by_space = CHAR_MAX,
		.p_sign_posn = CHAR_MAX,
		.n_sign_posn = CHAR_MAX,
		.int_frac_digits = CHAR_MAX,
		.int_p_cs_precedes = CHAR_MAX,
		.int_n_cs_precedes = CHAR_MAX,
		.int_p_sep_by_space = CHAR_MAX,
		.int_n_sep_by_space = CHAR_MAX,
		.int_p_sign_posn = CHAR_MAX,
		.int_n_sign_posn = CHAR_MAX
	};
	return &locale;
}

char *setlocale(int category, const char *locale)
{
	return NULL;
}



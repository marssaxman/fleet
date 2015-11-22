#include "format.h"

void _format_start(struct format_state *state, const char *format_string)
{
	state->fmt = format_string;
}

bool _format_done(struct format_state *state)
{
	return '\0' == *state->fmt;
}

static struct format_chunk format_specifier(struct format_state *state)
{
	struct format_chunk out = {0,0};
	return out;
}

static struct format_chunk format_literal(struct format_state *state)
{
	struct format_chunk out = {state->fmt, 0};
	while (*state->fmt != '\0' && *state->fmt != '%') {
		state->fmt++;
	}
	out.size = state->fmt - out.addr;
	return out;
}

struct format_chunk _format_next(struct format_state *state)
{
	return ((*state->fmt == '%')? format_specifier: format_literal)(state);
}



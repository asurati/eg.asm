/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2022 Amol Surati */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

#include "main.h"

#define MAX_TOKENS			200

static
int inst_base_tokenize(struct inst_base *this, int ls, int le)
{
	int is_delim, i, j, k, is_space, ts, len;
	const char *buf;
	static char *tokens[MAX_TOKENS];
	static const char *delims = ".,;()[]-+/*$";

	buf = this->as->buf;
	ts = -1;
	for (i = ls, j = 0; i < le; ++i) {
		is_delim = is_space = isspace(buf[i]);

		/* Not even a delim, because no open token. */
		if (is_delim && ts < 0)
			continue;

		/* in-instruction delims */
		for (k = 0; k < (int)strlen(delims); ++k)
			is_delim |= buf[i] == delims[k];

		if (is_delim) {
			/*
			 * For back to back delims, there may not be a token
			 * open. for e.g. [0x1f,ix0][0x0]. For ][, there is no
			 * open token.
			 */
			/* Close any open token */
			if (ts >= 0) {
				len = i - ts;
				tokens[j] = calloc(len + 1, sizeof(char));
				memcpy(tokens[j], &buf[ts], len);
				++j;
				ts = -1;
			}

			if (is_space)
				continue;

			/* Create a token for the non-space delims */
			assert(j < MAX_TOKENS);
			tokens[j] = calloc(2, sizeof(char));
			tokens[j][0] = buf[i];
			++j;

			/* ; is the end of the curr instr. return */
			if (buf[i] == ';')
				break;
		} else if (ts < 0) {
			ts = i;
		}
	}

	if (i == le)
		return EINVAL;

	this->num_tokens = j;
	this->tokens = calloc(j, sizeof(char *));
	if (this->tokens == NULL)
		return ENOMEM;
	memcpy(this->tokens, tokens, j * sizeof(char *));
	return 0;
}

/* le contains the buf_size, i.e. the end of the file */
static
int inst_base_parse_labels(struct inst_base *this, int *ls, int *le)
{
	int i, e, s, len, j;
	const char *buf;

	buf = this->as->buf;
	i = *ls;
	e = *le;
	for (;;) {
		/* Skip whitespace. */
		for (i = i; i < e; ++i) {
			if (!isspace(buf[i]))
				break;
		}

		/* Line starts here */
		*ls = s = i;

		/* Done with the file */
		if (s == e)
			return 0;

		/* Is it a comment? Ignore until the end of the line. */
		if (buf[s] == '#') {
			for (i = s; i < e; ++i) {
				/* The \n will be skipped in the loop abv */
				if (buf[i] == '\n')
					break;
			}
			continue;
		}

		/* Scan for delimiters */
		for (i = s; i < e; ++i) {
			if (buf[i] == ';')	/* InstrEnd */
				break;
			if (buf[i] == ':')	/* LabelEnd */
				break;
		}

		/* No delims found. Invalid */
		if (i == e)
			return EINVAL;

		/* Delim found is InstrEnd. Return. */
		if (buf[i] == ';') {
			*le = i + 1;	/* Next invocation will begin here */
			return 0;
		}

		/* Else a : was found. */
		len = i - s;
		++i;	/* Skip the :, and continue scan for more labels. */

		/* Is the label correct? */
		if (len <= 0)	/* : alone */
			return EINVAL;

		/* TODO: Lables should have only letters, numbers and _ */

		/* Add the label to the current instruction. */
		j = this->num_labels++;
		this->labels = realloc(this->labels, (j + 1) * sizeof(char *));
		this->labels[j] = calloc(len + 1, sizeof(char));
		memcpy((void *)this->labels[j], &buf[s], len);
	}
}

int inst_base_fix_label(struct inst_base *this, const char *label, int *out)
{
	int i, j;
	struct asm_base *as;
	const struct inst_all *in;

	as = this->as;
	for (i = 0; i < as->num_insts; ++i) {
		in = &as->insts[i];
		for (j = 0; j < in->base.num_labels; ++j) {
			if (strcmp(in->base.labels[j], label))
				continue;
			*out = in->base.pc;
			return 0;
		}
	}
	return EINVAL;
}

static
int inst_all_parse(struct inst_all *this)
{
	int err;
	struct inst_base *base;

	base = &this->base;

	/* Should be one of c,a,l,v,t,m,g */
	err = EINVAL;
	if (inst_base_is_next_token(base, "c"))
		err = inst_cf_parse_all(this);
	else if (inst_base_is_next_token(base, "v"))
		err = inst_vtx_parse_all(this);
	else if (inst_base_is_next_token(base, "a"))
		err = inst_alu_parse_all(this);
	else if (inst_base_is_next_token(base, "t"))
		err = inst_tex_parse_all(this);
	return err;
}

static
int inst_all_fix_labels(struct inst_all *this)
{
	int err;
	struct inst_base *base;

	base = &this->base;

	err = 0;
	if (base->type >= IT_CF && base->type <= IT_CF_AIE_SWIZ)
		err = inst_cf_fix_labels_all(this);
	return err;
}

#if 0
static
int inst_base_encode(struct inst_base *this)
{
	union inst_all *in;

	in = (union inst_all *)this;	/* Because ain.base is first member */
	if (this->type >= IT_CF && this->type <= IT_CF_AIE_SWIZ)
		return inst_cf_encode_all(in);
	if (this->type == IT_VTX_GPR || this->type == IT_VTX_SEM)
		return inst_vtx_encode_all(in);
	if (this->type == IT_ALU_OP2 || this->type == IT_ALU_OP3)
		return inst_alu_encode_all(in);
	return EINVAL;
}
#endif

static
void inst_all_print(const struct inst_all *this)
{
	int i, j;
	const char *buf;
	const struct inst_base *base;

	base = &this->base;
	buf = this->base.as->buf;

	/* print any labels first. */
	for (i = 0; i < base->num_labels; ++i)
		printf("/*%s:*/\n", base->labels[i]);
	for (i = 0; i < base->num_words; ++i)
		printf("0x%08x, ", base->w[i]);
	printf ("/*%d: ", base->pc);
	for (i = base->ls; i < base->le; ++i) {
		/* Replace multiple spaces with a single space */
		if (isspace(buf[i])) {
			for (j = i + 1; j < base->le; ++j) {
				if (!isspace(buf[j]))
					break;
			}
			i = j - 1;
			/* Nothing but space until the end */
			if (j == base->le)
				continue;
			printf(" ");

		} else {
			printf("%c", buf[i]);
		}
	}
	printf("*/\n");
}

static
int inst_base_parse_number_token(struct inst_base *this, const char *t,
				 int *out)
{
	if (strlen(t) >= 2 && t[0] == '0' && t[1] == 'x')
		sscanf(&t[2], "%x", (unsigned int *)out);
	else
		sscanf(t, "%d", out);
	return 0;
	(void)this;
}

int inst_base_parse_number(struct inst_base *this, int *out)
{
	int err;
	const char *t;

	t = inst_base_get_next_token(this);
	err = inst_base_parse_number_token(this, t, out);
	return err;
}

int inst_base_parse_count(struct inst_base *this, int *out)
{
	int err;

	if (inst_base_is_next_token(this, "(") == false)
		return EINVAL;

	err = inst_base_parse_number(this, out);
	if (err)
		return err;

	if (inst_base_is_next_token(this, ")") == false)
		return EINVAL;
	return 0;
}

int inst_base_parse_register(struct inst_base *this, int *out)
{
	const char *t;

	t = inst_base_get_next_token(this);
	if (t[0] != 'r' && t[0] != 'R')
		return EINVAL;
	return inst_base_parse_number_token(this, &t[1], out);
}

static
int inst_base_parse_swizzle_char(struct inst_base *this, char sc)
{
	if (sc == 'x' || sc == 'X') return SEL_X;
	if (sc == 'y' || sc == 'Y') return SEL_Y;
	if (sc == 'z' || sc == 'Z') return SEL_Z;
	if (sc == 'w' || sc == 'W') return SEL_W;
	if (sc == '0') return SEL_0;
	if (sc == '1') return SEL_1;
	return SEL_MASK;
	(void)this;
}

int inst_base_parse_channel(struct inst_base *this, int *out)
{
	const char *t;

	t = inst_base_get_next_token(this);
	if (strlen(t) != 1)
		return EINVAL;
	*out = inst_base_parse_swizzle_char(this, t[0]);
	return 0;
}

int inst_base_parse_swizzle(struct inst_base *this, int *swiz)
{
	int i;
	const char *t;

	t = inst_base_get_next_token(this);
	if (strlen(t) != 4)
		return EINVAL;
	for (i = 0; i < 4; ++i)
		swiz[i] = inst_base_parse_swizzle_char(this, t[i]);
	return 0;
}

int main(int argc, char **argv)
{
	int i, size, err, ls, le, pc;
	FILE *f;
	char *buf;
	struct asm_base as;
	struct inst_all *in;

	if (argc != 2) {
		printf("Usage: %s input.s\n", argv[0]);
		return EINVAL;
	}

	f = fopen(argv[1], "rb");
	if (f == NULL)
		return errno;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	buf = malloc(size);
	if (buf == NULL)
		return ENOMEM;
	fread(buf, 1, size, f);
	fclose(f);

	pc = 0;
	asm_base_construct(&as, buf, size);
	for (i = 0; i < as.buf_size; i = le) {
		in = &as.insts[as.num_insts];

		/* inst_base construction done here for all inst types */
		inst_all_construct(in, &as);

		/* [ls, le) */
		ls = i;
		le = as.buf_size;
		err = inst_base_parse_labels(&in->base, &ls, &le);
		if (err) {
			printf("labels err\n");
			break;
		}

		/* Let it end with i == buf_size. */
		if (ls == as.buf_size)
			continue;

		err = inst_base_tokenize(&in->base, ls, le);
		if (err) {
			printf("tokenize err\n");
			break;
		}
#if 0
		{
			int j;
			for (j = 0; j < in->num_tokens; ++j)
				printf("tokens[%d]: %s\n", j, in->tokens[j]);
		}
#endif

		err = inst_all_parse(in);
		if (err) {
			printf("parse err\n");
			break;
		}

		in->base.pc = pc;
		in->base.ls = ls;
		in->base.le = le;
		pc += in->base.num_words / 2;	/* For the next instruction */

		++as.num_insts;
		if (as.num_insts % 100)
			continue;
		size = (as.num_insts + 100) * sizeof(*in);
		as.insts = realloc(as.insts, size);
	}

	if (err) {
		printf("err %d, i = %x, done = %d\n", err, i, as.num_insts);
		return err;
	}
	for (i = 0; i < as.num_insts; ++i) {
		in = &as.insts[i];
		err = inst_all_fix_labels(in);
		if (err)
			break;
	}

	if (err) {
		printf("fix_labels err %d, i = %x\n", err, i);
		return err;
	}

#if 0
	for (i = 0; i < as.num_insts; ++i) {
		in = &as.insts[i].base;
		err = inst_base_encode(in);
		if (err)
			break;
	}

	if (err) {
		printf("encode err %d, i = %x\n", err, i);
		return err;
	}
#endif
	for (i = 0; i < as.num_insts; ++i) {
		in = &as.insts[i];
		inst_all_print(in);
	}
	return err;
}

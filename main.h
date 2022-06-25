/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2022 Amol Surati */

#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stdint.h>

#include "bits.h"
#include "cf.h"
#include "vtx.h"
#include "alu.h"
#include "tex.h"

#ifndef container_of
#define container_of(p, t, m)		(t *)((char *)p - offsetof(t, m))
#endif

#define SEL_X				0
#define SEL_Y				1
#define SEL_Z				2
#define SEL_W				3
#define SEL_0				4
#define SEL_1				5
#define SEL_MASK			7

/* Based on Table 10.1 Summary of Microcode Formats */
enum inst_type {
	IT_INVALID,

	IT_CF,
	IT_CF_GWS,
	IT_CF_ALU,
	IT_CF_ALU_EXT,
	IT_CF_AIE_RAT,
	IT_CF_AIE_BUF,
	IT_CF_AIE_SWIZ,

	IT_ALU_OP2,
	IT_ALU_OP3,

	IT_LDS,

	IT_VTX_GPR,
	IT_VTX_SEM,

	IT_TEX,

	IT_MEM_RD,

	IT_GDS,
};

struct asm_base;
struct inst_base {
	struct asm_base			*as;
	const char			**tokens;
	const char			**labels;

	int				w[4];
	int				pc;	/* 64-bit units */
	int				num_words;

	enum inst_type			type;

	int				num_labels;
	int				num_tokens;
	int				next_token;

	int				ls;	/* For printing. */
	int				le;
};

static inline
void inst_base_construct(struct inst_base *this, struct asm_base *as)
{
	this->as = as;
	this->next_token = -1;
}

static inline
const char *inst_base_get_next_token(struct inst_base *this)
{
	assert(this->next_token < this->num_tokens - 1);
	++this->next_token;
	return this->tokens[this->next_token];
}

static inline
bool inst_base_is_next_token(struct inst_base *this, const char *s)
{
	const char *t;

	t = inst_base_get_next_token(this);
	if (!strcmp(t, s))
		return true;
	--this->next_token;	/* Undo if not found. */
	return false;
}

struct inst_all {
	struct inst_base		base;
	union {
		struct inst_cf		cf;
		struct inst_cf_gws	cf_gws;
		struct inst_cf_alu	cf_alu;
		struct inst_cf_aie_rat	cf_aie_rat;
		struct inst_cf_aie_buf	cf_aie_buf;
		struct inst_cf_aie_swiz	cf_aie_swiz;

		struct inst_vtx		vtx;
		struct inst_alu		alu;
		struct inst_tex		tex;
	} u;
};

static inline
void inst_all_construct(struct inst_all *this, struct asm_base *as)
{
	memset(this, 0, sizeof(*this));
	inst_base_construct(&this->base, as);
}


struct asm_base {
	const char			*buf;

	struct inst_all			*insts;

	int				buf_size;
	int				num_insts;
};

static inline
void asm_base_construct(struct asm_base *this, const char *buf, int buf_size)
{
	this->buf	= buf;
	this->buf_size	= buf_size;
	this->insts	= malloc(100 * sizeof(struct inst_all));
	this->num_insts	= 0;
}

int	inst_cf_parse_all(struct inst_all *all);
int	inst_vtx_parse_all(struct inst_all *all);
int	inst_alu_parse_all(struct inst_all *all);
int	inst_tex_parse_all(struct inst_all *all);

int	inst_base_parse_swizzle(struct inst_base *this, int *out);
int	inst_base_parse_channel(struct inst_base *this, int *out);
int	inst_base_parse_register(struct inst_base *this, int *out);
int	inst_base_parse_number(struct inst_base *this, int *out);
int	inst_base_parse_count(struct inst_base *this, int *out);
#endif

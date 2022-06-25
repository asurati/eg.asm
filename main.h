/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2022 Amol Surati */

#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stdint.h>

#include "bits.h"
#include "cf.h"

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

int	inst_base_parse_swizzle(struct inst_base *this, int *swiz);
int	inst_base_parse_register(struct inst_base *this, int *out);
int	inst_base_parse_number(struct inst_base *this, int *out);
int	inst_base_parse_count(struct inst_base *this, int *out);

/**** ALU_WORD0 ****/
#define SQ_ALU_WORD0_SRC0_SEL_POS			0
#define SQ_ALU_WORD0_SRC0_REL_POS			9
#define SQ_ALU_WORD0_SRC0_CHAN_POS			10
#define SQ_ALU_WORD0_SRC0_NEG_POS			12
#define SQ_ALU_WORD0_SRC1_SEL_POS			13
#define SQ_ALU_WORD0_SRC1_REL_POS			22
#define SQ_ALU_WORD0_SRC1_CHAN_POS			23
#define SQ_ALU_WORD0_SRC1_NEG_POS			25
#define SQ_ALU_WORD0_INDEX_MODE_POS			26
#define SQ_ALU_WORD0_PRED_SEL_POS			29
#define SQ_ALU_WORD0_LAST_POS				31
#define SQ_ALU_WORD0_SRC0_SEL_BITS			9
#define SQ_ALU_WORD0_SRC0_REL_BITS			1
#define SQ_ALU_WORD0_SRC0_CHAN_BITS			2
#define SQ_ALU_WORD0_SRC0_NEG_BITS			1
#define SQ_ALU_WORD0_SRC1_SEL_BITS			9
#define SQ_ALU_WORD0_SRC1_REL_BITS			1
#define SQ_ALU_WORD0_SRC1_CHAN_BITS			2
#define SQ_ALU_WORD0_SRC1_NEG_BITS			1
#define SQ_ALU_WORD0_INDEX_MODE_BITS			3
#define SQ_ALU_WORD0_PRED_SEL_BITS			2
#define SQ_ALU_WORD0_LAST_BITS				1

#define INDEX_AR_X					0
#define INDEX_LOOP					4
#define INDEX_GLOBAL					5
#define INDEX_GLOBAL_AR_X				6

#define PRED_SEL_OFF					0
#define PRED_SEL_0					2
#define PRED_SEL_1					3

#define ALU_SRC_GPR_BASE				0
#define ALU_SRC_PARAM_BASE				0x1c0

/**** ALU_WORD1_OP2 ****/
#define SQ_ALU_WORD1_OP2_SRC0_ABS_POS			0
#define SQ_ALU_WORD1_OP2_SRC1_ABS_POS			1
#define SQ_ALU_WORD1_OP2_UPDATE_EXEC_MASK_POS		2
#define SQ_ALU_WORD1_OP2_UPDATE_PRED_POS		3
#define SQ_ALU_WORD1_OP2_WRITE_DST_POS			4
#define SQ_ALU_WORD1_OP2_OUT_MOD_POS			5
#define SQ_ALU_WORD1_OP2_SRC0_ABS_BITS			1
#define SQ_ALU_WORD1_OP2_SRC1_ABS_BITS			1
#define SQ_ALU_WORD1_OP2_UPDATE_EXEC_MASK_BITS		1
#define SQ_ALU_WORD1_OP2_UPDATE_PRED_BITS		1
#define SQ_ALU_WORD1_OP2_WRITE_DST_BITS			1
#define SQ_ALU_WORD1_OP2_OUT_MOD_BITS			2

#define SQ_ALU_WORD1_INST_POS				7
#define SQ_ALU_WORD1_BANK_SWIZZLE_POS			18
#define SQ_ALU_WORD1_DST_GPR_POS			21
#define SQ_ALU_WORD1_DST_REL_POS			28
#define SQ_ALU_WORD1_DST_CHAN_POS			29
#define SQ_ALU_WORD1_CLAMP_POS				31
#define SQ_ALU_WORD1_INST_BITS				11
#define SQ_ALU_WORD1_BANK_SWIZZLE_BITS			3
#define SQ_ALU_WORD1_DST_GPR_BITS			7
#define SQ_ALU_WORD1_DST_REL_BITS			1
#define SQ_ALU_WORD1_DST_CHAN_BITS			2
#define SQ_ALU_WORD1_CLAMP_BITS				1

#define ALU_OP2_INST_INTERP_XY				214
#define ALU_OP2_INST_INTERP_ZW				215
#define ALU_OP2_INST_INTERP_Z				217

#define ALU_OMOD_OFF					0
#define ALU_OMOD_M2					1
#define ALU_OMOD_M4					2
#define ALU_OMOD_D2					3

#define ALU_VEC_012					0
#define ALU_VEC_021					1
#define ALU_VEC_120					2
#define ALU_VEC_102					3
#define ALU_VEC_201					4
#define ALU_VEC_210					5

#define ALU_SCL_210					0
#define ALU_SCL_122					1
#define ALU_SCL_212					2
#define ALU_SCL_221					3

/**** ALU_WORD1_OP3 ****/
/* TODO */

/**** VTX_WORD0 ****/
#define SQ_VTX_WORD0_INST_POS				0
#define SQ_VTX_WORD0_FETCH_TYPE_POS			5
#define SQ_VTX_WORD0_FETCH_WHOLE_QUAD_POS		7
#define SQ_VTX_WORD0_BUF_ID_POS				8
#define SQ_VTX_WORD0_SRC_GPR_POS			16
#define SQ_VTX_WORD0_SRC_REL_POS			23
#define SQ_VTX_WORD0_SRC_SEL_X_POS			24
#define SQ_VTX_WORD0_MEGA_FETCH_COUNT_POS		26
#define SQ_VTX_WORD0_INST_BITS				5
#define SQ_VTX_WORD0_FETCH_TYPE_BITS			2
#define SQ_VTX_WORD0_FETCH_WHOLE_QUAD_BITS		1
#define SQ_VTX_WORD0_BUF_ID_BITS			8
#define SQ_VTX_WORD0_SRC_GPR_BITS			7
#define SQ_VTX_WORD0_SRC_REL_BITS			1
#define SQ_VTX_WORD0_SRC_SEL_X_BITS			2
#define SQ_VTX_WORD0_MEGA_FETCH_COUNT_BITS		6

#define VTX_FETCH_TYPE_VERTEX				0
#define VTX_FETCH_TYPE_INSTANCE				1

#define VTX_INST_FETCH					0
#define VTX_INST_SEMANTIC				1

/**** VTX_WORD1_GPR ****/
#define SQ_VTX_WORD1_GPR_DST_GPR_POS			0
#define SQ_VTX_WORD1_GPR_DST_REL_POS			7
#define SQ_VTX_WORD1_GPR_DST_GPR_BITS			7
#define SQ_VTX_WORD1_GPR_DST_REL_BITS			1

/**** VTX_WORD1_SEM ****/
#define SQ_VTX_WORD1_SEM_ID_POS				0
#define SQ_VTX_WORD1_SEM_ID_BITS			8

/**** VTX_WORD1 ****/
#define SQ_VTX_WORD1_DST_SEL_X_POS			9
#define SQ_VTX_WORD1_DST_SEL_Y_POS			12
#define SQ_VTX_WORD1_DST_SEL_Z_POS			15
#define SQ_VTX_WORD1_DST_SEL_W_POS			18
#define SQ_VTX_WORD1_USE_CONST_FIELDS_POS		21
#define SQ_VTX_WORD1_DATA_FORMAT_POS			22
#define SQ_VTX_WORD1_NUM_FORMAT_ALL_POS			28
#define SQ_VTX_WORD1_FORMAT_COMP_ALL_POS		30
#define SQ_VTX_WORD1_SRF_MODE_ALL_POS			31
#define SQ_VTX_WORD1_DST_SEL_X_BITS			3
#define SQ_VTX_WORD1_DST_SEL_Y_BITS			3
#define SQ_VTX_WORD1_DST_SEL_Z_BITS			3
#define SQ_VTX_WORD1_DST_SEL_W_BITS			3
#define SQ_VTX_WORD1_USE_CONST_FIELDS_BITS		1
#define SQ_VTX_WORD1_DATA_FORMAT_BITS			6
#define SQ_VTX_WORD1_NUM_FORMAT_ALL_BITS		2
#define SQ_VTX_WORD1_FORMAT_COMP_ALL_BITS		1
#define SQ_VTX_WORD1_SRF_MODE_ALL_BITS			1

#define FMT_32_32_FLOAT					30
#define FMT_32_32_32_32_FLOAT				35
#define FMT_32_32_32					47
#define FMT_32_32_32_FLOAT				48

#define FORMAT_COMP_UNSIGNED				0
#define FORMAT_COMP_SIGNED				1

#define NUM_FORMAT_NORM					0
#define NUM_FORMAT_INT					1
#define NUM_FORMAT_SCALED				2

/**** VTX_WORD2 ****/
#define SQ_VTX_WORD2_OFFSET_POS				0
#define SQ_VTX_WORD2_ENDIAN_SWAP_POS			16
#define SQ_VTX_WORD2_CONST_BUF_NO_STRIDE_POS		18
#define SQ_VTX_WORD2_MEGA_FETCH_POS			19
#define SQ_VTX_WORD2_ALT_CONST_POS			20
#define SQ_VTX_WORD2_BUF_INDEX_MODE_POS			21
#define SQ_VTX_WORD2_OFFSET_BITS			16
#define SQ_VTX_WORD2_ENDIAN_SWAP_BITS			2
#define SQ_VTX_WORD2_CONST_BUF_NO_STRIDE_BITS		1
#define SQ_VTX_WORD2_MEGA_FETCH_BITS			1
#define SQ_VTX_WORD2_ALT_CONST_BITS			1
#define SQ_VTX_WORD2_BUF_INDEX_MODE_BITS		2

#define CF_INDEX_NONE					0
#define CF_INDEX_0					1
#define CF_INDEX_1					2
#define CF_INVALID					3

/**** TEX_WORD0 ****/
#define SQ_TEX_WORD0_INST_POS				0
#define SQ_TEX_WORD0_INST_MOD_POS			5
#define SQ_TEX_WORD0_FETCH_WHOLE_QUAD_POS		7
#define SQ_TEX_WORD0_RSRC_ID_POS			8
#define SQ_TEX_WORD0_SRC_GPR_POS			16
#define SQ_TEX_WORD0_SRC_REL_POS			23
#define SQ_TEX_WORD0_ALT_CONST_POS			24
#define SQ_TEX_WORD0_RSRC_INDEX_MODE_POS		25
#define SQ_TEX_WORD0_SAMPLER_INDEX_MODE_POS		27
#define SQ_TEX_WORD0_INST_BITS				5
#define SQ_TEX_WORD0_INST_MOD_BITS			2
#define SQ_TEX_WORD0_FETCH_WHOLE_QUAD_BITS		1
#define SQ_TEX_WORD0_RSRC_ID_BITS			8
#define SQ_TEX_WORD0_SRC_GPR_BITS			7
#define SQ_TEX_WORD0_SRC_REL_BITS			1
#define SQ_TEX_WORD0_ALT_CONST_BITS			1
#define SQ_TEX_WORD0_RSRC_INDEX_MODE_BITS		2
#define SQ_TEX_WORD0_SAMPLER_INDEX_MODE_BITS		2

/**** TEX_WORD1 ****/
#define SQ_TEX_WORD1_DST_GPR_POS			0
#define SQ_TEX_WORD1_DST_REL_POS			7
#define SQ_TEX_WORD1_DST_SEL_X_POS			9
#define SQ_TEX_WORD1_DST_SEL_Y_POS			12
#define SQ_TEX_WORD1_DST_SEL_Z_POS			15
#define SQ_TEX_WORD1_DST_SEL_W_POS			18
#define SQ_TEX_WORD1_LOD_BIAS_POS			21
#define SQ_TEX_WORD1_COORD_TYPE_X_POS			28
#define SQ_TEX_WORD1_COORD_TYPE_Y_POS			29
#define SQ_TEX_WORD1_COORD_TYPE_Z_POS			30
#define SQ_TEX_WORD1_COORD_TYPE_W_POS			31
#define SQ_TEX_WORD1_DST_GPR_BITS			7
#define SQ_TEX_WORD1_DST_REL_BITS			1
#define SQ_TEX_WORD1_DST_SEL_X_BITS			3
#define SQ_TEX_WORD1_DST_SEL_Y_BITS			3
#define SQ_TEX_WORD1_DST_SEL_Z_BITS			3
#define SQ_TEX_WORD1_DST_SEL_W_BITS			3
#define SQ_TEX_WORD1_LOD_BIAS_BITS			7
#define SQ_TEX_WORD1_COORD_TYPE_X_BITS			1
#define SQ_TEX_WORD1_COORD_TYPE_Y_BITS			1
#define SQ_TEX_WORD1_COORD_TYPE_Z_BITS			1
#define SQ_TEX_WORD1_COORD_TYPE_W_BITS			1

/**** TEX_WORD2 ****/
#define SQ_TEX_WORD2_OFFSET_X_POS			0
#define SQ_TEX_WORD2_OFFSET_Y_POS			5
#define SQ_TEX_WORD2_OFFSET_Z_POS			10
#define SQ_TEX_WORD2_SAMPLER_ID_POS			15
#define SQ_TEX_WORD2_SRC_SEL_X_POS			20
#define SQ_TEX_WORD2_SRC_SEL_Y_POS			23
#define SQ_TEX_WORD2_SRC_SEL_Z_POS			26
#define SQ_TEX_WORD2_SRC_SEL_W_POS			29
#define SQ_TEX_WORD2_OFFSET_X_BITS			5
#define SQ_TEX_WORD2_OFFSET_Y_BITS			5
#define SQ_TEX_WORD2_OFFSET_Z_BITS			5
#define SQ_TEX_WORD2_SAMPLER_ID_BITS			5
#define SQ_TEX_WORD2_SRC_SEL_X_BITS			3
#define SQ_TEX_WORD2_SRC_SEL_Y_BITS			3
#define SQ_TEX_WORD2_SRC_SEL_Z_BITS			3
#define SQ_TEX_WORD2_SRC_SEL_W_BITS			3
#endif

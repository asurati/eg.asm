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

/* cc.[a,f,b(const),nb(const)] */
static
int inst_cf_parse_cc(struct inst_cf *this)
{
	int cf_const, err;
	struct inst_base *base;
	struct inst_all *all;

	all = container_of(this, struct inst_all, u.cf);
	base = &all->base;

	if (inst_base_is_next_token(base, ".") == false)
		return EINVAL;

	if (inst_base_is_next_token(base, "a"))
		this->w1.cond = CF_COND_ACTIVE;
	else if (inst_base_is_next_token(base, "f"))
		this->w1.cond = CF_COND_FALSE;
	else if (inst_base_is_next_token(base, "b"))
		this->w1.cond = CF_COND_BOOL;
	else if (inst_base_is_next_token(base, "nb"))
		this->w1.cond = CF_COND_NOT_BOOL;
	else
		return EINVAL;

	if (this->w1.cond == CF_COND_ACTIVE || this->w1.cond == CF_COND_FALSE)
		return 0;

	err = inst_base_parse_count(base, &cf_const);
	if (!err)
		this->w1.cf_const = cf_const;
	return err;
}

/* Mirror cf_parse */
static
int inst_cf_encode(struct inst_cf *this)
{
	int *w;
	struct inst_base *base;
	struct inst_all *all;

	all = container_of(this, struct inst_all, u.cf);
	base = &all->base;
	w = base->w;

	w[0] |= bits_set(CF_WORD0_ADDR, this->w0.addr);
	w[0] |= bits_set(CF_WORD0_JMP_TAB_SEL, this->w0.jump_table_sel);

	w[1] |= bits_set(CF_WORD1_POP_COUNT, this->w1.pop_count);
	w[1] |= bits_set(CF_WORD1_CONST, this->w1.cf_const);
	w[1] |= bits_set(CF_WORD1_COND, this->w1.cond);
	w[1] |= bits_set(CF_WORD1_COUNT, this->w1.count - 1);
	w[1] |= bits_set(CF_WORD1_VALID_PIXEL_MODE, this->w1.valid_pixel_mode);
	w[1] |= bits_set(CF_WORD1_END_OF_PROGRAM, this->w1.end_of_program);
	w[1] |= bits_set(CF_WORD1_INST, this->w1.cf_inst);
	w[1] |= bits_set(CF_WORD1_WHOLE_QUAD_MODE, this->w1.whole_quad_mode);
	w[1] |= bits_set(CF_WORD1_BARRIER, this->w1.barrier);
	return 0;
}

static
int inst_cf_parse(struct inst_cf *this, int code)
{
	int count, err;
	struct inst_base *base;
	struct inst_all *all;

	all = container_of(this, struct inst_all, u.cf);
	base = &all->base;

	this->w1.cf_inst = code;

	switch (code) {
	case CF_INST_VC:
	case CF_INST_TC:
		/* # of instructions in (%d) or (0x%x) */
		err = inst_base_parse_count(base, &count);
		if (err)
			return err;
		this->w1.count = count;	/* -1 when encoding. */
		break;
	}

	if (inst_base_is_next_token(base, "cc")) {
		err = inst_cf_parse_cc(this);
		if (err)
			return err;
	}

	switch (code) {
	case CF_INST_CALL_FS:
	case CF_INST_VC:
	case CF_INST_TC:
		/* label */
		this->w0.label = inst_base_get_next_token(base);
		break;
	}

	/* Flags and ; */
	for (;;) {
		if (inst_base_is_next_token(base, ";"))
			break;
next_flag:
		if (inst_base_is_next_token(base, "eop"))
			this->w1.end_of_program = 1;
		else if (inst_base_is_next_token(base, "vpm"))
			this->w1.valid_pixel_mode = 1;
		else if (inst_base_is_next_token(base, "wqm"))
			this->w1.whole_quad_mode = 1;
		else if (inst_base_is_next_token(base, "b"))
			this->w1.barrier = 1;
		else
			return EINVAL;

		if (inst_base_is_next_token(base, ","))
			goto next_flag;
	}
	return 0;
}

/* [base + indexgpr * size] */
static
int inst_cf_aie_parse_array_base(struct inst_base *base, int *out_arr_base,
				 int *out_ix_gpr, int *out_size)
{
	int err;

	*out_arr_base = *out_ix_gpr = 0;
	*out_size = 1;

	if (inst_base_is_next_token(base, "[") == false)
		return EINVAL;

	err = inst_base_parse_number(base, out_arr_base);
	if (err)
		return err;

	if (inst_base_is_next_token(base, "+") == false)
		goto done;

	err = inst_base_parse_register(base, out_ix_gpr);
	if (err)
		return err;

	if (inst_base_is_next_token(base, "*") == false)
		goto done;

	err = inst_base_parse_number(base, out_size);
	if (err)
		return err;
done:
	if (inst_base_is_next_token(base, "]") == false)
		return EINVAL;
	return 0;
}

static
int inst_cf_aie_swiz_encode(struct inst_cf_aie_swiz *this)
{
	int *w;
	struct inst_base *base;
	struct inst_all *all;

	all = container_of(this, struct inst_all, u.cf_aie_swiz);
	base = &all->base;
	w = base->w;

	w[0] |= bits_set(CF_AIE_WORD0_ARRAY_BASE, this->w0.array_base);
	w[0] |= bits_set(CF_AIE_WORD0_TYPE, this->w0.type);
	w[0] |= bits_set(CF_AIE_WORD0_RW_GPR, this->w0.rw_gpr);
	w[0] |= bits_set(CF_AIE_WORD0_RW_REL, this->w0.rw_rel);
	w[0] |= bits_set(CF_AIE_WORD0_INDEX_GPR, this->w0.index_gpr);
	w[0] |= bits_set(CF_AIE_WORD0_ELEM_SIZE, this->w0.elem_size - 1);

	w[1] |= bits_set(CF_AIE_WORD1_SWIZ_SEL_X, this->w1.sel_x);
	w[1] |= bits_set(CF_AIE_WORD1_SWIZ_SEL_Y, this->w1.sel_y);
	w[1] |= bits_set(CF_AIE_WORD1_SWIZ_SEL_Z, this->w1.sel_z);
	w[1] |= bits_set(CF_AIE_WORD1_SWIZ_SEL_W, this->w1.sel_w);

	w[1] |= bits_set(CF_AIE_WORD1_BURST_COUNT, this->w1.burst_count - 1);
	w[1] |= bits_set(CF_AIE_WORD1_VALID_PIXEL_MODE, this->w1.valid_pixel_mode);
	w[1] |= bits_set(CF_AIE_WORD1_END_OF_PROGRAM, this->w1.end_of_program);
	w[1] |= bits_set(CF_AIE_WORD1_INST, this->w1.cf_inst);
	w[1] |= bits_set(CF_AIE_WORD1_MARK, this->w1.mark);
	w[1] |= bits_set(CF_AIE_WORD1_BARRIER, this->w1.barrier);
	return 0;
}

static
int inst_cf_aie_swiz_parse(struct inst_cf_aie_swiz *this, int code)
{
	int count, swiz[4], err;
	struct inst_base *base;
	struct inst_all *all;
	bool is_pos, is_prm, is_pix;

	all = container_of(this, struct inst_all, u.cf_aie_swiz);
	base = &all->base;

	this->w1.cf_inst = code;

	if (inst_base_is_next_token(base, ".") == false)
		return EINVAL;

	/* swiz has only 3 */
	is_pos = is_pix = is_prm = false;
	if (inst_base_is_next_token(base, "pos"))
		is_pos = true;
	else if (inst_base_is_next_token(base, "prm"))
		is_prm = true;
	else if (inst_base_is_next_token(base, "pix"))
		is_pix = true;
	else
		return EINVAL;

	if (is_pos)
		this->w0.type = EXPORT_TYPE_POS;
	else if (is_prm)
		this->w0.type = EXPORT_TYPE_PARAM;
	else
		this->w0.type = EXPORT_TYPE_PIXEL;

	/* burst count */
	err = inst_base_parse_count(base, &count);
	if (err)
		return err;
	this->w1.burst_count = count;

	/* No cc in AIE */

	/* array base [base + index * elem_size] */
	err = inst_cf_aie_parse_array_base(base, &this->w0.array_base,
					   &this->w0.index_gpr,
					   &this->w0.elem_size);
	if (err)
		return err;
	
	/* Must be a comma */
	if (inst_base_is_next_token(base, ",") == false)
		return EINVAL;

	/* Must be a GPR. */
	err = inst_base_parse_register(base, &this->w0.rw_gpr);
	if (err)
		return err;
	
	swiz[SEL_X] = SEL_X;
	swiz[SEL_Y] = SEL_Y;
	swiz[SEL_Z] = SEL_Z;
	swiz[SEL_W] = SEL_W;

	/* Is there a .swizzle */
	if (inst_base_is_next_token(base, ".")) {
		err = inst_base_parse_swizzle(base, swiz);
		if (err)
			return err;
	}

	this->w1.sel_x = swiz[SEL_X];
	this->w1.sel_y = swiz[SEL_Y];
	this->w1.sel_z = swiz[SEL_Z];
	this->w1.sel_w = swiz[SEL_W];

	/* Flags and ; */
	for (;;) {
		if (inst_base_is_next_token(base, ";"))
			break;
next_flag:
		if (inst_base_is_next_token(base, "eop"))
			this->w1.end_of_program = 1;
		else if (inst_base_is_next_token(base, "vpm"))
			this->w1.valid_pixel_mode = 1;
		else if (inst_base_is_next_token(base, "rel"))
			this->w0.rw_rel = 1;
		else if (inst_base_is_next_token(base, "m"))
			this->w1.mark = 1;
		else if (inst_base_is_next_token(base, "b"))
			this->w1.barrier = 1;
		else
			return EINVAL;

		if (inst_base_is_next_token(base, ","))
			goto next_flag;
	}
	return 0;
}

/* kc#(bufid[addr],mode) */
static
int inst_cf_alu_parse_kcache(struct inst_cf_alu *this, int ix)
{
	int buf, mode, addr, err;
	struct inst_base *base;
	struct inst_all *all;

	all = container_of(this, struct inst_all, u.cf_alu);
	base = &all->base;

	if (inst_base_is_next_token(base, "(") == false)
		return EINVAL;

	err = inst_base_parse_number(base, &buf);
	if (err)
		return err;

	if (inst_base_is_next_token(base, "[") == false)
		return EINVAL;
	err = inst_base_parse_number(base, &addr);
	if (err)
		return err;
	if (inst_base_is_next_token(base, "]") == false)
		return EINVAL;

	if (inst_base_is_next_token(base, ",") == false)
		return EINVAL;

	if (inst_base_is_next_token(base, "nop"))
		mode = CF_KCACHE_MODE_NOP;
	else if (inst_base_is_next_token(base, "l1"))
		mode = CF_KCACHE_MODE_LOCK_1;
	else if (inst_base_is_next_token(base, "l2"))
		mode = CF_KCACHE_MODE_LOCK_2;
	else if (inst_base_is_next_token(base, "lli"))
		mode = CF_KCACHE_MODE_LOCK_LOOP_INDEX;
	else
		return EINVAL;

	if (inst_base_is_next_token(base, ")") == false)
		return EINVAL;

	if (ix == 0) {
		this->w0.kcache_bank0 = buf;
		this->w0.kcache_mode0 = mode;
		this->w1.kcache_addr0 = addr;
	} else if (ix == 1) {
		this->w0.kcache_bank1 = buf;
		this->w1.kcache_mode1 = mode;
		this->w1.kcache_addr1 = addr;
	} else if (ix == 2) {
		this->w2.kcache_bank2 = buf;
		this->w2.kcache_mode2 = mode;
		this->w3.kcache_addr2 = addr;
	} else if (ix == 3) {
		this->w2.kcache_bank3 = buf;
		this->w3.kcache_mode3 = mode;
		this->w3.kcache_addr3 = addr;
	} else {
		return EINVAL;
	}
	return 0;
}

static
int inst_cf_alu_encode(struct inst_cf_alu *this)
{
	int *w;
	struct inst_base *base;
	struct inst_all *all;

	all = container_of(this, struct inst_all, u.cf_alu);
	base = &all->base;
	w = base->w;
	assert(base->num_words == 2);	/* TODO CF_ALU_EXT */

	w[0] |= bits_set(CF_ALU_WORD0_ADDR, this->w0.addr);
	w[0] |= bits_set(CF_ALU_WORD0_KCACHE_BANK0, this->w0.kcache_bank0);
	w[0] |= bits_set(CF_ALU_WORD0_KCACHE_BANK1, this->w0.kcache_bank1);
	w[0] |= bits_set(CF_ALU_WORD0_KCACHE_MODE0, this->w0.kcache_mode0);

	w[1] |= bits_set(CF_ALU_WORD1_KCACHE_MODE1, this->w1.kcache_mode1);
	w[1] |= bits_set(CF_ALU_WORD1_KCACHE_ADDR0, this->w1.kcache_addr0);
	w[1] |= bits_set(CF_ALU_WORD1_KCACHE_ADDR1, this->w1.kcache_addr1);
	w[1] |= bits_set(CF_ALU_WORD1_COUNT, this->w1.count - 1);
	w[1] |= bits_set(CF_ALU_WORD1_ALT_CONST, this->w1.alt_const);
	w[1] |= bits_set(CF_ALU_WORD1_INST, this->w1.cf_inst);
	w[1] |= bits_set(CF_ALU_WORD1_WHOLE_QUAD_MODE, this->w1.whole_quad_mode);
	w[1] |= bits_set(CF_ALU_WORD1_BARRIER, this->w1.barrier);
	return 0;
}

static
int inst_cf_alu_parse(struct inst_cf_alu *this, int code)
{
	int err, count;
	struct inst_base *base;
	struct inst_all *all;

	all = container_of(this, struct inst_all, u.cf_alu);
	base = &all->base;

	this->w1.cf_inst = code;

	/* # of instructions in (%d) or (0x%x) */
	err = inst_base_parse_count(base, &count);
	if (err)
		return err;
	this->w1.count = count;	/* -1 when encoding. */

	/* KC0 KC1 */
	if (inst_base_is_next_token(base, "kc0")) {
		err = inst_cf_alu_parse_kcache(this, 0);
		if (err)
			return err;
	}

	if (inst_base_is_next_token(base, "kc1")) {
		err = inst_cf_alu_parse_kcache(this, 1);
		if (err)
			return err;
	}

	/* Label */
	this->w0.label = inst_base_get_next_token(base);

	/* Flags and ; */
	for (;;) {
		if (inst_base_is_next_token(base, ";"))
			break;
next_flag:
		if (inst_base_is_next_token(base, "alt"))
			this->w1.alt_const = 1;
		else if (inst_base_is_next_token(base, "wqm"))
			this->w1.whole_quad_mode = 1;
		else if (inst_base_is_next_token(base, "b"))
			this->w1.barrier = 1;
		else
			return EINVAL;

		if (inst_base_is_next_token(base, ","))
			goto next_flag;
	}
	return 0;
}

int inst_cf_parse_all(struct inst_all *all)
{
	int err, code;
	struct inst_base *base;

	base = &all->base;

	if (inst_base_is_next_token(base, ".") == false)
		return EINVAL;

	/* Scan the code. Divide into cf, gws, alu, rat, buf, swiz */
	if (inst_base_is_next_token(base, "fs")) {
		base->type = IT_CF;
		code = CF_INST_CALL_FS;
	} else if (inst_base_is_next_token(base, "vc")) {
		base->type = IT_CF;
		code = CF_INST_VC;
	} else if (inst_base_is_next_token(base, "ret")) {
		base->type = IT_CF;
		code = CF_INST_RETURN;
	} else if (inst_base_is_next_token(base, "nop")) {
		base->type = IT_CF;
		code = CF_INST_NOP;
	} else if (inst_base_is_next_token(base, "xd")) {
		base->type = IT_CF_AIE_SWIZ;
		code = CF_INST_EXPORT_DONE;
	} else if (inst_base_is_next_token(base, "alu")) {
		base->type = IT_CF_ALU;
		code = CF_INST_ALU;
	}

	/* TODO: CF_ALU_EXT has 4 words */
	assert(base->type != IT_CF_ALU_EXT);
	base->num_words = 2;

	switch (base->type) {
	case IT_CF:
		err = inst_cf_parse(&all->u.cf, code);
		break;
	case IT_CF_AIE_SWIZ:
		err = inst_cf_aie_swiz_parse(&all->u.cf_aie_swiz, code);
		break;
	case IT_CF_ALU:
		err = inst_cf_alu_parse(&all->u.cf_alu, code);
		break;
	default:
		err = EINVAL;
		break;
	}
	return err;
}

int inst_cf_fix_labels_all(struct inst_all *all)
{
	int err;
	struct inst_base *base;
	const char *label;
	int *addr;

	/* only IT_CF, IT_CF_ALU and IT_CF_ALU_EXT have labels. */
	base = &all->base;
	label = NULL;

	switch (base->type) {
	case IT_CF:
		label = all->u.cf.w0.label;
		addr = &all->u.cf.w0.addr;
		break;
	case IT_CF_ALU:
		label = all->u.cf_alu.w0.label;
		addr = &all->u.cf_alu.w0.addr;
		break;
	case IT_CF_ALU_EXT:
		err = EINVAL;	/* TODO */
		break;
	default:
		err = 0;
		break;
	}

	if (label)
		err = inst_base_fix_label(base, label, addr);
	return err;
}

/* Mirror cf_parse_all */
int inst_cf_encode_all(struct inst_all *all)
{
	int err;
	struct inst_base *base;

	base = &all->base;

	switch (base->type) {
	case IT_CF:
		err = inst_cf_encode(&all->u.cf);
		break;
	case IT_CF_AIE_SWIZ:
		err = inst_cf_aie_swiz_encode(&all->u.cf_aie_swiz);
		break;
	case IT_CF_ALU:
		err = inst_cf_alu_encode(&all->u.cf_alu);
		break;
	default:
		err = EINVAL;
		break;
	}
	return err;
}

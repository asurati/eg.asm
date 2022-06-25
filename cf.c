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
	} else if (inst_base_is_next_token(base, "xd")) {
		base->type = IT_CF_AIE_SWIZ;
		code = CF_INST_EXPORT_DONE;
	}

	switch (base->type) {
	case IT_CF:
		err = inst_cf_parse(&all->u.cf, code);
		break;
	case IT_CF_AIE_SWIZ:
		err = inst_cf_aie_swiz_parse(&all->u.cf_aie_swiz, code);
		break;
	default:
		err = EINVAL;
		break;
	}
	return err;
}

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

int inst_tex_parse_all(struct inst_all *all)
{
	int err, code, swiz[4];
	struct inst_base *base;
	struct inst_tex *this;

	base = &all->base;
	this = &all->u.tex;

	if (inst_base_is_next_token(base, ".") == false)
		return EINVAL;

	if (inst_base_is_next_token(base, "samp")) {
		base->type = IT_TEX;
		code = TC_INST_SAMPLE;
	}

	if (base->type != IT_TEX)
		return EINVAL;

	this->w0.tex_inst = code;
	base->num_words = 4;

	/* dst gpr.swizzle */
	err = inst_base_parse_register(base, &this->w1.dst_gpr);
	if (err)
		return err;

	swiz[SEL_X] = SEL_X;
	swiz[SEL_Y] = SEL_Y;
	swiz[SEL_Z] = SEL_Z;
	swiz[SEL_W] = SEL_W;
	if (inst_base_is_next_token(base, ".")) {
		err = inst_base_parse_swizzle(base, swiz);
		if (err)
			return err;
	}
	this->w1.dst_sel_x = swiz[SEL_X];
	this->w1.dst_sel_y = swiz[SEL_Y];
	this->w1.dst_sel_z = swiz[SEL_Z];
	this->w1.dst_sel_w = swiz[SEL_W];

	if (inst_base_is_next_token(base, ",") == false)
		return EINVAL;

	/* Either ps or vs */
	if (inst_base_is_next_token(base, "ps") == false &&
	    inst_base_is_next_token(base, "vs") == false)
		return EINVAL;

	/* buffer */
	if (inst_base_is_next_token(base, "[") == false)
		return EINVAL;
	err = inst_base_parse_number(base, &this->w0.rsrc_id);
	if (err)
		return err;
	if (inst_base_is_next_token(base, "]") == false)
		return EINVAL;

	/* sampler */
	if (inst_base_is_next_token(base, "[") == false)
		return EINVAL;
	err = inst_base_parse_number(base, &this->w2.sampler_id);
	if (err)
		return err;
	if (inst_base_is_next_token(base, "]") == false)
		return EINVAL;

	/* address */
	if (inst_base_is_next_token(base, "[") == false)
		return EINVAL;
	err = inst_base_parse_register(base, &this->w0.src_gpr);
	if (err)
		return err;
	swiz[SEL_X] = SEL_X;
	swiz[SEL_Y] = SEL_Y;
	swiz[SEL_Z] = SEL_Z;
	swiz[SEL_W] = SEL_W;
	if (inst_base_is_next_token(base, ".")) {
		err = inst_base_parse_swizzle(base, swiz);
		if (err)
			return err;
	}
	this->w2.src_sel_x = swiz[SEL_X];
	this->w2.src_sel_y = swiz[SEL_Y];
	this->w2.src_sel_z = swiz[SEL_Z];
	this->w2.src_sel_w = swiz[SEL_W];
	if (inst_base_is_next_token(base, "]") == false)
		return EINVAL;

	/* x,y,z,bias */
	if (inst_base_is_next_token(base, "+")) {
		if (inst_base_is_next_token(base, "[") == false)
			return EINVAL;
		err = inst_base_parse_number(base, &this->w2.offset_x);
		if (err)
			return err;

		if (inst_base_is_next_token(base, ",") == false)
			return EINVAL;
		err = inst_base_parse_number(base, &this->w2.offset_y);
		if (err)
			return err;

		if (inst_base_is_next_token(base, ",") == false)
			return EINVAL;
		err = inst_base_parse_number(base, &this->w2.offset_z);
		if (err)
			return err;

		if (inst_base_is_next_token(base, ",") == false)
			return EINVAL;
		err = inst_base_parse_number(base, &this->w1.lod_bias);
		if (err)
			return err;
		if (inst_base_is_next_token(base, "]") == false)
			return EINVAL;
	}

	/* Flags and ; */
	for (;;) {
		if (inst_base_is_next_token(base, ";"))
			break;
next_flag:
		if (inst_base_is_next_token(base, "alt"))
			this->w0.alt_const = 1;
		else if (inst_base_is_next_token(base, "srel"))
			this->w0.src_rel = 1;
		else if (inst_base_is_next_token(base, "drel"))
			this->w1.dst_rel = 1;
		else if (inst_base_is_next_token(base, "fwq"))
			this->w0.fetch_whole_quad = 1;

		else if (inst_base_is_next_token(base, "rim0"))
			this->w0.rsrc_index_mode = 1;
		else if (inst_base_is_next_token(base, "rim1"))
			this->w0.rsrc_index_mode = 2;
		else if (inst_base_is_next_token(base, "sim0"))
			this->w0.sampler_index_mode = 1;
		else if (inst_base_is_next_token(base, "sim1"))
			this->w0.sampler_index_mode = 2;

		else if (inst_base_is_next_token(base, "xn"))
			this->w1.coord_type_x = 1;
		else if (inst_base_is_next_token(base, "yn"))
			this->w1.coord_type_y = 1;
		else if (inst_base_is_next_token(base, "zn"))
			this->w1.coord_type_z = 1;
		else if (inst_base_is_next_token(base, "wn"))
			this->w1.coord_type_w = 1;
		else
			return EINVAL;

		if (inst_base_is_next_token(base, ","))
			goto next_flag;
	}
	return 0;
}

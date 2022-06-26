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

int inst_vtx_encode_all(struct inst_all *all)
{
	int *w, t[2];
	struct inst_base *base;
	struct inst_vtx *this;

	this = &all->u.vtx;
	base = &all->base;
	w = base->w;
	t[0] = t[1] = 0;

	w[0] |= bits_set(VTX_WORD0_INST, this->w0.vc_inst);
	w[0] |= bits_set(VTX_WORD0_FETCH_TYPE, this->w0.fetch_type);
	w[0] |= bits_set(VTX_WORD0_FETCH_WHOLE_QUAD, this->w0.fetch_whole_quad);
	w[0] |= bits_set(VTX_WORD0_BUF_ID, this->w0.buffer_id);
	w[0] |= bits_set(VTX_WORD0_SRC_GPR, this->w0.src_gpr);
	w[0] |= bits_set(VTX_WORD0_SRC_REL, this->w0.src_rel);
	w[0] |= bits_set(VTX_WORD0_SRC_SEL_X, this->w0.src_sel_x);
	w[0] |= bits_set(VTX_WORD0_MEGA_FETCH_COUNT, this->w0.mega_fetch_count);

	t[0] |= bits_set(VTX_WORD1_SEM_ID, this->w1.sem_id);

	t[1] |= bits_set(VTX_WORD1_GPR_DST_GPR, this->w1.dst_gpr);
	t[1] |= bits_set(VTX_WORD1_GPR_DST_REL, this->w1.dst_rel);

	if (this->w0.vc_inst == VC_INST_SEMANTIC)
		w[1] |= t[0];
	else
		w[1] |= t[1];
	w[1] |= bits_set(VTX_WORD1_DST_SEL_X, this->w1.dst_sel_x);
	w[1] |= bits_set(VTX_WORD1_DST_SEL_Y, this->w1.dst_sel_y);
	w[1] |= bits_set(VTX_WORD1_DST_SEL_Z, this->w1.dst_sel_z);
	w[1] |= bits_set(VTX_WORD1_DST_SEL_W, this->w1.dst_sel_w);
	w[1] |= bits_set(VTX_WORD1_USE_CONST_FIELDS, this->w1.use_const_fields);
	w[1] |= bits_set(VTX_WORD1_DATA_FORMAT, this->w1.data_format);
	w[1] |= bits_set(VTX_WORD1_NUM_FORMAT_ALL, this->w1.num_format_all);
	w[1] |= bits_set(VTX_WORD1_FORMAT_COMP_ALL, this->w1.format_comp_all);
	w[1] |= bits_set(VTX_WORD1_SRF_MODE_ALL, this->w1.srf_mode_all);

	w[2] |= bits_set(VTX_WORD2_OFFSET, this->w2.offset);
	w[2] |= bits_set(VTX_WORD2_ENDIAN_SWAP, this->w2.endian_swap);
	w[2] |= bits_set(VTX_WORD2_CONST_BUF_NO_STRIDE, this->w2.const_buf_no_stride);
	w[2] |= bits_set(VTX_WORD2_MEGA_FETCH, this->w2.mega_fetch);
	w[2] |= bits_set(VTX_WORD2_ALT_CONST, this->w2.alt_const);
	w[2] |= bits_set(VTX_WORD2_BUF_INDEX_MODE, this->w2.buffer_index_mode);
	return 0;
}

int inst_vtx_parse_all(struct inst_all *all)
{
	int err, code, swiz[4];
	struct inst_base *base;
	struct inst_vtx *this;

	base = &all->base;
	this = &all->u.vtx;

	if (inst_base_is_next_token(base, ".") == false)
		return EINVAL;

	if (inst_base_is_next_token(base, "sem")) {
		base->type = IT_VTX_SEM;
		code = VC_INST_SEMANTIC;
	} else if (inst_base_is_next_token(base, "reg")) {
		base->type = IT_VTX_GPR;
		code = VC_INST_FETCH;
	} else {
		return EINVAL;
	}

	this->w0.vc_inst = code;
	base->num_words = 4;

	/* TODO assume fetch_type is vertex = 0 */

	/* sem: #. reg: r# */
	if (base->type == IT_VTX_SEM)
		err = inst_base_parse_number(base, &this->w1.sem_id);
	else
		err = inst_base_parse_register(base, &this->w1.dst_gpr);
	if (err)
		return err;

	if (inst_base_is_next_token(base, ",") == false)
		return EINVAL;

	if (inst_base_is_next_token(base, "flt3"))
		this->w1.data_format = FMT_32_32_32_FLOAT;
	else if (inst_base_is_next_token(base, "flt2"))
		this->w1.data_format = FMT_32_32_FLOAT;
	else
		return EINVAL;

	if (inst_base_is_next_token(base, ",") == false)
		return EINVAL;

	/*default is unsigned. - n/i/s */
	if (inst_base_is_next_token(base, "-"))
		this->w1.format_comp_all = FORMAT_COMP_SIGNED;

	if (inst_base_is_next_token(base, "n"))
		this->w1.num_format_all = NUM_FORMAT_NORM;
	else if (inst_base_is_next_token(base, "i"))
		this->w1.num_format_all = NUM_FORMAT_INT;
	else if (inst_base_is_next_token(base, "s"))
		this->w1.num_format_all = NUM_FORMAT_SCALED;
	else
		return EINVAL;

	if (inst_base_is_next_token(base, ",") == false)
		return EINVAL;

	/* base[id][offset] */
	if (inst_base_is_next_token(base, "fs") == false)
		return EINVAL;	/* TODO more bases. */

	if (inst_base_is_next_token(base, "[") == false)
		return EINVAL;
	err = inst_base_parse_number(base, &this->w0.buffer_id);
	if (err)
		return err;
	if (inst_base_is_next_token(base, "]") == false)
		return EINVAL;

	if (inst_base_is_next_token(base, "[") == false)
		return EINVAL;
	err = inst_base_parse_number(base, &this->w2.offset);
	if (err)
		return err;
	if (inst_base_is_next_token(base, "]") == false)
		return EINVAL;

	/* dst swizzle */
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

	/* addr register and channel. */
	err = inst_base_parse_register(base, &this->w0.src_gpr);
	if (err)
		return err;
	if (inst_base_is_next_token(base, ".")) {
		err = inst_base_parse_channel(base, &this->w0.src_sel_x);
		if (err)
			return err;
	}

	/* Flags and ; */
	for (;;) {
		if (inst_base_is_next_token(base, ";"))
			break;
next_flag:
		if (inst_base_is_next_token(base, "alt"))
			this->w2.alt_const = 1;
		else if (inst_base_is_next_token(base, "cbns"))
			this->w2.const_buf_no_stride = 1;
		else if (inst_base_is_next_token(base, "mf"))
			this->w2.mega_fetch = 1;
		else if (inst_base_is_next_token(base, "ucf"))
			this->w1.use_const_fields = 1;
		else if (inst_base_is_next_token(base, "sma"))
			this->w1.srf_mode_all = 1;
		else if (inst_base_is_next_token(base, "fwq"))
			this->w0.fetch_whole_quad = 1;
		else if (inst_base_is_next_token(base, "srel"))
			this->w0.src_rel = 1;
		else if (inst_base_is_next_token(base, "drel"))
			this->w1.dst_rel = 1;
		else
			return EINVAL;

		if (inst_base_is_next_token(base, ","))
			goto next_flag;
	}
	return 0;
}

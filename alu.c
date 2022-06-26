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

static
int inst_alu_parse_src(struct inst_alu *this, bool is_op2, int ix)
{
	int err, abs, neg, num, kc, chan, sel;
	struct inst_base *base;
	const char *t;
	struct inst_all *all;

	all = container_of(this, struct inst_all, u.alu);
	base = &all->base;

	abs = neg = num = kc = chan = 0;

	/* op2 has both abs and neg for each src +- */
	if (is_op2 && inst_base_is_next_token(base, "+"))
		abs = 1;
	/* op3 has only neg */
	if (inst_base_is_next_token(base, "-"))
		neg = 1;

	t = inst_base_get_next_token(base);
	if (t[0] == 'r')
		sel = 0;
	else if (t[0] == 'p')
		sel = 448;
	else if (t[0] == 'k')
		sel = 0;
	else
		return EINVAL;
	sscanf(&t[1], "%d", &num);
	if (t[0] == 'k') {
		switch (num) {
		case 0: sel = 159; break;
		case 1: sel = 191; break;
		case 2: sel = 287; break;
		case 3: sel = 319; break;
		default: return EINVAL;
		}

		if (inst_base_is_next_token(base, "[") == false)
			return EINVAL;
		err = inst_base_parse_number(base, &num);
		if (err)
			return err;
		if (inst_base_is_next_token(base, "]") == false)
			return EINVAL;
	}
	sel += num;

	/* Read channel if prsent */
	if (inst_base_is_next_token(base, ".")) {
		err = inst_base_parse_channel(base, &chan);
		if (err)
			return err;
	}

	switch (ix) {
	case 0:
		this->w0.src0_sel	= sel;
		this->w0.src0_chan	= chan;
		this->w0.src0_neg	= neg;
		this->w1.src0_abs	= abs;
		break;
	case 1:
		this->w0.src1_sel	= sel;
		this->w0.src1_chan	= chan;
		this->w0.src1_neg	= neg;
		this->w1.src1_abs	= abs;
		break;
	case 2:
		this->w1.src2_sel	= sel;
		this->w1.src2_chan	= chan;
		this->w1.src2_neg	= neg;
		break;
	default:
		return EINVAL;
	}
	return 0;
}

static
int inst_alu_encode(struct inst_alu *this, bool is_op2)
{
	int *w, t[2];
	struct inst_base *base;
	struct inst_all *all;

	all = container_of(this, struct inst_all, u.alu);
	base = &all->base;
	w = base->w;
	t[0] = t[1] = 0;

	w[0] |= bits_set(ALU_WORD0_SRC0_SEL, this->w0.src0_sel);
	w[0] |= bits_set(ALU_WORD0_SRC0_REL, this->w0.src0_rel);
	w[0] |= bits_set(ALU_WORD0_SRC0_CHAN, this->w0.src0_chan);
	w[0] |= bits_set(ALU_WORD0_SRC0_NEG, this->w0.src0_neg);
	w[0] |= bits_set(ALU_WORD0_SRC1_SEL, this->w0.src1_sel);
	w[0] |= bits_set(ALU_WORD0_SRC1_REL, this->w0.src1_rel);
	w[0] |= bits_set(ALU_WORD0_SRC1_CHAN, this->w0.src1_chan);
	w[0] |= bits_set(ALU_WORD0_SRC1_NEG, this->w0.src1_neg);
	w[0] |= bits_set(ALU_WORD0_INDEX_MODE, this->w0.index_mode);
	w[0] |= bits_set(ALU_WORD0_PRED_SEL, this->w0.pred_sel);
	w[0] |= bits_set(ALU_WORD0_LAST, this->w0.last);

	/* Only in op2 */
	t[0] |= bits_set(ALU_WORD1_OP2_SRC0_ABS, this->w1.src0_abs);
	t[0] |= bits_set(ALU_WORD1_OP2_SRC1_ABS, this->w1.src1_abs);
	t[0] |= bits_set(ALU_WORD1_OP2_UPDATE_EXEC_MASK, this->w1.update_exec_mask);
	t[0] |= bits_set(ALU_WORD1_OP2_UPDATE_PRED, this->w1.update_pred);
	t[0] |= bits_set(ALU_WORD1_OP2_WRITE_ENABLE, this->w1.write_enable);
	t[0] |= bits_set(ALU_WORD1_OP2_OUT_MOD, this->w1.omod);

	/* Only in op3 */
	t[1] |= bits_set(ALU_WORD1_OP3_SRC2_SEL, this->w1.src2_sel);
	t[1] |= bits_set(ALU_WORD1_OP3_SRC2_REL, this->w1.src2_rel);
	t[1] |= bits_set(ALU_WORD1_OP3_SRC2_CHAN, this->w1.src2_chan);
	t[1] |= bits_set(ALU_WORD1_OP3_SRC2_NEG, this->w1.src2_neg);

	if (is_op2)
		w[1] |= t[0];
	else
		w[1] |= t[1];

	w[1] |= bits_set(ALU_WORD1_INST, this->w1.alu_inst);
	w[1] |= bits_set(ALU_WORD1_BANK_SWIZZLE, this->w1.bank_swizzle);
	w[1] |= bits_set(ALU_WORD1_DST_GPR, this->w1.dst_gpr);
	w[1] |= bits_set(ALU_WORD1_DST_REL, this->w1.dst_rel);
	w[1] |= bits_set(ALU_WORD1_DST_CHAN, this->w1.dst_chan);
	w[1] |= bits_set(ALU_WORD1_CLAMP, this->w1.clamp);
	return 0;
}

static
int inst_alu_parse(struct inst_alu *this, int code, bool is_op2)
{
	int err;
	struct inst_base *base;
	struct inst_all *all;

	all = container_of(this, struct inst_all, u.alu);
	base = &all->base;

	this->w1.alu_inst = code;

	/* Destination: register or - */
	if (inst_base_is_next_token(base, "-") == false) {
		err = inst_base_parse_register(base, &this->w1.dst_gpr);
		if (err)
			return err;
		this->w1.write_enable = 1;
	}

	if (inst_base_is_next_token(base, ".") == false)
		return EINVAL;

	err = inst_base_parse_channel(base, &this->w1.dst_chan);
	if (err)
		return err;

	/* output modifiers, if any */
	if (is_op2) {
		if (inst_base_is_next_token(base, "*")) {
			if (inst_base_is_next_token(base, "2"))
				this->w1.omod = ALU_OMOD_M2;
			else if (inst_base_is_next_token(base, "4"))
				this->w1.omod = ALU_OMOD_M4;
			else
				return EINVAL;
		} else if (inst_base_is_next_token(base, "/")) {
			if (inst_base_is_next_token(base, "2"))
				this->w1.omod = ALU_OMOD_D2;
			else
				return EINVAL;
		}
	}

	if (inst_base_is_next_token(base, ",") == false)
		return EINVAL;

	err = inst_alu_parse_src(this, is_op2, 0);
	if (err)
		return err;

	if (inst_base_is_next_token(base, ",") == false)
		return EINVAL;

	err = inst_alu_parse_src(this, is_op2, 1);
	if (err)
		return err;

	if (!is_op2) {
		if (inst_base_is_next_token(base, ",") == false)
			return EINVAL;
		err = inst_alu_parse_src(this, is_op2, 2);
	}
	return err;
}

int inst_alu_parse_all(struct inst_all *all)
{
	int err, code;
	struct inst_base *base;
	struct inst_alu *this;

	base = &all->base;
	this = &all->u.alu;
	code = -1;

	if (inst_base_is_next_token(base, ".") == false)
		return EINVAL;

	if (inst_base_is_next_token(base, "iz")) {
		base->type = IT_ALU_OP2;
		code = ALU_INST_INTERP_Z;
	} else if (inst_base_is_next_token(base, "ixy")) {
		base->type = IT_ALU_OP2;
		code = ALU_INST_INTERP_XY;
	}

	base->num_words = 2;

	switch (base->type) {
	case IT_ALU_OP2:
		err = inst_alu_parse(&all->u.alu, code, true);
		break;
	case IT_ALU_OP3:
		err = inst_alu_parse(&all->u.alu, code, false);
		break;
	default:
		err = EINVAL;
		break;
	}

	if (err)
		return err;

	/* Flags and ; */

	/*
	 * ps0, ps1, last, v210..., iml, img, imga, uem, up, clamp,
	 * s0rel, s1rel, s2rel, drel
	 */
	for (;;) {
		if (inst_base_is_next_token(base, ";"))
			break;
next_flag:
		if (inst_base_is_next_token(base, "ps0"))
			this->w0.pred_sel = 2;
		else if (inst_base_is_next_token(base, "ps1"))
			this->w0.pred_sel = 3;
		else if (inst_base_is_next_token(base, "last"))
			this->w0.last = 1;
		else if (inst_base_is_next_token(base, "iml"))
			this->w0.index_mode = 4;
		else if (inst_base_is_next_token(base, "img"))
			this->w0.index_mode = 5;
		else if (inst_base_is_next_token(base, "imga"))
			this->w0.index_mode = 6;

		else if (inst_base_is_next_token(base, "uem"))
			this->w1.update_exec_mask = 1;
		else if (inst_base_is_next_token(base, "up"))
			this->w1.update_pred = 1;

		/* Do not specify v012 and s210 in asm source */
		else if (inst_base_is_next_token(base, "021"))
			this->w1.bank_swizzle = 1;
		else if (inst_base_is_next_token(base, "120"))
			this->w1.bank_swizzle = 2;
		else if (inst_base_is_next_token(base, "102"))
			this->w1.bank_swizzle = 3;
		else if (inst_base_is_next_token(base, "201"))
			this->w1.bank_swizzle = 4;
		else if (inst_base_is_next_token(base, "210"))
			this->w1.bank_swizzle = 5;

		else if (inst_base_is_next_token(base, "122"))
			this->w1.bank_swizzle = 1;
		else if (inst_base_is_next_token(base, "212"))
			this->w1.bank_swizzle = 2;
		else if (inst_base_is_next_token(base, "221"))
			this->w1.bank_swizzle = 3;
		else
			return EINVAL;

		if (inst_base_is_next_token(base, ","))
			goto next_flag;
	}
	return err;
}

int inst_alu_encode_all(struct inst_all *all)
{
	int err;
	struct inst_base *base;

	base = &all->base;
	switch (base->type) {
	case IT_ALU_OP2:
		err = inst_alu_encode(&all->u.alu, true);
		break;
	case IT_ALU_OP3:
		err = inst_alu_encode(&all->u.alu, false);
		break;
	default:
		err = EINVAL;
		break;
	}
	return err;
}

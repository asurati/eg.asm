/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2022 Amol Surati */

#ifndef ALU_H
#define ALU_H

/**** ALU_WORD0 ****/
#define ALU_WORD0_SRC0_SEL_POS				0
#define ALU_WORD0_SRC0_REL_POS				9
#define ALU_WORD0_SRC0_CHAN_POS				10
#define ALU_WORD0_SRC0_NEG_POS				12
#define ALU_WORD0_SRC1_SEL_POS				13
#define ALU_WORD0_SRC1_REL_POS				22
#define ALU_WORD0_SRC1_CHAN_POS				23
#define ALU_WORD0_SRC1_NEG_POS				25
#define ALU_WORD0_INDEX_MODE_POS			26
#define ALU_WORD0_PRED_SEL_POS				29
#define ALU_WORD0_LAST_POS				31
#define ALU_WORD0_SRC0_SEL_BITS				9
#define ALU_WORD0_SRC0_REL_BITS				1
#define ALU_WORD0_SRC0_CHAN_BITS			2
#define ALU_WORD0_SRC0_NEG_BITS				1
#define ALU_WORD0_SRC1_SEL_BITS				9
#define ALU_WORD0_SRC1_REL_BITS				1
#define ALU_WORD0_SRC1_CHAN_BITS			2
#define ALU_WORD0_SRC1_NEG_BITS				1
#define ALU_WORD0_INDEX_MODE_BITS			3
#define ALU_WORD0_PRED_SEL_BITS				2
#define ALU_WORD0_LAST_BITS				1

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
#define ALU_WORD1_OP2_SRC0_ABS_POS			0
#define ALU_WORD1_OP2_SRC1_ABS_POS			1
#define ALU_WORD1_OP2_UPDATE_EXEC_MASK_POS		2
#define ALU_WORD1_OP2_UPDATE_PRED_POS			3
#define ALU_WORD1_OP2_WRITE_DST_POS			4
#define ALU_WORD1_OP2_OUT_MOD_POS			5
#define ALU_WORD1_OP2_SRC0_ABS_BITS			1
#define ALU_WORD1_OP2_SRC1_ABS_BITS			1
#define ALU_WORD1_OP2_UPDATE_EXEC_MASK_BITS		1
#define ALU_WORD1_OP2_UPDATE_PRED_BITS			1
#define ALU_WORD1_OP2_WRITE_DST_BITS			1
#define ALU_WORD1_OP2_OUT_MOD_BITS			2

#define ALU_WORD1_INST_POS				7
#define ALU_WORD1_BANK_SWIZZLE_POS			18
#define ALU_WORD1_DST_GPR_POS				21
#define ALU_WORD1_DST_REL_POS				28
#define ALU_WORD1_DST_CHAN_POS				29
#define ALU_WORD1_CLAMP_POS				31
#define ALU_WORD1_INST_BITS				11
#define ALU_WORD1_BANK_SWIZZLE_BITS			3
#define ALU_WORD1_DST_GPR_BITS				7
#define ALU_WORD1_DST_REL_BITS				1
#define ALU_WORD1_DST_CHAN_BITS				2
#define ALU_WORD1_CLAMP_BITS				1

#define ALU_INST_INTERP_XY				214
#define ALU_INST_INTERP_ZW				215
#define ALU_INST_INTERP_Z				217

#define ALU_OMOD_OFF					0
#define ALU_OMOD_M2					1
#define ALU_OMOD_M4					2
#define ALU_OMOD_D2					3

/**** ALU_WORD1_OP3 ****/
/* TODO */

struct inst_alu_w0 {
	int				src0_sel;
	int				src0_rel;
	int				src0_chan;
	int				src0_neg;
	int				src1_sel;
	int				src1_rel;
	int				src1_chan;
	int				src1_neg;
	int				index_mode;
	int				pred_sel;
	int				last;
};

struct inst_alu_w1 {
	/* Only in op2 */
	int				src0_abs;
	int				src1_abs;
	int				update_exec_mask;
	int				update_pred;
	int				write_enable;
	int				omod;

	/* Only in op3. */
	int				src2_sel;
	int				src2_rel;
	int				src2_chan;
	int				src2_neg;

	/* In op2 and op3 */
	int				alu_inst;
	int				bank_swizzle;
	int				dst_gpr;
	int				dst_rel;
	int				dst_chan;
	int				clamp;
};

/* Instructions */
struct inst_alu {
	struct inst_alu_w0		w0;
	struct inst_alu_w1		w1;
};
#endif

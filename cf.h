/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2022 Amol Surati */

#ifndef CF_H
#define CF_H

/**** CF_WORD0 ****/
#define CF_WORD0_ADDR_POS				0
#define CF_WORD0_JMP_TAB_SEL_POS			24
#define CF_WORD0_ADDR_BITS				24
#define CF_WORD0_JMP_TAB_SEL_BITS			3

#define CF_JTS_CONST_A					0
#define CF_JTS_CONST_B					1
#define CF_JTS_CONST_C					2
#define CF_JTS_CONST_D					3
#define CF_JTS_INDEX_0					4
#define CF_JTS_INDEX_1					5

/**** CF_GWS_WORD0 ****/
#define CF_GWS_WORD0_VALUE_POS				0
#define CF_GWS_WORD0_RSRC_POS				16
#define CF_GWS_WORD0_SIGN_POS				25
#define CF_GWS_WORD0_VALUE_INDEX_MODE_POS		26
#define CF_GWS_WORD0_RSRC_INDEX_MODE_POS		28
#define CF_GWS_WORD0_INST_POS				30
#define CF_GWS_WORD0_VALUE_BITS				10
#define CF_GWS_WORD0_RSRC_BITS				5
#define CF_GWS_WORD0_SIGN_BITS				1
#define CF_GWS_WORD0_VALUE_INDEX_MODE_BITS		2
#define CF_GWS_WORD0_RSRC_INDEX_MODE_BITS		2
#define CF_GWS_WORD0_INST_BITS				2

/**** CF_WORD1 ****/
#define CF_WORD1_POP_COUNT_POS				0
#define CF_WORD1_CONST_POS				3
#define CF_WORD1_COND_POS				8
#define CF_WORD1_COUNT_POS				10
#define CF_WORD1_VALID_PIXEL_MODE_POS			20
#define CF_WORD1_END_OF_PROGRAM_POS			21
#define CF_WORD1_INST_POS				22
#define CF_WORD1_WHOLE_QUAD_MODE_POS			30
#define CF_WORD1_BARRIER_POS				31
#define CF_WORD1_POP_COUNT_BITS				3
#define CF_WORD1_CONST_BITS				5
#define CF_WORD1_COND_BITS				2
#define CF_WORD1_COUNT_BITS				6
#define CF_WORD1_VALID_PIXEL_MODE_BITS			1
#define CF_WORD1_END_OF_PROGRAM_BITS			1
#define CF_WORD1_INST_BITS				8
#define CF_WORD1_WHOLE_QUAD_MODE_BITS			1
#define CF_WORD1_BARRIER_BITS				1

#define CF_COND_ACTIVE					0
#define CF_COND_FALSE					1
#define CF_COND_BOOL					2
#define CF_COND_NOT_BOOL				3

#define CF_INST_NOP					0
#define CF_INST_TC					1
#define CF_INST_VC					2
#define CF_INST_CALL					18
#define CF_INST_CALL_FS					19
#define CF_INST_RETURN					20
#define CF_INST_HALT					31

/**** CF_ALU_WORD0 ****/
#define CF_ALU_WORD0_ADDR_POS				0
#define CF_ALU_WORD0_KCACHE_BANK0_POS			22
#define CF_ALU_WORD0_KCACHE_BANK1_POS			26
#define CF_ALU_WORD0_KCACHE_MODE0_POS			30
#define CF_ALU_WORD0_ADDR_BITS				22
#define CF_ALU_WORD0_KCACHE_BANK0_BITS			4
#define CF_ALU_WORD0_KCACHE_BANK1_BITS			4
#define CF_ALU_WORD0_KCACHE_MODE0_BITS			2

#define CF_KCACHE_MODE_NOP				0
#define CF_KCACHE_MODE_LOCK_1				1
#define CF_KCACHE_MODE_LOCK_2				2
#define CF_KCACHE_MODE_LOCK_LOOP_INDEX			3

/**** CF_ALU_WORD1 ****/
#define CF_ALU_WORD1_KCACHE_MODE1_POS			0
#define CF_ALU_WORD1_KCACHE_ADDR0_POS			2
#define CF_ALU_WORD1_KCACHE_ADDR1_POS			10
#define CF_ALU_WORD1_COUNT_POS				18
#define CF_ALU_WORD1_ALT_CONST_POS			25
#define CF_ALU_WORD1_INST_POS				26
#define CF_ALU_WORD1_WHOLE_QUAD_MODE_POS		30
#define CF_ALU_WORD1_BARRIER_POS			31
#define CF_ALU_WORD1_KCACHE_MODE1_BITS			2
#define CF_ALU_WORD1_KCACHE_ADDR0_BITS			8
#define CF_ALU_WORD1_KCACHE_ADDR1_BITS			8
#define CF_ALU_WORD1_COUNT_BITS				7
#define CF_ALU_WORD1_ALT_CONST_BITS			1
#define CF_ALU_WORD1_INST_BITS				4
#define CF_ALU_WORD1_WHOLE_QUAD_MODE_BITS		1
#define CF_ALU_WORD1_BARRIER_BITS			1

/* val << 4, when considered relative to CF_WORD1.INST */
#define CF_INST_ALU					(8 << 4)
/* TODO: other INST */
#define CF_INST_ALU_ELSE_AFTER				(15 << 4)

/**** CF_{ALLOC,IMPORT,EXPORT}_WORD0 ****/
#define CF_AIE_WORD0_ARRAY_BASE_POS			0
#define CF_AIE_WORD0_TYPE_POS				13
#define CF_AIE_WORD0_RW_GPR_POS				15
#define CF_AIE_WORD0_RW_REL_POS				22
#define CF_AIE_WORD0_INDEX_GPR_POS			23
#define CF_AIE_WORD0_ELEM_SIZE_POS			30
#define CF_AIE_WORD0_ARRAY_BASE_BITS			13
#define CF_AIE_WORD0_TYPE_BITS				2
#define CF_AIE_WORD0_RW_GPR_BITS			7
#define CF_AIE_WORD0_RW_REL_BITS			1
#define CF_AIE_WORD0_INDEX_GPR_BITS			7
#define CF_AIE_WORD0_ELEM_SIZE_BITS			2

#define EXPORT_TYPE_PIXEL				0
#define EXPORT_TYPE_POS					1
#define EXPORT_TYPE_PARAM				2

/**** CF_{ALLOC,IMPORT,EXPORT}_WORD1_BUF ****/
#define CF_AIE_WORD1_BUF_ARRAY_SIZE_POS		0
#define CF_AIE_WORD1_BUF_COMP_MASK_POS		12
#define CF_AIE_WORD1_BUF_ARRAY_SIZE_BITS		12
#define CF_AIE_WORD1_BUF_COMP_MASK_BITS		4

/**** CF_{ALLOC,IMPORT,EXPORT}_WORD1_SWIZ ****/
#define CF_AIE_WORD1_SWIZ_SEL_X_POS			0
#define CF_AIE_WORD1_SWIZ_SEL_Y_POS			3
#define CF_AIE_WORD1_SWIZ_SEL_Z_POS			6
#define CF_AIE_WORD1_SWIZ_SEL_W_POS			9
#define CF_AIE_WORD1_SWIZ_SEL_X_BITS			3
#define CF_AIE_WORD1_SWIZ_SEL_Y_BITS			3
#define CF_AIE_WORD1_SWIZ_SEL_Z_BITS			3
#define CF_AIE_WORD1_SWIZ_SEL_W_BITS			3

/**** CF_{ALLOC,IMPORT,EXPORT}_WORD1 ****/
#define CF_AIE_WORD1_BURST_COUNT_POS			16
#define CF_AIE_WORD1_VALID_PIXEL_MODE_POS		20
#define CF_AIE_WORD1_END_OF_PROGRAM_POS		21
#define CF_AIE_WORD1_INST_POS			22
#define CF_AIE_WORD1_MARK_POS			30
#define CF_AIE_WORD1_BARRIER_POS			31
#define CF_AIE_WORD1_BURST_COUNT_BITS		4
#define CF_AIE_WORD1_VALID_PIXEL_MODE_BITS		1
#define CF_AIE_WORD1_END_OF_PROGRAM_BITS		1
#define CF_AIE_WORD1_INST_BITS			8
#define CF_AIE_WORD1_MARK_BITS			1
#define CF_AIE_WORD1_BARRIER_BITS			1

#define CF_INST_EXPORT					83
#define CF_INST_EXPORT_DONE				84

struct inst_cf_w0 {
	const char			*label;
	int				addr;
	int				jump_table_sel;
};

struct inst_cf_gws_w0 {
	int				value;
	int				rsrc;
	int				sign;
	int				value_index_mode;
	int				rsrc_index_mode;
	int				gws_opcode;
};

struct inst_cf_w1 {
	int				pop_count;
	int				cf_const;
	int				cond;
	int				count;
	int				valid_pixel_mode;
	int				end_of_program;
	int				cf_inst;
	int				whole_quad_mode;
	int				barrier;
};

struct inst_cf_alu_w0 {
	const char			*label;
	int				addr;
	int				kcache_bank0;
	int				kcache_bank1;
	int				kcache_mode0;
};

struct inst_cf_alu_w1 {
	int				kcache_mode1;
	int				kcache_addr0;
	int				kcache_addr1;
	int				count;
	int				alt_const;
	int				cf_inst;
	int				whole_quad_mode;
	int				barrier;
};

/* Also known as w0_ext */
struct inst_cf_alu_w2 {
	int				kcache_bank_index_mode0;
	int				kcache_bank_index_mode1;
	int				kcache_bank_index_mode2;
	int				kcache_bank_index_mode3;
	int				kcache_bank2;
	int				kcache_bank3;
	int				kcache_mode2;
};

/* Also known as w1_ext */
struct inst_cf_alu_w3 {
	int				kcache_mode3;
	int				kcache_addr2;
	int				kcache_addr3;
	int				cf_inst;
	int				barrier;
};

/* Works with both BUF and SWIZ */
struct inst_cf_aie_w0 {
	int				array_base;
	int				type;
	int				rw_gpr;
	int				rw_rel;
	int				index_gpr;
	int				elem_size;
};

/* RAT works with BUF */
struct inst_cf_aie_rat_w0 {
	int				rat_id;
	int				rat_inst;
	int				rat_index_mode;
	int				type;
	int				rw_gpr;
	int				rw_rel;
	int				index_gpr;
	int				elem_size;
};

struct inst_cf_aie_buf_w1 {
	int				array_size;
	int				comp_mask;
	int				burst_count;
	int				valid_pixel_mode;
	int				end_of_program;
	int				cf_inst;
	int				mark;
	int				barrier;
};

struct inst_cf_aie_swiz_w1 {
	int				sel_x;
	int				sel_y;
	int				sel_z;
	int				sel_w;
	int				burst_count;
	int				valid_pixel_mode;
	int				end_of_program;
	int				cf_inst;
	int				mark;
	int				barrier;
};

/* Instructions */
struct inst_cf {
	struct inst_cf_w0		w0;
	struct inst_cf_w1		w1;
};

struct inst_cf_gws {
	struct inst_cf_gws_w0		w0;
	struct inst_cf_w1		w1;
};

struct inst_cf_alu {
	struct inst_cf_alu_w0		w0;
	struct inst_cf_alu_w1		w1;
	struct inst_cf_alu_w2		w2;
	struct inst_cf_alu_w3		w3;
};

struct inst_cf_aie_rat {
	struct inst_cf_aie_rat_w0	w0;
	struct inst_cf_aie_buf_w1	w1;
};

struct inst_cf_aie_buf {
	struct inst_cf_aie_w0		w0;
	struct inst_cf_aie_buf_w1	w1;
};

struct inst_cf_aie_swiz {
	struct inst_cf_aie_w0		w0;
	struct inst_cf_aie_swiz_w1	w1;
};
#endif

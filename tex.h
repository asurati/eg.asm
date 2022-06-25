/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2022 Amol Surati */

#ifndef TEX_H
#define TEX_H

/**** TEX_WORD0 ****/
#define TEX_WORD0_INST_POS				0
#define TEX_WORD0_INST_MOD_POS				5
#define TEX_WORD0_FETCH_WHOLE_QUAD_POS			7
#define TEX_WORD0_RSRC_ID_POS				8
#define TEX_WORD0_SRC_GPR_POS				16
#define TEX_WORD0_SRC_REL_POS				23
#define TEX_WORD0_ALT_CONST_POS				24
#define TEX_WORD0_RSRC_INDEX_MODE_POS			25
#define TEX_WORD0_SAMPLER_INDEX_MODE_POS		27
#define TEX_WORD0_INST_BITS				5
#define TEX_WORD0_INST_MOD_BITS				2
#define TEX_WORD0_FETCH_WHOLE_QUAD_BITS			1
#define TEX_WORD0_RSRC_ID_BITS				8
#define TEX_WORD0_SRC_GPR_BITS				7
#define TEX_WORD0_SRC_REL_BITS				1
#define TEX_WORD0_ALT_CONST_BITS			1
#define TEX_WORD0_RSRC_INDEX_MODE_BITS			2
#define TEX_WORD0_SAMPLER_INDEX_MODE_BITS		2

/**** TEX_WORD1 ****/
#define TEX_WORD1_DST_GPR_POS				0
#define TEX_WORD1_DST_REL_POS				7
#define TEX_WORD1_DST_SEL_X_POS				9
#define TEX_WORD1_DST_SEL_Y_POS				12
#define TEX_WORD1_DST_SEL_Z_POS				15
#define TEX_WORD1_DST_SEL_W_POS				18
#define TEX_WORD1_LOD_BIAS_POS				21
#define TEX_WORD1_COORD_TYPE_X_POS			28
#define TEX_WORD1_COORD_TYPE_Y_POS			29
#define TEX_WORD1_COORD_TYPE_Z_POS			30
#define TEX_WORD1_COORD_TYPE_W_POS			31
#define TEX_WORD1_DST_GPR_BITS				7
#define TEX_WORD1_DST_REL_BITS				1
#define TEX_WORD1_DST_SEL_X_BITS			3
#define TEX_WORD1_DST_SEL_Y_BITS			3
#define TEX_WORD1_DST_SEL_Z_BITS			3
#define TEX_WORD1_DST_SEL_W_BITS			3
#define TEX_WORD1_LOD_BIAS_BITS				7
#define TEX_WORD1_COORD_TYPE_X_BITS			1
#define TEX_WORD1_COORD_TYPE_Y_BITS			1
#define TEX_WORD1_COORD_TYPE_Z_BITS			1
#define TEX_WORD1_COORD_TYPE_W_BITS			1

/**** TEX_WORD2 ****/
#define TEX_WORD2_OFFSET_X_POS				0
#define TEX_WORD2_OFFSET_Y_POS				5
#define TEX_WORD2_OFFSET_Z_POS				10
#define TEX_WORD2_SAMPLER_ID_POS			15
#define TEX_WORD2_SRC_SEL_X_POS				20
#define TEX_WORD2_SRC_SEL_Y_POS				23
#define TEX_WORD2_SRC_SEL_Z_POS				26
#define TEX_WORD2_SRC_SEL_W_POS				29
#define TEX_WORD2_OFFSET_X_BITS				5
#define TEX_WORD2_OFFSET_Y_BITS				5
#define TEX_WORD2_OFFSET_Z_BITS				5
#define TEX_WORD2_SAMPLER_ID_BITS			5
#define TEX_WORD2_SRC_SEL_X_BITS			3
#define TEX_WORD2_SRC_SEL_Y_BITS			3
#define TEX_WORD2_SRC_SEL_Z_BITS			3
#define TEX_WORD2_SRC_SEL_W_BITS			3

#define TC_INST_SAMPLE					16

struct inst_tex_w0 {
	int				tex_inst;
	int				inst_mod;
	int				fetch_whole_quad;
	int				rsrc_id;
	int				src_gpr;
	int				src_rel;
	int				alt_const;
	int				rsrc_index_mode;
	int				sampler_index_mode;
};

struct inst_tex_w1 {
	int				dst_gpr;
	int				dst_rel;
	int				dst_sel_x;
	int				dst_sel_y;
	int				dst_sel_z;
	int				dst_sel_w;
	int				lod_bias;
	int				coord_type_x;
	int				coord_type_y;
	int				coord_type_z;
	int				coord_type_w;
};

struct inst_tex_w2 {
	int				offset_x;
	int				offset_y;
	int				offset_z;
	int				sampler_id;
	int				src_sel_x;
	int				src_sel_y;
	int				src_sel_z;
	int				src_sel_w;
};

/* Instructions */
struct inst_tex {
	struct inst_tex_w0		w0;
	struct inst_tex_w1		w1;
	struct inst_tex_w2		w2;
};
#endif

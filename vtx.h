/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2022 Amol Surati */

#ifndef VTX_H
#define VTX_H

/**** VTX_WORD0 ****/
#define VTX_WORD0_INST_POS				0
#define VTX_WORD0_FETCH_TYPE_POS			5
#define VTX_WORD0_FETCH_WHOLE_QUAD_POS			7
#define VTX_WORD0_BUF_ID_POS				8
#define VTX_WORD0_SRC_GPR_POS				16
#define VTX_WORD0_SRC_REL_POS				23
#define VTX_WORD0_SRC_SEL_X_POS				24
#define VTX_WORD0_MEGA_FETCH_COUNT_POS			26
#define VTX_WORD0_INST_BITS				5
#define VTX_WORD0_FETCH_TYPE_BITS			2
#define VTX_WORD0_FETCH_WHOLE_QUAD_BITS			1
#define VTX_WORD0_BUF_ID_BITS				8
#define VTX_WORD0_SRC_GPR_BITS				7
#define VTX_WORD0_SRC_REL_BITS				1
#define VTX_WORD0_SRC_SEL_X_BITS			2
#define VTX_WORD0_MEGA_FETCH_COUNT_BITS			6

#define VC_INST_FETCH					0
#define VC_INST_SEMANTIC				1

/**** VTX_WORD1_GPR ****/
#define VTX_WORD1_GPR_DST_GPR_POS			0
#define VTX_WORD1_GPR_DST_REL_POS			7
#define VTX_WORD1_GPR_DST_GPR_BITS			7
#define VTX_WORD1_GPR_DST_REL_BITS			1

/**** VTX_WORD1_SEM ****/
#define VTX_WORD1_SEM_ID_POS				0
#define VTX_WORD1_SEM_ID_BITS				8

/**** VTX_WORD1 ****/
#define VTX_WORD1_DST_SEL_X_POS				9
#define VTX_WORD1_DST_SEL_Y_POS				12
#define VTX_WORD1_DST_SEL_Z_POS				15
#define VTX_WORD1_DST_SEL_W_POS				18
#define VTX_WORD1_USE_CONST_FIELDS_POS			21
#define VTX_WORD1_DATA_FORMAT_POS			22
#define VTX_WORD1_NUM_FORMAT_ALL_POS			28
#define VTX_WORD1_FORMAT_COMP_ALL_POS			30
#define VTX_WORD1_SRF_MODE_ALL_POS			31
#define VTX_WORD1_DST_SEL_X_BITS			3
#define VTX_WORD1_DST_SEL_Y_BITS			3
#define VTX_WORD1_DST_SEL_Z_BITS			3
#define VTX_WORD1_DST_SEL_W_BITS			3
#define VTX_WORD1_USE_CONST_FIELDS_BITS			1
#define VTX_WORD1_DATA_FORMAT_BITS			6
#define VTX_WORD1_NUM_FORMAT_ALL_BITS			2
#define VTX_WORD1_FORMAT_COMP_ALL_BITS			1
#define VTX_WORD1_SRF_MODE_ALL_BITS			1

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
#define VTX_WORD2_OFFSET_POS				0
#define VTX_WORD2_ENDIAN_SWAP_POS			16
#define VTX_WORD2_CONST_BUF_NO_STRIDE_POS		18
#define VTX_WORD2_MEGA_FETCH_POS			19
#define VTX_WORD2_ALT_CONST_POS				20
#define VTX_WORD2_BUF_INDEX_MODE_POS			21
#define VTX_WORD2_OFFSET_BITS				16
#define VTX_WORD2_ENDIAN_SWAP_BITS			2
#define VTX_WORD2_CONST_BUF_NO_STRIDE_BITS		1
#define VTX_WORD2_MEGA_FETCH_BITS			1
#define VTX_WORD2_ALT_CONST_BITS			1
#define VTX_WORD2_BUF_INDEX_MODE_BITS			2

struct inst_vtx_w0 {
	int				vc_inst;
	int				fetch_type;
	int				fetch_whole_quad;
	int				buffer_id;
	int				src_gpr;
	int				src_rel;
	int				src_sel_x;
	int				mega_fetch_count;
};

struct inst_vtx_w1 {
	int				sem_id;		/* only for vtx_sem */

	int				dst_gpr;	/* only for vtx_gpr */
	int				dst_rel;

	int				dst_sel_x;
	int				dst_sel_y;
	int				dst_sel_z;
	int				dst_sel_w;
	int				use_const_fields;
	int				data_format;
	int				num_format_all;
	int				format_comp_all;
	int				srf_mode_all;
};

struct inst_vtx_w2 {
	int				offset;
	int				endian_swap;
	int				const_buf_no_stride;
	int				mega_fetch;
	int				alt_const;
	int				buffer_index_mode;
};

/* Instructions */
struct inst_vtx {
	struct inst_vtx_w0		w0;
	struct inst_vtx_w1		w1;
	struct inst_vtx_w2		w2;
};
#endif

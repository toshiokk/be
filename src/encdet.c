/**************************************************************************
 *   encdet.c - character encoding determination                          *
 *                                                                        *
 *   Copyright (C) 2026-2026 PCR                                          *
 *                                                                        *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 2, or (at your option)  *
 *   any later version.                                                   *
 *                                                                        *
 *   This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program; if not, write to the Free Software          *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            *
 *                                                                        *
 **************************************************************************/

#include "headers.h"

// |Code  | Kanji                                | Half width Kana   |
// |------|--------------------------------------|-------------------|
// |ASCII | 0x07,09,0a,0c,0d,1b,0x20~7e,80~ff    | --                |
// |BINARY| Not ASCII                            | --                |
// |JIS   | 0x21,21 ~ 0x21,7e                    | --                |
// |      | 0x7e,21 ~ 0x7e,7e                    | --                |
// |      | Kanji-in:  0x1b, 0x24, 0x40          | --                |
// |      | Kanji-out: 0x1b, 0x28, 0x4a          | --                |
// |EUCJP | 0xa1,a1 ~ 0xa1,fe                    | 0x8e,a0 ~ 0x8e,df |
// |      | 0xfe,a1 ~ 0xfe,fe                    | --                |
// |SJIS  | 0x81,40 ~ 0x81,7e, 0x81,80 ~ 0x81,fc | 0xa0 ~ 0xdf       |
// |      | 0x9f,40 ~ 0x9f,7e, 0x9f,80 ~ 0x9f,fc | --                |
// |      | 0xe0,40 ~ 0xe0,7e, 0xe0,80 ~ 0xe0,fc | --                |
// |      | 0xfc,40 ~ 0xfc,7e, 0xfc,80 ~ 0xfc,fc | --                |
// | UTF8 | 0xc0,80                              | --                |
// |      | 0xe0,80,80                           | --                |
// |      | 0xf0,80,80,80                        | --                |
// |      | 0xf8,80,80,80,80                     | --                |
// |      | 0xfc,80,80,80,80,80                  | --                |

// Note:
// - ASCII-byte     : 0x07,09,0a,0c,0d,1b,0x20~7e,80~ff
// - anti-ASCII-byte: 0x00~06,08,0b,0e~1a,1c~1f,7f
// - corrupted Kanji-in/out sequences are such as 0x1b-24-3f
// - corrupted EUCJP        sequences are such as 0xa1,a0
// - corrupted SJIS         sequences are such as 0x81,3f
// - corrupted UTF8         sequences are such as 0xc0,7f

#define IS_BIN_BYTE(byte)					\
	(((0x00 <= byte) && (byte <= 0x06))		\
	 || (byte == 0x08) || (byte == 0x0b)	\
	 || ((0x0e <= byte) && (byte <= 0x1a))	\
	 || ((0x1c <= byte) && (byte <= 0x1f))	\
	 || (byte == 0x7f))						// 7 + 1 + 1 + 13 + 4 + 1 = 27
#define IS_BIN_BYTE_ASCII8(byte)		(IS_BIN_BYTE(byte) || (0x80 <= byte))
#define IS_ASCII7_BYTE(byte)			(! IS_BIN_BYTE_ASCII8(byte))

// |Code  | Positive observation               | Negative observation                     |
// |------|------------------------------------|------------------------------------------|
// |ASCII | does not contain 'anti-ASCII-byte' | contains         'anti-ASCII-byte'       |
// |BINARY| contains         'anti-ASCII-byte' | does not contain 'anti-ASCII-byte'       |
// |JIS   | contains Kanji-in/out sequence     | contains corrupted Kanji-in/out sequence |
// |EUCJP | contains EUCJP sequence            | contains corrupted EUCJP sequence        |
// |SJIS  | contains SJIS sequence             | contains corrupted SJIS sequence         |
// |UTF8  | contains UTF8 sequence             | contains corrupted UTF8 sequence         |

// |Code  | likeliness index                 |
// |------|----------------------------------|
// |ASCII | --                               | Not BINARY, JIS, EUCJP, SJIS nand UTF8
// |BINARY| (Pos-seq / Neg-seq) / (27 / 256) | > 10
// |JIS   | Pos-seq / Neg-seq                | > 10
// |EUCJP | Pos-seq / Neg-seq                | > 10
// |SJIS  | Pos-seq / Neg-seq                | > 10
// |UTF8  | Pos-seq / Neg-seq                | > 10

typedef enum {
	JIS_OFF		= 0,	// 0x1b
	JIS_1B		= 1,	// 0x24
	JIS_IN_24	= 2,	// 0x42
	JIS_OUT_28	= 3,	// 0x42
} jis_state_t;

typedef enum {
	EUC_0	= 0,
	EUC_8E	= 1,		// 0x8e,a0 ~ 0x8e,df
	EUC_A1	= 2,		// 0xa1,a1 ~ 0xfe,fe
} euc_state_t;

typedef enum {
	SJIS_0	= 0,
	SJIS_1	= 1,
} sjis_state_t;

#define BYTES_TO_BE_CHECKED			65536

// 10000: 100.00: appalently clear likeliness
//  1000:  10.00: relatively clear likeliness
//   100:   1.00: No         clear likeliness
//    10:   0.10: No               likeliness
//     1:   0.01: No               likeliness
//     0:   0.00: No               likeliness
typedef UINT32 likeliness_t;
#define MAX_LIKELINESS_INDEX	(999999999)

typedef struct encoding_stats encoding_stats_t;
struct encoding_stats {
	char state;				// guesser state
	char utf8c_bytes;		// UTF8 code length
	size_t bytes_checked;	// count of the bytes examined
	size_t bytes_matched;	// count of the bytes matching to the Positive observation
	size_t bytes_unmatched;	// count of the bytes matching to the Negative observation
	long match_weight;		// significance of the match
	long unmatch_weight;	// significance of the unmatch
	int (*check_byte)(encoding_stats_t *enc_stats, UCHAR byte);
	likeliness_t (*likeliness_index)(encoding_stats_t *enc_stats);	// 0 - MAX_LIKELINESS_INDEX
	const char *name;
};

PRIVATE int check_byte_ascii(encoding_stats_t *enc_stats, UCHAR byte)
{
	enc_stats->bytes_checked++;
	if (IS_ASCII7_BYTE(byte)) {
		enc_stats->bytes_matched++;
	} else {
		enc_stats->bytes_unmatched++;
	}
	return 0;
}
PRIVATE int check_byte_binary(encoding_stats_t *enc_stats, UCHAR byte)
{
	enc_stats->bytes_checked++;
	if (IS_BIN_BYTE(byte)) {
		enc_stats->bytes_matched++;
	} else {
		enc_stats->bytes_unmatched++;
	}
	return 0;
}
PRIVATE int check_byte_jis(encoding_stats_t *enc_stats, UCHAR byte)
{
	enc_stats->bytes_checked++;
	switch (enc_stats->state) {
	case JIS_OFF:
		switch (byte) {
		case 0x1b:
			enc_stats->state = JIS_1B;
			break;
		default:
			break;
		}
		break;
	case JIS_1B:
		switch (byte) {
		case 0x24:
			enc_stats->state = JIS_IN_24;
			break;
		case 0x28:
			enc_stats->state = JIS_OUT_28;
			break;
		default:
			enc_stats->state = JIS_OFF;
			enc_stats->bytes_unmatched += 2;
			return -2;
		}
		break;
	case JIS_IN_24:
		switch (byte) {
		case 0x42:
			enc_stats->state = JIS_OFF;
			enc_stats->bytes_matched += 3;
			return 3;
		default:
			enc_stats->state = JIS_OFF;
			enc_stats->bytes_unmatched += 3;
			return -3;
		}
		break;
	case JIS_OUT_28:
		switch (byte) {
		case 0x42:
			enc_stats->state = JIS_OFF;
			enc_stats->bytes_matched += 3;
			return 3;
		default:
			enc_stats->state = JIS_OFF;
			enc_stats->bytes_unmatched += 3;
			return -3;
		}
		break;
	}
	return 0;
}
PRIVATE int check_byte_eucjp(encoding_stats_t *enc_stats, UCHAR byte)
{
	enc_stats->bytes_checked++;
	switch (enc_stats->state) {
	case EUC_0:
		if (byte == 0x8e) {
			enc_stats->state = EUC_8E;
		} else if ((0xa1 <= byte) && (byte <= 0xfe)) {
			enc_stats->state = EUC_A1;
		} else {
			enc_stats->state = EUC_0;
		}
		break;
	case EUC_8E:
		if ((0xa0 <= byte) && (byte <= 0xdf)) {
			enc_stats->state = EUC_0;
			enc_stats->bytes_matched += 2;
			return 2;
		} else {
			enc_stats->state = EUC_0;
			enc_stats->bytes_unmatched += 2;
			return -2;
		}
		break;
	case EUC_A1:
		if ((0xa1 <= byte) && (byte <= 0xfe)) {
			enc_stats->state = EUC_0;
			enc_stats->bytes_matched += 2;
			return 2;
		} else {
			enc_stats->state = EUC_0;
			enc_stats->bytes_unmatched += 2;
			return -2;
		}
		break;
	}
	return 0;
}
PRIVATE int check_byte_sjis(encoding_stats_t *enc_stats, UCHAR byte)
{
	enc_stats->bytes_checked++;
	switch (enc_stats->state) {
	case SJIS_0:
		if (((0x81 <= byte) && (byte <= 0x9f)) || ((0xe0 <= byte) && (byte <= 0xfc))) {
			enc_stats->state = SJIS_1;
		} else {
			enc_stats->state = SJIS_0;
		}
		break;
	case SJIS_1:
		if (((0x40 <= byte) && (byte <= 0x7e)) || ((0x80 <= byte) && (byte <= 0xfc))) {
			enc_stats->state = SJIS_0;
			enc_stats->bytes_matched += 2;
			return 2;
		} else {
			enc_stats->state = SJIS_0;
			enc_stats->bytes_unmatched += 2;
			return -2;
		}
		break;
	}
	return 0;
}
PRIVATE int check_byte_utf8(encoding_stats_t *enc_stats, UCHAR byte)
{
	enc_stats->bytes_checked++;
	if (enc_stats->state == 0) {
		if (IS_UTF8_1ST_BYTE(byte)) {
			enc_stats->utf8c_bytes = UTF8_BYTES(byte);
			enc_stats->state = UTF8_TRAILING_BYTES(byte);
		}
	} else {
		if (IS_UTF8_TRAILING_BYTE(byte)) {
			enc_stats->state--;
			if (enc_stats->state == 0) {
				char matched_bytes = enc_stats->utf8c_bytes;
				enc_stats->bytes_matched += enc_stats->utf8c_bytes;
				enc_stats->utf8c_bytes = 0;
				return matched_bytes;
			}
		} else {
			enc_stats->state--;
			char unmatched_bytes = enc_stats->utf8c_bytes - enc_stats->state;
			enc_stats->bytes_unmatched += unmatched_bytes;
			enc_stats->state = 0;
			enc_stats->utf8c_bytes = 0;
			return -unmatched_bytes;
		}
	}
	return 0;
}

#define BYTES_BINARY		27		// anti-ASCII-byte: 0x00~06,08,0b,0e~1a,1c~1f,7f
#define BYTES_WHOLE			256
#define BYTES_NON_BINARY	(BYTES_WHOLE - BYTES_BINARY)
#define DIV_AVOIDING_DIV_BY_0(dividend, divisor)		((dividend) / LIM_MIN(1, (divisor)))
#define DIV_(dividend, divisor)							DIV_AVOIDING_DIV_BY_0(dividend, divisor)
PRIVATE likeliness_t likeliness_index_ascii(encoding_stats_t *enc_stats)
{
	likeliness_t likeliness = DIV_((long)enc_stats->bytes_matched * enc_stats->match_weight,
	 enc_stats->bytes_unmatched * enc_stats->unmatch_weight);
	return (likeliness_t)MIN_MAX_(0, likeliness, MAX_LIKELINESS_INDEX);
}
PRIVATE likeliness_t likeliness_index_binary(encoding_stats_t *enc_stats)
{
	likeliness_t likeliness = DIV_((long)enc_stats->bytes_matched * enc_stats->match_weight,
	 enc_stats->bytes_unmatched * enc_stats->unmatch_weight);
	return (likeliness_t)MIN_MAX_(0, likeliness, MAX_LIKELINESS_INDEX);
}
PRIVATE likeliness_t likeliness_index_jesu(encoding_stats_t *enc_stats)
{
	likeliness_t likeliness = DIV_((long)enc_stats->bytes_matched * enc_stats->match_weight,
	 enc_stats->bytes_unmatched * enc_stats->unmatch_weight);
	return (likeliness_t)MIN_MAX_(0, likeliness, MAX_LIKELINESS_INDEX);
}

PRIVATE encoding_stats_t encoding_stats[ENCDET_SUPPORTED_ENCODINGS] = {
	{ 0, 0, 0, 0, 0,    10, 10, check_byte_ascii , likeliness_index_ascii , "ENCDET_ASCII"  },
	{ 0, 0, 0, 0, 0,   100, 10, check_byte_utf8  , likeliness_index_jesu  , "ENCDET_UTF8"   },
	{ 0, 0, 0, 0, 0, 10000, 10, check_byte_binary, likeliness_index_binary, "ENCDET_BINARY" },
	{ 0, 0, 0, 0, 0, 30000,  1, check_byte_jis   , likeliness_index_jesu  , "ENCDET_JIS"    },
	{ 0, 0, 0, 0, 0,   100, 10, check_byte_eucjp , likeliness_index_jesu  , "ENCDET_EUCJP"  },
	{ 0, 0, 0, 0, 0,   100, 10, check_byte_sjis  , likeliness_index_jesu  , "ENCDET_SJIS"   },
};

PRIVATE void clear_encoding_stats()
{
	for (int enc_idx = 0; enc_idx < ENCDET_SUPPORTED_ENCODINGS; enc_idx++) {
		encoding_stats[enc_idx].state = 0;
		encoding_stats[enc_idx].utf8c_bytes = 0;
		encoding_stats[enc_idx].bytes_checked = 0;
		encoding_stats[enc_idx].bytes_matched = 0;
		encoding_stats[enc_idx].bytes_unmatched = 0;
	}
}
PRIVATE int get_encoding_most_likely()
{
	likeliness_t max_likeliness = 0;
	encoding_t enc = ENCDET_ASCII;
	for (int enc_idx = 0; enc_idx < ENCDET_SUPPORTED_ENCODINGS; enc_idx++) {
		likeliness_t likeliness = encoding_stats[enc_idx].likeliness_index(
		 &(encoding_stats[enc_idx]));
		if (likeliness > max_likeliness) {
			max_likeliness = likeliness;
			enc = enc_idx;
		}
	}
#ifdef ENABLE_DEBUG
////
flf_dprintf("encoding guessed: %d:[%s]\n", enc, encoding_stats[enc].name);
#endif // ENABLE_DEBUG
	return enc;
}
#ifdef ENABLE_DEBUG
PRIVATE void dump_encoding_stats()
{
	for (int enc_idx = 0; enc_idx < ENCDET_SUPPORTED_ENCODINGS; enc_idx++) {
		encoding_stats_t *enc_stats = &encoding_stats[enc_idx];
		d_printf("%2d:%-16s: %6d,%5d,%5d, %10d,%10d, %10d\n",
		 enc_idx, encoding_stats[enc_idx].name,
		 enc_stats->bytes_checked,
		 enc_stats->bytes_matched,
		 enc_stats->bytes_unmatched,
		 enc_stats->bytes_matched * enc_stats->match_weight,
		 enc_stats->bytes_unmatched * enc_stats->unmatch_weight,
		 (int)enc_stats->likeliness_index(&(encoding_stats[enc_idx])));
	}
}
#endif // ENABLE_DEBUG

int determine_encoding_file_path(const char *full_path)
{
	FILE *fp = fopen(full_path, "rb");
	if (fp == NULL) {
		return -1;
	}
	clear_encoding_stats();
	for (int bytes_chked = 0; bytes_chked < BYTES_TO_BE_CHECKED; ) {
		UCHAR bin_buf[BYTES_TO_BE_CHECKED];
		int bytes = fread(bin_buf, 1, BYTES_TO_BE_CHECKED, fp);
		if (bytes <= 0) {
			break;
		}
		for (int enc_idx = 0; enc_idx < ENCDET_SUPPORTED_ENCODINGS; enc_idx++) {
			for (int off = 0; off < bytes; off++) {
				encoding_stats[enc_idx].check_byte(&(encoding_stats[enc_idx]), bin_buf[off]);
			}
		}
		bytes_chked += bytes;
	}
#ifdef ENABLE_DEBUG
////
flf_dprintf("path:\n %s\n", full_path);
////
	dump_encoding_stats();
#endif // ENABLE_DEBUG

	fclose(fp);
	return get_encoding_most_likely();
}

// End of encdet.c

// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <locale.h>

#include "strlist.h"

#include "iter.h"

#define miku_jp_1 \
"初音ミクの魅力は、誰もがプロデュースできるシンガーであることです。 「初音ミク」のリ"
#define miku_jp_2 \
"リース直後から、アマチュア・プロを問わず、多くのミュージシャンが一斉に初音ミクが歌う"
#define miku_jp_3 \
"楽曲を動画投稿サイト「ニコニコ動画」や「Youtube」で発表しました。 すると、今度はその"
#define miku_jp_4 \
"曲に刺激されたイラストレーターが初音ミクの絵を描き、気に入った曲にPVをつける映像作家"
#define miku_jp_5 \
"も現れ・・・いつのまにかたくさんのクリエイターたちが初音ミクというバーチャルアイドル"
#define miku_jp_6 \
"の「プロデュース」を楽しむようになりました。"
#define miku_jp miku_jp_1 miku_jp_2 miku_jp_3 miku_jp_4 miku_jp_5 miku_jp_6

#define miku_en_1 \
"Hatsune Miku, sometimes called Miku Hatsune, officially code-named CV01, is a"
#define miku_en_2 \
"Vocaloid software voicebank developed by Crypton Future Media and its official"
#define miku_en_3 \
"mascot character, a sixteen-year-old girl with long, turquoise twintails."
#define miku_en miku_en_1 " " miku_en_2 " " miku_en_3

TESTDECL_BEGIN(setup)
{
	setlocale(LC_ALL, "C.UTF-8");
}

TESTDECL_ROUTINE(SL_INIT)
{
	struct strlist __cleanup(sl_destroy) sl = SL_INIT(sl);

	sl_push(&sl, XC("miku"));
	sl_push(&sl, XC("3939"));

	xchar __cleanup(__free) *res1 = sl_pop(&sl);
	xchar __cleanup(__free) *res2 = sl_pop(&sl);

	USSERT_NONNULL(res1);
	USSERT_NONNULL(res2);

	USSERT_STREQUAL(res1, XC("3939"));
	USSERT_STREQUAL(res2, XC("miku"));
}

TESTDECL_ROUTINE(sl_init)
{
	xchar *res;
	struct strlist sl;

	sl_init(&sl, 0);
	sl_push(&sl, XC("miku"));
	res = sl_pop(&sl);
	USSERT_NONNULL(res);
	sl_destroy(&sl);
	free(res);

	sl_init(&sl, SL_STORE_SBUF);
	sl_push(&sl, XC("miku"));
	res = sl_pop(&sl);
	USSERT_NONNULL(res);
	sl_destroy(&sl);

	sl_init(&sl, SL_STORE_REF);
	sl_push(&sl, XC("miku"));
	res = sl_pop(&sl);
	USSERT_NONNULL(res);
	sl_destroy(&sl);

	sl_init(&sl, SL_STORE_REF | SL_DUP_ON_POP);
	sl_push(&sl, XC("miku"));
	res = sl_pop(&sl);
	USSERT_NONNULL(res);
	sl_destroy(&sl);
	free(res);

	sl_init(&sl, SL_STORE_SBUF | SL_DUP_ON_POP);
	sl_push(&sl, XC("miku"));
	res = sl_pop(&sl);
	USSERT_NONNULL(res);
	sl_destroy(&sl);
	free(res);
}

TESTDECL_ROUTINE(sl_read_line)
{
	uint i;
	struct strlist __cleanup(sl_destroy) sl = SL_INIT(sl);
	xchar __cleanup(__free) *line = NULL;
	xchar *sample_jp[] = {
		[0] = XC(miku_jp_6),
		[1] = XC(miku_jp_5),
		[2] = XC(miku_jp_4),
		[3] = XC(miku_jp_3),
		[4] = XC(miku_jp_2),
		[5] = XC(miku_jp_1),
	};
	xchar *sample_en[] = {
		[0] = XC(miku_en_3),
		[1] = XC(miku_en_2),
		[2] = XC(miku_en_1),
	};

	sl_read_line(&sl, XC(miku_jp), 80);
	idx_for_each(i, sizeof_array(sample_jp)) {
		line = sl_pop_front(&sl);
		USSERT_NONNULL(line);

		USSERT_STREQUAL(line, sample_jp[i]);
		free(line);
	}

	sl_read_line(&sl, XC(miku_en), 80);
	idx_for_each(i, sizeof_array(sample_en)) {
		line = sl_pop_front(&sl);
		USSERT_NONNULL(line);

		USSERT_STREQUAL(line, sample_en[i]);
		free(line);
	}

	line = NULL;
}

TESTDECL_END();

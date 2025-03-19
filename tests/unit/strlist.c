// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <locale.h>

#include "iter.h"
#include "strlist.h"

#ifdef _WIN32
# undef __argv
#endif

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

UT_BEGIN(setup)
{
	setlocale(LC_ALL, "C.UTF-8");
}

UT_ROUTINE(sl__0)
{
	xchar __cleanup(__free) *res;
	struct strlist __cleanup(sl_destroy) sl;

	sl_init(&sl, 0);

	sl_push(&sl, XC("miku"));
	sl_push(&sl, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("3939"));
	free(res);

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("miku"));
	free(res);

	sl_push_back(&sl, XC("miku"));
	sl_push_back(&sl, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("miku"));
	free(res);

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("3939"));
	free(res);

	res = NULL;
}

UT_ROUTINE(sl__SL_STORE_SBUF)
{
	xchar *res;
	struct strlist __cleanup(sl_destroy) sl;

	sl_init(&sl, SL_STORE_SBUF);

	sl_push(&sl, XC("miku"));
	sl_push(&sl, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("miku"));

	sl_push_back(&sl, XC("miku"));
	sl_push_back(&sl, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("miku"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("3939"));
}

UT_ROUTINE(sl__SL_STORE_REF)
{
	xchar *res;
	struct strlist __cleanup(sl_destroy) sl;

	sl_init(&sl, SL_STORE_REF);

	sl_push(&sl, XC("miku"));
	sl_push(&sl, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("miku"));

	sl_push_back(&sl, XC("miku"));
	sl_push_back(&sl, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("miku"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("3939"));
}

UT_ROUTINE(sl__SL_STORE_CHR)
{
	char __cleanup(__free) *res;
	struct strlist __cleanup(sl_destroy) sl;

	sl_init(&sl, SL__STORE_CHR);

	sl_push_chr(&sl, "miku");
	sl_push_chr(&sl, "3939");

	res = sl_pop_chr(&sl);
	UA_NONNULL(res);
	UA_STREQ_MB(res, "3939");
	free(res);

	res = sl_pop_chr(&sl);
	UA_NONNULL(res);
	UA_STREQ_MB(res, "miku");
	free(res);

	sl_push_back_chr(&sl, "miku");
	sl_push_back_chr(&sl, "3939");

	res = sl_pop_chr(&sl);
	UA_NONNULL(res);
	UA_STREQ_MB(res, "miku");
	free(res);

	res = sl_pop_chr(&sl);
	UA_NONNULL(res);
	UA_STREQ_MB(res, "3939");
	free(res);

	res = NULL;
}

UT_ROUTINE(sl__SL_STORE_REF__SL_DUP_ON_POP)
{
	xchar __cleanup(__free) *res;
	struct strlist __cleanup(sl_destroy) sl;

	sl_init(&sl, SL_STORE_REF | SL_DUP_ON_POP);

	sl_push(&sl, XC("miku"));
	sl_push(&sl, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("3939"));
	free(res);

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("miku"));
	free(res);

	sl_push_back(&sl, XC("miku"));
	sl_push_back(&sl, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("miku"));
	free(res);

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("3939"));
	free(res);

	res = NULL;
}

UT_ROUTINE(sl__SL_STORE_SBUF__SL_DUP_ON_POP)
{
	xchar __cleanup(__free) *res;
	struct strlist __cleanup(sl_destroy) sl;

	sl_init(&sl, SL_STORE_SBUF | SL_DUP_ON_POP);

	sl_push(&sl, XC("miku"));
	sl_push(&sl, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("3939"));
	free(res);

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("miku"));
	free(res);

	sl_push_back(&sl, XC("miku"));
	sl_push_back(&sl, XC("3939"));

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("miku"));
	free(res);

	res = sl_pop(&sl);
	UA_NONNULL(res);
	UA_STREQ(res, XC("3939"));
	free(res);

	res = NULL;
}

UT_ROUTINE(sl_read_line)
{
	uint i;
	struct strlist __cleanup(sl_destroy) sl = SL_INIT(sl);
	xchar __cleanup(__free) *line = NULL;

	xchar *sample_jp[] = {
		[0] = XC(miku_jp_1),
		[1] = XC(miku_jp_2),
		[2] = XC(miku_jp_3),
		[3] = XC(miku_jp_4),
		[4] = XC(miku_jp_5),
		[5] = XC(miku_jp_6),
	};
	xchar *sample_en[] = {
		[0] = XC(miku_en_1),
		[1] = XC(miku_en_2),
		[2] = XC(miku_en_3),
	};

	sl_read_line(&sl, XC(miku_jp), 80);
	idx_for_each(i, sizeof_array(sample_jp)) {
		line = sl_pop(&sl);
		UA_NONNULL(line);

		UA_STREQ(line, sample_jp[i]);
		free(line);
	}

	sl_read_line(&sl, XC(miku_en), 80);
	idx_for_each(i, sizeof_array(sample_en)) {
		line = sl_pop(&sl);
		UA_NONNULL(line);

		UA_STREQ(line, sample_en[i]);
		free(line);
	}

	line = NULL;
}

UT_ROUTINE(sl_items)
{
	xchar __cleanup(__free) *res;
	struct strlist __cleanup(sl_destroy) sl = SL_INIT(sl);

	sl_push(&sl, XC("miku"));
	UA_EQ(sl.items, 1);

	sl_push(&sl, XC("3939"));
	UA_EQ(sl.items, 2);

	res = sl_pop(&sl);
	UA_EQ(sl.items, 1);
	free(res);

	res = sl_pop(&sl);
	UA_EQ(sl.items, 0);
	free(res);

	sl_push_back(&sl, XC("miku"));
	UA_EQ(sl.items, 1);

	sl_push_back(&sl, XC("3939"));
	UA_EQ(sl.items, 2);

	res = sl_pop(&sl);
	UA_EQ(sl.items, 1);
	free(res);

	res = sl_pop(&sl);
	UA_EQ(sl.items, 0);
	free(res);

	res = NULL;
}

UT_ROUTINE(sl_to_argv)
{
	struct strlist __cleanup(sl_destroy) sl;
	xchar *cmd[] = {
		XC("miku"),
		XC("run"),
		XC("dev"),
	};
	uint i;
	u32 __flags[] = {
		SL_STORE_COPY,
		SL_STORE_COPY | SL_DUP_ON_POP,
		SL_STORE_SBUF,
		SL_STORE_SBUF | SL_DUP_ON_POP,
		SL_STORE_REF,
		SL_STORE_REF | SL_DUP_ON_POP,
		0,
	};
	u32 *flags = __flags;

	while (*flags) {
		sl_init(&sl, *flags);

		idx_for_each(i, sizeof_array(cmd))
			sl_push_back(&sl, cmd[i]);

		xchar **__argv = sl_to_argv(&sl);
		xchar **argv = __argv;

		idx_for_each(i, sizeof_array(cmd)) {
			UA_NONNULL(argv[i]);
			UA_STREQ(argv[i], cmd[i]);
		}

		if (sl.flags & SL_DUP_ON_POP) {
			while (*argv)
				free(*argv++);
		}

		free(__argv);
		sl_destroy(&sl);
		flags++;
	}

	sl_init(&sl, 0);
}

UT_END();

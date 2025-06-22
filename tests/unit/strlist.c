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

static void __sl_free(struct strentry **item)
{
	if (!*item)
		return;

	sl_free(*item);
}

UT_BEGIN(setup)
{
	setlocale(LC_ALL, "C.UTF-8");
}

UT_ROUTINE(sl_mode_cp)
{
	struct strlist __cleanup(sl_destroy) sl;
	struct strentry __cleanup(__sl_free) *item;
	const xchar *str;

	sl_init(&sl, SL_MODE_CP);

	sl_push(&sl, XC("miku"));
	sl_push(&sl, XC("3939"));

	item = sl_pop(&sl);
	str = sl_str(item);

	UA_NONNULL(str);
	UA_STREQ(str, XC("3939"));
	sl_free(item);

	item = sl_pop(&sl);
	str = sl_str(item);

	UA_NONNULL(str);
	UA_STREQ(str, XC("miku"));
	sl_free(item);

	sl_push_back(&sl, XC("miku"));
	sl_push_back(&sl, XC("3939"));

	item = sl_pop(&sl);
	str = sl_str(item);

	UA_NONNULL(str);
	UA_STREQ(str, XC("miku"));
	sl_free(item);

	item = sl_pop(&sl);
	str = sl_str(item);

	UA_NONNULL(str);
	UA_STREQ(str, XC("3939"));
}

UT_ROUTINE(sl_mode_cache)
{
	struct strlist __cleanup(sl_destroy) sl;
	struct strentry __cleanup(__sl_free) *item;
	const xchar *str;

	u32 flags[] = {
		SL_MODE_MV,
		SL_MODE_SB,
	};
	uint i;

	for_each_idx(i, sizeof_array(flags)) {
		sl_init(&sl, flags[i]);

		sl_push(&sl, XC("miku"));
		sl_push(&sl, XC("3939"));

		item = sl_pop(&sl);
		str = sl_str(item);

		UA_NONNULL(str);
		UA_STREQ(str, XC("3939"));

		sl_put_idle(&sl, item);
		UA_PASS(list_is_first(&item->list, &sl.idle));

		item = sl_pop(&sl);
		str = sl_str(item);

		UA_NONNULL(str);
		UA_STREQ(str, XC("miku"));

		sl_put_idle(&sl, item);
		UA_PASS(list_is_first(&item->list, &sl.idle));

		item = NULL;

		sl_push_back(&sl, XC("miku"));
		sl_push_back(&sl, XC("3939"));

		UA_PASS(list_is_empty(&sl.idle));

		item = sl_pop(&sl);
		str = sl_str(item);

		UA_NONNULL(str);
		UA_STREQ(str, XC("miku"));
		sl_free(item);

		item = sl_pop(&sl);
		str = sl_str(item);

		UA_NONNULL(str);
		UA_STREQ(str, XC("3939"));

		if (i + 1 == sizeof_array(flags))
			break;

		sl_free(item);
		sl_destroy(&sl);
	}
}

UT_ROUTINE(sl_mode_mb)
{
	struct strlist __cleanup(sl_destroy) sl;
	struct strentry __cleanup(__sl_free) *item;
	const char *str;

	sl_init(&sl, SL_MODE_MB);

	sl_push_mb(&sl, "miku");
	sl_push_mb(&sl, "3939");

	item = sl_pop(&sl);
	str = sl_str_mb(item);

	UA_NONNULL(str);
	UA_STREQ_MB(str, "3939");
	sl_free(item);

	item = sl_pop(&sl);
	str = sl_str(item);

	UA_NONNULL(str);
	UA_STREQ_MB(str, "miku");
	sl_free(item);

	sl_push_back(&sl, "miku");
	sl_push_back(&sl, "3939");

	item = sl_pop(&sl);
	str = sl_str(item);

	UA_NONNULL(str);
	UA_STREQ_MB(str, "miku");
	sl_free(item);

	item = sl_pop(&sl);
	str = sl_str(item);

	UA_NONNULL(str);
	UA_STREQ_MB(str, "3939");
}

UT_ROUTINE(sl_items)
{
	struct strlist __cleanup(sl_destroy) sl = SL_INIT(sl);
	struct strentry __cleanup(__sl_free) *item;

	sl_push(&sl, XC("miku"));
	UA_EQ(sl.items, 1);

	sl_push(&sl, XC("3939"));
	UA_EQ(sl.items, 2);

	item = sl_pop(&sl);
	UA_EQ(sl.items, 1);
	sl_free(item);

	item = sl_pop(&sl);
	UA_EQ(sl.items, 0);
	sl_free(item);

	sl_push_back(&sl, XC("miku"));
	UA_EQ(sl.items, 1);

	sl_push_back(&sl, XC("3939"));
	UA_EQ(sl.items, 2);

	item = sl_pop(&sl);
	UA_EQ(sl.items, 1);
	sl_free(item);

	item = sl_pop(&sl);
	UA_EQ(sl.items, 0);
}

UT_ROUTINE(sl_to_argv)
{
	struct strlist __cleanup(sl_destroy) sl;
	xchar *cmd[] = {
		XC("miku"),
		XC("run"),
		XC("dev"),
	};

	u32 flags[] = {
		SL_MODE_CP,
		SL_MODE_SB,
		SL_MODE_MV,
	};
	uint i, j;

	for_each_idx(i, sizeof_array(flags)) {
		sl_init(&sl, flags[i]);

		for_each_idx(j, sizeof_array(cmd))
			sl_push_back(&sl, cmd[i]);

		xchar **__argv = sl_to_argv(&sl);
		xchar **argv = __argv;

		for_each_idx(j, sizeof_array(cmd)) {
			UA_NONNULL(argv[i]);
			UA_STREQ(argv[i], cmd[i]);
		}

		free_argv(__argv);
		sl_destroy(&sl);
	}

	sl_init(&sl, 0);
}

UT_ROUTINE(sl_read_line_en)
{
	struct strlist __cleanup(sl_destroy) sl = SL_INIT(sl);
	struct strentry __cleanup(__sl_free) *item;
	const xchar *str;

	xchar *lines[] = {
		[0] = XC(miku_en_1),
		[1] = XC(miku_en_2),
		[2] = XC(miku_en_3),
	};
	uint i;

	sl_read_line(&sl, XC(miku_en), 80);
	for_each_idx(i, sizeof_array(lines)) {
		item = sl_pop(&sl);
		str = sl_str(item);

		UA_NONNULL(str);
		UA_STREQ(str, lines[i]);
		sl_free(item);
	}

	item = NULL;
}

UT_ROUTINE(sl_read_line_jp)
{
	struct strlist __cleanup(sl_destroy) sl = SL_INIT(sl);
	struct strentry __cleanup(__sl_free) *item;
	const xchar *str;

	xchar *lines[] = {
		[0] = XC(miku_jp_1),
		[1] = XC(miku_jp_2),
		[2] = XC(miku_jp_3),
		[3] = XC(miku_jp_4),
		[4] = XC(miku_jp_5),
		[5] = XC(miku_jp_6),
	};
	uint i;

	sl_read_line(&sl, XC(miku_jp), 80);
	for_each_idx(i, sizeof_array(lines)) {
		item = sl_pop(&sl);
		str = sl_str(item);

		UA_NONNULL(str);
		UA_STREQ(str, lines[i]);
		sl_free(item);
	}

	item = NULL;
}

UT_END();

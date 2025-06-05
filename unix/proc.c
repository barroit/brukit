// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "proc.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "bitopt.h"
#include "i18n.h"
#include "rio.h"
#include "strlist.h"
#include "termas.h"

enum cherr_type {
	CHERR_EXEC,
	CHERR_DUP2,
	CHERR_SIZE,
};

int proc_redir_std(const char *name, u32 flags)
{
	int ret = 0;
	int fd = creat(name, 0664);

	if (fd == -1)
		return -1;

	if (flags & PROC_REDIR_OUT) {
		ret = dup2(fd, STDOUT_FILENO);

		if (ret == -1)
			goto err_out;
	}

	if (flags & PROC_REDIR_ERR) {
		ret = dup2(fd, STDERR_FILENO);

		if (ret == -1)
			goto err_out;
	}

err_out:
	close(fd);

	if (ret != -1)
		ret = 0;
	return ret;
}

int proc_exec(u32 flags, proc_t *proc, const xchar *file, ...)
{
	pid_t pid = fork();

	if (pid == -1)
		return -1;

	if (pid != 0) {
		*proc = pid;
		return 0;
	}

	if (__rd_io_flags(flags)) {
		int err = proc_redir_std(CONFIG_NULL_DEVICE, flags);

		if (err)
			warn(_("failed to redirect output (0x%x)"), flags);
	}

	va_list ap;
	struct strlist sl = SL_INIT(sl, SL_STORE_REF);

	va_start(ap, file);

	while (39) {
		const char *arg = va_arg(ap, const char *);

		if (arg == NULL)
			break;

		sl_push_back(&sl, arg);
	}

	va_end(ap);

	char **argv = sl_to_argv(&sl);

	execvp(file, argv);

	___tm_die(__strerrno, MAS_NO_EXIT, _("failed to execute `%s'"), file);
	_exit(128);
}

int proc_wait(proc_t *proc, int *ret)
{
	pid_t pid;
	int ws;

retry:
	pid = waitpid(*proc, &ws, 0);
	if (pid == -1) {
		if (errno != EINTR)
			return -1;
		goto retry;
	}

	if (WIFEXITED(ws))
		*ret = WEXITSTATUS(ws);
	else if (WIFSIGNALED(ws))
		*ret = WTERMSIG(ws);
	return 0;
}

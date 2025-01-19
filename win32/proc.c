// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "proc.h"

#include <io.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

#include "strbuf.h"
#include "termas.h"
#include "xchar.h"

int proc_rd_io(const xchar *name, u32 flags)
{
	int err = 0;
	SECURITY_ATTRIBUTES sec = {
		.nLength        = sizeof(sec),
		.bInheritHandle = 1,
	};
	HANDLE file = CreateFile(name, GENERIC_WRITE, FILE_SHARE_READ, &sec,
				 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE)
		return -1;

	int fd;
	int ret = _open_osfhandle((intptr_t)file, O_WRONLY);

	if (ret == -1)
		goto err_open_fd;
	fd = ret;

	if (flags & PROC_RD_STDOUT) {
		err = !SetStdHandle(STD_OUTPUT_HANDLE, file);
		if (err)
			goto err_rd_io;

		ret = dup2(fd, STDOUT_FILENO);
		if (ret == -1)
			goto err_rd_io;
	}

	if (flags & PROC_RD_STDERR) {
		err = !SetStdHandle(STD_ERROR_HANDLE, file);
		if (err)
			goto err_rd_io;

		ret = dup2(fd, STDERR_FILENO);
		if (ret == -1)
			goto err_rd_io;
	}

err_rd_io:
	close(fd);
err_open_fd:
	CloseHandle(file);

	if (ret != -1)
		ret = 0;
	if (err)
		ret = -1;
	return ret;
}

int proc_exec(u32 flags, proc_t *proc, const xchar *file, ...)
{
	va_list ap;
	struct strbuf sb = SB_INIT;

	va_start(ap, file);

	while (39) {
		const xchar *arg = va_arg(ap, xchar *);

		if (arg == NULL)
			break;

		const xchar *ws = xc_mbsws(arg);

		if (!ws)
			sb_puts(&sb, arg);
		else
			sb_printf(&sb, XC("'%s'"), arg);

		sb_putc(&sb, XC(' '));
	}

	va_end(ap);

	HANDLE nul;
	STARTUPINFO info = {
		.cb = sizeof(info),
	};

	if (__rd_io_flags(flags)) {
		SECURITY_ATTRIBUTES sec = {
			.nLength        = sizeof(sec),
			.bInheritHandle = 1,
		};

		nul = CreateFile(XC(CONFIG_NULL_DEVICE),
				 GENERIC_WRITE, 0, &sec,
				 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		BUG_ON(nul == INVALID_HANDLE_VALUE);

		info.dwFlags = STARTF_USESTDHANDLES;
		info.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

		if (flags & PROC_RD_STDOUT)
			info.hStdOutput = nul;
		else
			info.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

		if (flags & PROC_RD_STDERR)
			info.hStdError = nul;
		else
			info.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	}

	int err = !CreateProcess(file, sb.buf, NULL, NULL, TRUE,
				 0, NULL, NULL, &info, proc);
	int errnum = winerrno();

	sb_destroy(&sb);
	if (__rd_io_flags(flags))
		CloseHandle(nul);

	if (!err)
		return 0;

	errno = errnum;
	return -1;
}

int proc_wait(proc_t *proc, int *__ret)
{
	int err = 0;
	int errnum;
	DWORD code;
	DWORD res = WaitForSingleObject(proc->hProcess, INFINITE);

	switch (res) {
	case WAIT_OBJECT_0:
		GetExitCodeProcess(proc->hProcess, &code);
		*__ret = code;
		break;
	case WAIT_FAILED:
		err = 1;
		errnum = winerrno();
	}

	CloseHandle(proc->hProcess);
	CloseHandle(proc->hThread);

	if (!err)
		return 0;

	errno = errnum;
	return -1;
}

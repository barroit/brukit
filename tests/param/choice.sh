#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run --kdf scrypt
UA_OPTEQ kdf scrypt

run --kdf scrypt --kdf=argon2
UA_OPTEQ kdf argon2

run --kdf fake_kdf
UA_ERRLIKE 'valid choices are'

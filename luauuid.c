/*
 * Copyright (c) 2016 - 2026 Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Micro Systems Marc Balmer nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL MICRO SYSTEMS MARC BALMER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* UUID generation functions for Lua */

#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include <lua.h>
#include <lauxlib.h>

#include "luauuid.h"

static int
lua_uuid_generator(lua_State *L, void (*generator)(uuid_t))
{
	uuid_t uuid, *u;
	const char *format;
	char str[UUID_STR_SIZE + 1];

	generator(uuid);
	format = lua_tostring(L, 1);

	if (format && *format == 't') {
		uuid_unparse(uuid, str);
		lua_pushlstring(L, str, UUID_STR_SIZE);
	} else {
		u = (uuid_t *)lua_newuserdata(L, sizeof(uuid_t));
		uuid_copy(*u, uuid);
		luaL_setmetatable(L, UUID_METATABLE);
	}
	return 1;
}

static int
lua_uuid_generate(lua_State *L)
{
	return lua_uuid_generator(L, uuid_generate);
}

static int
lua_uuid_generate_random(lua_State *L)
{
	return lua_uuid_generator(L, uuid_generate_random);
}

static int
lua_uuid_generate_time(lua_State *L)
{
	return lua_uuid_generator(L, uuid_generate_time);
}

#ifdef UUID_TYPE_DCE_TIME_V6
static int
lua_uuid_generate_time_v6(lua_State *L)
{
	return lua_uuid_generator(L, uuid_generate_time_v6);
}
#endif

#ifdef UUID_TYPE_DCE_TIME_V7
static int
lua_uuid_generate_time_v7(lua_State *L)
{
	return lua_uuid_generator(L, uuid_generate_time_v7);
}
#endif

static int
lua_uuid_generate_time_safe(lua_State *L)
{
	uuid_t uuid, *u;
	const char *format;
	char str[UUID_STR_SIZE + 1];

	format = lua_tostring(L, 1);
	lua_pushboolean(L, uuid_generate_time_safe(uuid) == 0 ? 1 : 0);

	if (format && *format == 't') {
		uuid_unparse(uuid, str);
		lua_pushlstring(L, str, UUID_STR_SIZE);
	} else {
		u = (uuid_t *)lua_newuserdata(L, sizeof(uuid_t));
		uuid_copy(*u, uuid);
		luaL_setmetatable(L, UUID_METATABLE);
	}
	return 2;
}

static int
lua_uuid_parse(lua_State *L)
{
	uuid_t uuid, *u;

	if (!uuid_parse(luaL_checkstring(L, 1), uuid)) {
		u = (uuid_t *)lua_newuserdata(L, sizeof(uuid_t));
		uuid_copy(*u, uuid);
		luaL_setmetatable(L, UUID_METATABLE);
	} else
		lua_pushnil(L);
	return 1;
}

static int
lua_uuid_create(lua_State *L)
{
	uuid_t uuid, *u;
	size_t len;
	const char *buf;

	buf = luaL_checklstring(L, 1, &len);
	if (len != sizeof(uuid_t))
		return luaL_error(L, "uuid.create expects string of %ld bytes",
		    sizeof(uuid_t));

	memcpy(&uuid, buf, sizeof(uuid_t));

	u = (uuid_t *)lua_newuserdata(L, sizeof(uuid_t));
	uuid_copy(*u, uuid);
	luaL_setmetatable(L, UUID_METATABLE);
	return 1;
}

/* uuid methods */
static int
lua_uuid_clear(lua_State *L)
{
	uuid_t *u;

	u = luaL_checkudata(L, 1, UUID_METATABLE);
	uuid_clear(*u);
	return 0;
}

static int
lua_uuid_concat(lua_State *L)
{
	luaL_Buffer b;
	size_t l;
	const char *s;

	luaL_buffinit(L, &b);
	s = luaL_tolstring(L, 1, &l);
	luaL_addlstring(&b, s, l);
	s = luaL_tolstring(L, 2, &l);
	luaL_addlstring(&b, s, l);

	luaL_pushresult(&b);
	return 1;
}

static int
lua_uuid_compare(lua_State *L)
{
	uuid_t *u1, *u2;

	u1 = luaL_checkudata(L, 1, UUID_METATABLE);
	u2 = luaL_checkudata(L, 2, UUID_METATABLE);
	lua_pushinteger(L, uuid_compare(*u1, *u2));
	return 1;
}

static int
lua_uuid_data(lua_State *L)
{
	uuid_t *u;

	u = luaL_checkudata(L, 1, UUID_METATABLE);
	lua_pushlstring(L, *u, sizeof(uuid_t));
	return 1;
}

static int
lua_uuid_is_null(lua_State *L)
{
	uuid_t *u;

	u = luaL_checkudata(L, 1, UUID_METATABLE);
	lua_pushboolean(L, uuid_is_null(*u));
	return 1;
}

static int
lua_uuid_time(lua_State *L)
{
	struct timeval tv;
	uuid_t *u;

	u = luaL_checkudata(L, 1, UUID_METATABLE);
	uuid_time(*u, &tv);
	lua_pushinteger(L, tv.tv_sec);
	lua_pushinteger(L, tv.tv_usec);
	return 2;
}

static int
lua_uuid_type(lua_State *L)
{
	struct timeval tv;
	uuid_t *u;

	u = luaL_checkudata(L, 1, UUID_METATABLE);
	switch (uuid_type(*u)) {
	case UUID_TYPE_DCE_NIL:
		lua_pushstring(L, "dce-nil");
		break;
	case UUID_TYPE_DCE_TIME:
		lua_pushstring(L, "dce-time");
		break;
	case UUID_TYPE_DCE_SECURITY:
		lua_pushstring(L, "dce-security");
		break;
	case UUID_TYPE_DCE_MD5:
		lua_pushstring(L, "dce-md5");
		break;
	case UUID_TYPE_DCE_RANDOM:
		lua_pushstring(L, "dce-random");
		break;
	case UUID_TYPE_DCE_SHA1:
		lua_pushstring(L, "dce-sha1");
		break;
#ifdef UUID_TYPE_DCE_TIME_V6
	case UUID_TYPE_DCE_TIME_V6:
		lua_pushstring(L, "dce-time-v6");
		break;
#endif
#ifdef UUID_TYPE_DCE_TIME_V7
	case UUID_TYPE_DCE_TIME_V7:
		lua_pushstring(L, "dce-time-v7");
		break;
#endif

	default:
		lua_pushstring(L, "unknown");
	}
	return 1;
}

static int
lua_uuid_variant(lua_State *L)
{
	struct timeval tv;
	uuid_t *u;

	u = luaL_checkudata(L, 1, UUID_METATABLE);
	switch (uuid_variant(*u)) {
	case UUID_VARIANT_NCS:
		lua_pushstring(L, "ncs");
		break;
	case UUID_VARIANT_DCE:
		lua_pushstring(L, "dce");
		break;
	case UUID_VARIANT_MICROSOFT:
		lua_pushstring(L, "microsoft");
		break;
	case UUID_VARIANT_OTHER:
		lua_pushstring(L, "other");
		break;
	default:
		lua_pushstring(L, "unknown");
	}
	return 1;
}
static int
lua_uuid_unparse(lua_State *L)
{
	char str[UUID_STR_SIZE + 1];
	uuid_t *u;

	u = luaL_checkudata(L, 1, UUID_METATABLE);
	uuid_unparse(*u, str);
	lua_pushstring(L, str);
	return 1;
}

static int
lua_uuid_equal(lua_State *L)
{
	uuid_t *u1, *u2;

	u1 = luaL_checkudata(L, 1, UUID_METATABLE);
	u2 = luaL_checkudata(L, 2, UUID_METATABLE);
	lua_pushboolean(L, uuid_compare(*u1, *u2) == 0 ? 1 : 0);
	return 1;
}

static int
lua_uuid_less(lua_State *L)
{
	uuid_t *u1, *u2;

	u1 = luaL_checkudata(L, 1, UUID_METATABLE);
	u2 = luaL_checkudata(L, 2, UUID_METATABLE);
	lua_pushboolean(L, uuid_compare(*u1, *u2) < 0 ? 1 : 0);
	return 1;
}

static int
lua_uuid_less_or_equal(lua_State *L)
{
	uuid_t *u1, *u2;

	u1 = luaL_checkudata(L, 1, UUID_METATABLE);
	u2 = luaL_checkudata(L, 2, UUID_METATABLE);
	lua_pushboolean(L, uuid_compare(*u1, *u2) <= 0 ? 1 : 0);
	return 1;
}

static int
lua_uuid_length(lua_State *L)
{
	lua_pushinteger(L, UUID_STR_SIZE);
	return 1;
}

int
luaopen_uuid(lua_State *L)
{
	struct luaL_Reg luauuid[] = {
		{ "generate",		lua_uuid_generate },
		{ "generate_random",	lua_uuid_generate_random },
		{ "generate_time",	lua_uuid_generate_time },
		{ "generate_time_safe",	lua_uuid_generate_time_safe },
#ifdef UUID_TYPE_DCE_TIME_V6
		{ "generate_time_v6",	lua_uuid_generate_time_v6 },
#endif
#ifdef UUID_TYPE_DCE_TIME_V7
		{ "generate_time_v7",	lua_uuid_generate_time_v7 },
#endif
		{ "parse",		lua_uuid_parse },
		{ "create",		lua_uuid_create },
		{ NULL, NULL }
	};
	struct luaL_Reg uuid_methods[] = {
		{ "clear",		lua_uuid_clear },
		{ "compare",		lua_uuid_compare },
		{ "data",		lua_uuid_data },
		{ "is_null",		lua_uuid_is_null },
		{ "time",		lua_uuid_time },
		{ "type",		lua_uuid_type },
		{ "variant",		lua_uuid_variant },
		{ "unparse",		lua_uuid_unparse },
		{ "__eq",		lua_uuid_equal },
		{ "__lt",		lua_uuid_less },
		{ "__le",		lua_uuid_less_or_equal },
		{ "__gc",		lua_uuid_clear },
		{ "__close",		lua_uuid_clear },
		{ "__tostring",		lua_uuid_unparse },
		{ "__concat",		lua_uuid_concat },
		{ "__len",		lua_uuid_length },
		{ NULL, NULL }
	};
	if (luaL_newmetatable(L, UUID_METATABLE)) {
		luaL_setfuncs(L, uuid_methods, 0);

		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);

		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);

	luaL_newlib(L, luauuid);
	lua_pushliteral(L, "_COPYRIGHT");
	lua_pushliteral(L, "Copyright (C) 2016 - 2026 by "
	    "micro systems marc balmer");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "UUID generation functions for Lua");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "uuid 1.2.4");
	lua_settable(L, -3);

	return 1;
}

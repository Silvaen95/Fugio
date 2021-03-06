#include "luajsonarray.h"

#include <QJsonValueRef>

#include "luajsonobject.h"

const char *LuaJsonArray::JsonArrayUserData::TypeName = "qt.jsonarray";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaJsonArray::mLuaInstance[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaJsonArray::mLuaMethods[] =
{
	{ "__gc",				LuaJsonArray::luaDelete },
	{ "ipairs",				LuaJsonArray::luaBegin },
	{ "isArray",			LuaJsonArray::luaIsArray },
	{ "isObject",			LuaJsonArray::luaIsObject },
	{ 0, 0 }
};

int LuaJsonArray::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, JsonArrayUserData::TypeName );

	lua_pushvalue( L, -1 );
	lua_setfield( L, -2, "__index" );

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaInstance );

	return( 1 );
}

int LuaJsonArray::luaNew( lua_State *L )
{
	pushjsonarray( L, QJsonArray() );

	return( 1 );
}

int LuaJsonArray::luaDelete( lua_State *L )
{
	JsonArrayUserData		*UserData = checkjsonarraydata( L );

	if( UserData )
	{
		UserData->mJsonArray.~QJsonArray();
	}

	return( 0 );
}

int LuaJsonArray::luaBegin( lua_State *L )
{
	JsonArrayUserData			*UserData = checkjsonarraydata( L );

	UserData->mIterator = UserData->mJsonArray.begin();

	lua_pushcclosure( L, LuaJsonArray::luaNext, 1 );

	return( 1 );
}

int LuaJsonArray::luaNext( lua_State *L )
{
	JsonArrayUserData			*UserData = (JsonArrayUserData *)lua_touserdata( L, lua_upvalueindex( 1 ) );

	if( UserData->mIterator != UserData->mJsonArray.end() )
	{
		lua_pushinteger( L, UserData->mIterator.i + 1 );

		QJsonValueRef	 ValRef = *UserData->mIterator;

		switch( ValRef.type() )
		{
			case QJsonValue::Array:
				LuaJsonArray::pushjsonarray( L, ValRef.toArray() );
				break;

			case QJsonValue::Bool:
				lua_pushboolean( L, ValRef.toBool() );
				break;

			case QJsonValue::Double:
				lua_pushnumber( L, ValRef.toDouble() );
				break;

			case QJsonValue::String:
				lua_pushfstring( L, "%s", ValRef.toString().toLatin1().constData() );
				break;

			case QJsonValue::Object:
				LuaJsonObject::pushjsonobject( L, ValRef.toObject() );
				break;

			case QJsonValue::Null:
				lua_pushnil( L );
				break;

			case QJsonValue::Undefined:
				lua_pushnil( L );
				break;
		}

		UserData->mIterator++;

		return( 2 );
	}

	return( 0 );
}

int LuaJsonArray::luaIsArray(lua_State *L)
{
	lua_pushboolean( L, true );

	return( 1 );
}

int LuaJsonArray::luaIsObject(lua_State *L)
{
	lua_pushboolean( L, false );

	return( 1 );
}

#endif

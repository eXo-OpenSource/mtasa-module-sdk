#include "Module.h"
#include "CFunctions.h"
Module* g_Module = nullptr;

constexpr std::size_t kNumWorkers = 2;

Module::Module(ILuaModuleManager* manager) : _moduleManager(manager), _jobManager(kNumWorkers)
{
}

Module::~Module()
{
	// Shutdown job manager threads
	_jobManager.Stop();
}

void Module::Start()
{
	// Start job manager worker threads
	_jobManager.Start();
}

void Module::Process()
{
	// Call complete callbacks on main thread
	_jobManager.SpreadResults();
}

void Module::register_lua_global(lua_State* lua_vm, const char* name, void* value)
{
	lua_pushlightuserdata(lua_vm, value);
	lua_setglobal(lua_vm, name);
}

void Module::register_table_function(lua_State* lua_vm, const char* function_name, lua_CFunction function)
{
	lua_pushstring(lua_vm, function_name);
		lua_pushcfunction(lua_vm, function);
	lua_settable(lua_vm, -3);
}

void Module::register_class(lua_State* lua_vm, const char* metatable, const luaL_Reg* registry)
{
	// https://www.lua.org/pil/28.2.html
	// Register metatable
	luaL_newmetatable(lua_vm, metatable);
		lua_pushstring(lua_vm, "__index");
			lua_pushvalue(lua_vm, -2);
		lua_settable(lua_vm, -3);
	luaL_openlib(lua_vm, nullptr, registry, 0);
	
	lua_setglobal(lua_vm, metatable);
}

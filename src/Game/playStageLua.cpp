#include "pch.h"
#include "gamemanager.h"
#include "stages.h"
#include "globalvars.h"
#include <lua.hpp>

using namespace PewPew;

#pragma comment (lib, "lua5.1.lib")

static PlayStage* gStage = 0;
static lua_State* gLua = 0;
static HANDLE     gThreadHandle;
static DWORD      gThreadID = 0;


namespace PewPew
{
    int lSpawn(lua_State *lua);
    int lDelay(lua_State *lua);

    //--------------------------------------------------------------------
    DWORD WINAPI lScriptThreadFunc( LPVOID param ) 
    { 
        lua_getglobal(gLua, "doScript");
        lua_call(gLua,0,0);
        return 0; 
    }
    //--------------------------------------------------------------------
    int lDelay(lua_State *lua)
    {
        int delay = lua_tonumber(lua, 1);
        Sleep(delay);
        return 0;
    }
    //--------------------------------------------------------------------
    int lSpawn(lua_State *lua)
    {
        MessageElem msg;
        msg.op = "spawn";
        msg.p1.set (lua_tostring(lua, 1));
        msg.p2.set (lua_tonumber(lua, 2));
        gStage->getMsgIn().addMessage(msg);

        //wait for return value
        while (!gStage->getMsgOut().getMessage( msg ))
        {
            Sleep (1);
            if (msg.op == "spawnRet")
                break;
        }

        lua_pushstring( lua, msg.p1.getString().c_str());
        return 1;
    }
    //--------------------------------------------------------------------
    int lSpawnA(lua_State *lua)
    {
        MessageElem msg;
        msg.op = "spawn_a";
        msg.p1.set (lua_tostring(lua, 1));
        msg.p2.set (lua_tonumber(lua, 2));
        gStage->getMsgIn().addMessage(msg);

        //wait for return value
        while (!gStage->getMsgOut().getMessage( msg ))
        {
            Sleep (1);
            if (msg.op == "spawn_aRet")
                break;
        }

        lua_pushstring( lua, msg.p1.getString().c_str());
        return 1;
    }
    //--------------------------------------------------------------------
    int lAssignPow(lua_State *lua)
    {
        MessageElem msg;
        msg.op = "assign_pow";
        msg.p1.set (lua_tostring(lua, 1));
        msg.p2.set (lua_tostring(lua, 2));
        gStage->getMsgIn().addMessage(msg);
        return 0;
    }
    //--------------------------------------------------------------------
    int lMoveTo(lua_State *lua)
    {
        MessageElem msg;
        msg.op = "moveto";
        msg.p1.set (lua_tostring(lua,1));
        msg.p2.set (lua_tostring(lua,2));
        msg.p3.set (lua_tonumber(lua,3));
        gStage->getMsgIn().addMessage(msg);
        return 0;
    }
    //--------------------------------------------------------------------
    bool lInit(PlayStage* s)
    {
        gStage = s;
        gLua = lua_open();
        luaL_openlibs(gLua);

        lua_register(gLua, "delay", lDelay);
        lua_register(gLua, "spawn", lSpawn);
        lua_register(gLua, "spawn_a", lSpawnA);
        lua_register(gLua, "assign_pow", lAssignPow);
        lua_register(gLua, "moveto", lMoveTo);

        return true;
    }
    //--------------------------------------------------------------------
    bool lClose()
    {
        TerminateThread(gThreadHandle, 0);
        gStage = 0;
        lua_close(gLua);
        return true;
    }
    //--------------------------------------------------------------------
    void lStartScript (const string& filename)
    {
        if (!gStage) return;

        luaL_dofile (gLua, filename.c_str());
        gThreadHandle = CreateThread (NULL, 0, lScriptThreadFunc, 0, 0, &gThreadID);        
    }
}


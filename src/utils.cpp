#include "utils.hpp"

// ==========================
// ======= SteamUtils =======
// ==========================


using luasteam::CallResultListener;

namespace {

class CallbackListener;
CallbackListener *callback_listener = nullptr;
int userStats_ref = LUA_NOREF;

//const char *sort_methods[] = {"Ascending", "Descending", nullptr};
//const char *display_types[] = {"Numeric", "TimeSeconds", "TimeMilliSeconds", nullptr};


class CallbackListener {
  private:
    STEAM_CALLBACK(CallbackListener, OnGamepadTextInputDismissed, GamepadTextInputDismissed_t);
};

void CallbackListener::OnGamepadTextInputDismissed(GamepadTextInputDismissed_t *data) {
    if (data == nullptr) {
        return;
    }
    lua_State *L = luasteam::global_lua_state;
    if (!lua_checkstack(L, 4)) {
        return;
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, userStats_ref);
    lua_getfield(L, -1, "onGamepadTextInputDismissed");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 2);
    } else {


        lua_createtable(L, 0, 2);

        lua_pushboolean(L, data->m_bSubmitted);
        lua_setfield(L, -2, "submitted");
        lua_pushnumber(L, data->m_unSubmittedText);
        lua_setfield(L, -2, "submittedText");//len in bytes
        lua_call(L, 1, 0);
        lua_pop(L, 1);

    }
}

} // namespace






// uint32 GetAppID();
EXTERN int luasteam_getAppID(lua_State *L) {
    lua_pushnumber(L, SteamUtils()->GetAppID());
    return 1;
}



// bool GetEnteredGamepadTextInput( char *pchText, uint32 cchText );
EXTERN bool luasteam_getEnteredGamepadTextInput(lua_State *L) {
    char pchText[1024];
    SteamUtils()->GetEnteredGamepadTextInput(pchText, 1024);
    lua_pushstring(L, pchText);
    return 1;
}

// uint32 GetEnteredGamepadTextLength();
EXTERN int luasteam_getEnteredGamepadTextLength(lua_State *L) {
    lua_pushnumber(L, SteamUtils()->GetEnteredGamepadTextLength());
    return 1;
}

//SIMPLE AND O-O-O (0,0,cc,int,cc)
//bool ShowGamepadTextInput( EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode, const char *pchDescription, uint32 unCharMax, const char *pchExistingText );
    //int sort_method = luaL_checkoption(L, 2, nullptr, sort_methods) + 1;
    //int display_type = luaL_checkoption(L, 3, nullptr, display_types) + 1;
EXTERN int luasteam_showGamepadTextInput(lua_State *L) {
    char pchDescription[1024];
    const char *url = luaL_checkstring(L, 1);//O-O-O!
    SteamUtils()->ShowGamepadTextInput(url,0,0,pchDescription, 1024);
    return 1;
}


namespace luasteam {

void add_utils(lua_State *L) {
    lua_createtable(L, 0, 4);
    add_func(L, "getAppID", luasteam_getAppID);
    add_func(L, "GetEnteredGamepadTextInput", luasteam_getEnteredGamepadTextInput);
    add_func(L, "GetEnteredGamepadTextLength", luasteam_getEnteredGamepadTextLength);
    add_func(L, "ShowGamepadTextInput", luasteam_showGamepadTextInput);
    lua_pushvalue(L, -1);

    utils_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_setfield(L, -2, "utils");
}

void init_utils(lua_State *L) { callback_listener = new CallbackListener(); }

void shutdown_utils(lua_State *L) {
    luaL_unref(L, LUA_REGISTRYINDEX, utils_ref);
    utils_ref = LUA_NOREF;
    delete callback_listener;
    callback_listener = nullptr;
}


} // namespace luasteam

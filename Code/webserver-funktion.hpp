#pragma once
#include "x-header.hpp"

#define auswahl_aktiv "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
#define auswahl_deaktiv "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert"

String web_server_variablen(const String &var);
void web_server_get_analyse(String name, String msg);

void webserver_config();

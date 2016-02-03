/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2016 Antenore Gatta
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 *  In addition, as a special exception, the copyright holders give
 *  permission to link the code of portions of this program with the
 *  OpenSSL library under certain conditions as described in each
 *  individual source file, and distribute linked combinations
 *  including the two.
 *  You must obey the GNU General Public License in all respects
 *  for all of the code used other than OpenSSL. *  If you modify
 *  file(s) with this exception, you may extend this exception to your
 *  version of the file(s), but you are not obligated to do so. *  If you
 *  do not wish to do so, delete this exception statement from your
 *  version. *  If you delete this exception statement from all source
 *  files in the program, then also delete it here.
 *
 */

#include "plugin_config.h"

#define NO_IMPORT_PYGTK
#include <Python.h>
#include <pygtk-2.0/pygobject.h>
#include <pygtk-2.0/pygtk/pygtk.h>

#if GTK_VERSION == 3
# include <gtk/gtkx.h>
# include <gdk/gdkx.h>
#endif

#include "common/remmina_plugin.h"

#if PY_VERSION_HEX < 0x03050000
#define Py_DecodeLocale _Py_char2wchar
#endif

#if PY_VERSION_HEX < 0x03040000
#define PyMem_RawMalloc PyMem_Malloc
#define PyMem_RawFree   PyMem_Free
#endif

static RemminaPluginService *remmina_plugin_service = NULL;

static void remmina_plugin_python_simple_init(RemminaProtocolWidget *gp)
{
	TRACE_CALL("remmina_plugin_exec_init");
	remmina_plugin_service->log_printf("[%s] Plugin init\n", PLUGIN_NAME);
}

static gboolean remmina_plugin_python_simple_open_connection(RemminaProtocolWidget *gp)
{
	TRACE_CALL("remmina_plugin_python_simple_open_connection");
	remmina_plugin_service->log_printf("[%s] Plugin open connection\n", PLUGIN_NAME);

	wchar_t *program = Py_DecodeLocale(PLUGIN_NAME, NULL);
	Py_SetProgramName(program);  /* optional but recommended */
	Py_Initialize();
	printf("Module Path: %s\n", Py_GetPath());
	//PyRun_SimpleString("from time import time,ctime\n"
	//		"print('Hello, today is', ctime(time()))\n");
	FILE* file = fopen("/home/antenore/software/Remmina.next/remmina-plugins/python_simple/pygtk_test.py", "r");
	PyRun_SimpleFile(file, "/home/antenore/software/Remmina.next/remmina-plugins/python_simple/pygtk_test.py");
	fclose(file);
	Py_Finalize();
	PyMem_RawFree(program);

	return FALSE;
}

static gboolean remmina_plugin_python_simple_close_connection(RemminaProtocolWidget *gp)
{
	TRACE_CALL("remmina_plugin_python_simple_close_connection");
	remmina_plugin_service->log_printf("[%s] Plugin close connection\n", PLUGIN_NAME);
	remmina_plugin_service->protocol_plugin_emit_signal(gp, "disconnect");
	return FALSE;
}

/* Array of RemminaProtocolSetting for basic settings.
 * Each item is composed by:
 * a) RemminaProtocolSettingType for setting type
 * b) Setting name
 * c) Setting description
 * d) Compact disposition
 * e) Values for REMMINA_PROTOCOL_SETTING_TYPE_SELECT or REMMINA_PROTOCOL_SETTING_TYPE_COMBO
 * f) Unused pointer
 */
static const RemminaProtocolSetting remmina_plugin_python_simple_basic_settings[] =
{
	{ REMMINA_PROTOCOL_SETTING_TYPE_END, NULL, NULL, FALSE, NULL, NULL }
};

/* Protocol plugin definition and features */
static RemminaProtocolPlugin remmina_plugin = {
	REMMINA_PLUGIN_TYPE_PROTOCOL,                     // Type
	PLUGIN_NAME,                                      // Name
	PLUGIN_DESCRIPTION,                               // Description
	GETTEXT_PACKAGE,                                  // Translation domain
	PLUGIN_VERSION,                                   // Version number
	PLUGIN_APPICON,                                   // Icon for normal connection
	PLUGIN_APPICON,                                   // Icon for SSH connection
	remmina_plugin_python_simple_basic_settings,      // Array for basic settings
	NULL,                                             // Array for advanced settings
	REMMINA_PROTOCOL_SSH_SETTING_NONE,                // SSH settings type
	NULL,                                             // Array for available features
	remmina_plugin_python_simple_init,                // Plugin initialization
	remmina_plugin_python_simple_open_connection,     // Plugin open connection
	remmina_plugin_python_simple_close_connection,    // Plugin close connection
	NULL,                                             // Query for available features
	NULL,                                             // Call a feature
	NULL,                                             // Send a keystroke    */
};

G_MODULE_EXPORT gboolean remmina_plugin_entry(RemminaPluginService *service)
{
	TRACE_CALL("remmina_plugin_entry");
	remmina_plugin_service = service;

	bindtextdomain(GETTEXT_PACKAGE, REMMINA_LOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");

	if (!service->register_plugin((RemminaPlugin *) &remmina_plugin))
	{
		return FALSE;
	}

	return TRUE;
}

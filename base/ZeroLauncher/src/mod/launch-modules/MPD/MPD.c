/*
 * Last updated: March 14, 2008
 * ~Keripo
 *
 * Copyright (C) 2008 Keripo
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
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "pz.h"

#define ENABLE_MPD 1
#define TOGGLE_UPDATE 2

extern void pz_ipod_reboot();
extern void pz_set_backlight_timer(int sec);

static PzModule *module;
static PzConfig *config;
static char path[256], conf[256], pack[256];
static const char *on_off_options[] = {"Off", "On", 0};

static int send_command(char *str)
{
	int sock;
	struct hostent *he;
	struct sockaddr_in addr;
	char *hostname = getenv("MPD_HOST");
	char *port = getenv("MPD_PORT");

	if (hostname == NULL) hostname = "127.0.0.1";
	if (port == NULL) port = "6600";

	if ((he = gethostbyname(hostname)) == NULL) {
		herror(hostname);
		return 1;
	}
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return 2;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr = *((struct in_addr *)he->h_addr);
	addr.sin_port = htons(atoi(port));
	memset(&(addr.sin_zero), 0, 8);
	if (connect(sock,(struct sockaddr *)&addr,sizeof(struct sockaddr))==-1){
		perror("connect");
		close(sock);
		return 3;
	}
	send(sock, str, strlen(str), 0);
	send(sock, "\n", 1, 0);
	close(sock);
	return 0;
}

static void init_conf()
{
	
	if (!(access(conf, F_OK) == 0)) {
		FILE *fconf = fopen(conf, "w");
		// Hardcoded for the same reason in init()
		fprintf(fconf,
			"port                            \"6600\"\n"
			"music_directory                 \"/mnt\"\n"
			"playlist_directory              \"%sPlaylists\"\n"
			"log_file                        \"%sMisc/mpd.log\"\n"
			"error_file                      \"%sMisc/mpd.error\"\n"
			"filesystem_charset              \"ISO-8859-1\"\n"
			"db_file                         \"%sData/mpd.db\"\n"
			"state_file                      \"%sData/mpdstate\"\n"
			"mixer_control                   \"PCM\"\n"
			"audio_output {\n"
			"        type                    \"oss\"\n"
			"        name                    \"oss\"\n"
			"}\n",
			pack, pack, pack, pack, pack
			);
		fclose(fconf);
	}
}

static void init_loopback()
{
	switch (vfork()) {
		case 0:
			execl("/bin/ifconfig", "ifconfig", "lo", "127.0.0.1", NULL);
		case -1:
			pz_perror("Unable to initialize loopback interface");
			break;
		default:
			wait(NULL);
			break;
	}
}

static void init_db()
{
	
	if (pz_get_int_setting(config, TOGGLE_UPDATE) == 1) {
		pz_message("MPD will now update its database. Please be patient; you will be notified once the update is done.");
		pz_set_backlight_timer(-2); // Save batteries
		pid_t pid;
		switch (pid = vfork()) {
			case 0:
				execl(path, path, "--update-db", conf, NULL);
			case -1:
				pz_perror("Unable to update MPD");
				break;
			default:
				wait(NULL);
				break;
		}
		while (!(kill(pid, 0)));
		pz_set_int_setting(config, TOGGLE_UPDATE, 0);
		pz_save_config(config);
		pz_set_backlight_timer(2); // So you can read the message
		pz_message("MPD has finished updating. Your iPod will now reboot.");
		pz_ipod_reboot();
	}
}

static void init_mpd()
{
	
	switch (vfork()) {
		case 0:
			execl(path, path, conf, NULL);
		case -1:
			pz_perror("Unable to start MPD");
			break;
		default:
			wait(NULL);
			break;
	}
	putenv("MPD_PORT=6600");
	putenv("MPD_HOST=127.0.0.1");
	while (send_command(""));
}

static void cleanup_mpd()
{
	pz_save_config(config);
	send_command("kill");
}

static PzWindow *toggle_update()
{
	if (pz_get_int_setting(config, TOGGLE_UPDATE) == 0) {
		pz_message("MPD will update on the next time reboot.");
		pz_set_int_setting(config, TOGGLE_UPDATE, 1);
	} else {
		pz_message("MPD will not update on the next time reboot.");
		pz_set_int_setting(config, TOGGLE_UPDATE, 0);
	}
	pz_save_config(config);
	return NULL;
}

static void init()
{
	// Due to some wierd pointer logic which I don't
	// understand, "pz_module_get_datapath" can only
	// be used once or else the pointers screw up.
	// As a result, all paths here are hardcoded.
	
	module = pz_register_module("MPD", cleanup_mpd);
	
	config = pz_load_config(pz_module_get_datapath(module, "Conf/zerolauncher.conf"));
	if (!pz_get_setting(config, ENABLE_MPD))
		pz_set_int_setting(config, ENABLE_MPD, 0);
	if (!pz_get_setting(config, TOGGLE_UPDATE))
		pz_set_int_setting(config, TOGGLE_UPDATE, 1);
	
	pz_menu_add_setting_group("/Music/Enable MPD", "Settings", ENABLE_MPD, config, on_off_options);
	
	if (pz_get_int_setting(config, ENABLE_MPD) == 1) {
		
		pz_menu_add_action_group("/Music/Toggle Update", "Settings", toggle_update);
		
		sprintf(pack, "%s", pz_module_get_datapath(module, ""));
		sprintf(path, "%sMPD-ke", pack);
		sprintf(conf, "%sConf/mpd.conf", pack);
		init_loopback();
		init_conf();
		init_db();
		init_mpd();
	}
}

PZ_MOD_INIT(init)

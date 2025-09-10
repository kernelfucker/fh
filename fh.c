/* See LICENSE file for license details */
/* fh - minimal system fetch utility */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/utsname.h>

#define version "0.2"

#define maxl 256
#define offset 0

void fh_sys(void);
void fh_upt(void);
void fh_pkg(void);
void fh_mem(void);
void fh_swp(void);
void fh_usr(int line);
void fh_dsk(void);
void print_ascii(const char *dist);
void print_info_line(int ascii_line, const char* info);
void to_lwr_case(char *str);
void show_version(void);
void help(void);

struct {
	const char *lines[10];
	int h;
} ascii;

void to_lwr_str(char *str){
	for(; *str; ++str) *str = tolower(*str);
}

void to_lwr_case(char *str){
	for(; *str; ++str){
		if(*str == 'G') *str = 'g';
		else if(*str == 'M') *str = 'm';
		else if(*str == 'K') *str = 'k';
	}
}

void print_ascii(const char* dist){
	if(strcmp(dist, "artix") == 0){
		ascii.lines[0] = "      /\\";
		ascii.lines[1] = "     /  \\";
		ascii.lines[2] = "    /`'.,\\";
		ascii.lines[3] = "   /     ',";
		ascii.lines[4] = "  /      ,`\\";
		ascii.lines[5] = " /   ,.'`.  \\";
		ascii.lines[6] = "/.,'`     `'.\\";
		ascii.h = 7;
	}

	else if(strcmp(dist, "arch") == 0){
        	ascii.lines[0] = "      /\\";
        	ascii.lines[1] = "     /  \\";
        	ascii.lines[2] = "    /\\   \\";
        	ascii.lines[3] = "   /      \\";
        	ascii.lines[4] = "  /   ,,   \\";
        	ascii.lines[5] = " /   |  |  -\\";
        	ascii.lines[6] = "/_-''    ''-_\\";
        	ascii.h = 7;
	}

	else if(strcmp(dist, "debian") == 0){
        	ascii.lines[0] = "  _____";
        	ascii.lines[1] = " /  __ \\";
        	ascii.lines[2] = "|  /    |";
        	ascii.lines[3] = "|  \\___-";
        	ascii.lines[4] = "-";
        	ascii.lines[5] = "  --_";
        	ascii.h = 6;
	}

	else if(strcmp(dist, "void") == 0){
		ascii.lines[0] = "    _______";
		ascii.lines[1] = " _ \\______ -";
		ascii.lines[2] = "| \\  ___  \\ |";
		ascii.lines[3] = "| | /   \\ | |";
		ascii.lines[4] = "| | \\___/ | |";
		ascii.lines[5] = "| \\______ \\_|";
		ascii.lines[6] = " -_______\\";
		ascii.h = 7;
	}

	else if(strcmp(dist, "gentoo") == 0){
		ascii.lines[0] = " _-----_";
		ascii.lines[1] = "(       \\";
		ascii.lines[2] = "\\    0   \\";
		ascii.lines[3] = " \\        )";
		ascii.lines[4] = " /      _/";
		ascii.lines[5] = "(     _-";
		ascii.lines[6] = "\\____-";
		ascii.h = 7;
	}

	else if(strcmp(dist, "fedora") == 0){
		ascii.lines[0] = "      _____";
		ascii.lines[1] = "     /   __)\\";
		ascii.lines[2] = "     |  /  \\ \\";
		ascii.lines[3] = "  ___|  |__/ /";
		ascii.lines[4] = " / (_    _)_/";
		ascii.lines[5] = "/ /  |  |";
		ascii.lines[6] = "\\ \\__/  |";
		ascii.lines[7] = " \\(_____/";
		ascii.h = 8;
	}

	else if(strcmp(dist, "alpine") == 0){
		ascii.lines[0] = "   /\\ /\\";
		ascii.lines[1] = "  // \\  \\";
		ascii.lines[2] = " //   \\  \\";
		ascii.lines[3] = "///    \\  \\";
		ascii.lines[4] = "//      \\  \\";
		ascii.lines[5] = "         \\";
		ascii.h = 6;
	}

	else if(strcmp(dist, "endeavouros") == 0){
		ascii.lines[0] = "      /\\";
		ascii.lines[1] = "    //  \\\\";
		ascii.lines[2] = "   //    \\ \\";
		ascii.lines[3] = " / /     _) )";
		ascii.lines[4] = "/_/___-- __-";
		ascii.lines[5] = " /____--";
		ascii.h = 6;
	}

	else {
		ascii.lines[0] = "    .--.";
		ascii.lines[1] = "   |o_o |";
		ascii.lines[2] = "   |:_/ |";
		ascii.lines[3] = "  //   \\ \\";
		ascii.lines[4] = " (|     | )";
		ascii.lines[5] = "/'\\_   _/\\";
		ascii.lines[6] = "\\___)=(___/";
		ascii.h = 7;
	}
}

void print_info_line(int ascii_line, const char* info){
	int ascii_w = 0;
	if(ascii.h > 0){
		for(int i = 0; i < ascii.h; ++i){
			int len = strlen(ascii.lines[i]);
			if(len > ascii_w) ascii_w = len;
		}
	}

	if(ascii_line < ascii.h){
		printf("%-*s  %s\n", ascii_w, ascii.lines[ascii_line], info);
	} else {
		printf("%*s  %s\n", ascii_w, "", info);
	}
}

void fh_sys(void){
	struct utsname buf;
	if(uname(&buf) != 0) return;
	char dist[maxl] = "tux";
	FILE *f = fopen("/etc/os-release", "r");
	if(f){
		char line[maxl];
		while(fgets(line, maxl, f)){
			if(strncmp(line, "ID=", 3) == 0){
				char* id = strchr(line, '=') + 1;
				if(*id == '"') id++;				
				size_t len = strcspn(id, "\n\"");
				memcpy(dist, id, len);
				dist[len] = '\0';
				break;
			}
		}

		fclose(f);
	}

	to_lwr_str(dist);
	print_ascii(dist);

	char p_name[maxl] = {0};
	f = fopen("/etc/os-release", "r");
	if(f){
		char line[maxl];
		while(fgets(line, maxl, f)){
			if(strstr(line, "PRETTY_NAME")){
				char* name = strchr(line, '=') + 2;
				size_t len = strcspn(name, "\"");
				memcpy(p_name, name, len);
				p_name[len] = '\0';
				break;
			}
		}

		fclose(f);
	}

	char sys[maxl];
	if(p_name[0]){
		to_lwr_str(p_name);
		snprintf(sys, sizeof(sys), "sys: %s %s", p_name, buf.machine);
		print_info_line(0, sys);
	} else {
		snprintf(sys, sizeof(sys), "sys: %s %s", buf.sysname, buf.machine);
		to_lwr_str(sys);
		print_info_line(0, sys);
	}

	snprintf(sys, sizeof(sys), "krn: %s %s", buf.sysname, buf.release);
	to_lwr_str(sys);
	print_info_line(1, sys);
}

void fh_hst(void){
	char hst[maxl];
	if(gethostname(hst, sizeof(hst)) == 0){
		char info[maxl];
		snprintf(info, sizeof(info), "hst: %s", hst);
		print_info_line(2, info);
	}
}

void fh_upt(void){
	FILE* f = fopen("/proc/uptime", "r");
	if(f){
		double upt;
		if(fscanf(f, "%lf", &upt) == 1){
			int h = upt / 3600;
			int m = ((int)upt % 3600) / 60;
			char info[maxl];
			snprintf(info, sizeof(info), "upt: %dh, %dm", h, m);
			print_info_line(3, info);
		}

		fclose(f);
	}
}

int try_pk(const char* c, const char* name, const char* bin){
	if(access(bin, X_OK) != 0) return 0;
	char full_c[maxl];
	snprintf(full_c, sizeof(full_c), "%s 2>/dev/null", c);
	FILE* f = popen(full_c, "r");
	if(!f) return 0;
	int cn = 0;
	if(fscanf(f, "%d", &cn) == 1 && cn > 0){
		pclose(f);
		return cn;
	}

	pclose(f);
	return 0;
}

void fh_pkg(void){
	struct {
		const char* c;
		const char* name;
		const char* bin;
	} keepers[] = {
		{"pacman -Qq | wc -l", "pacman", "/usr/bin/pacman"},
		{"dpkg-query -f '.\\n' -W | wc -l", "dpkg", "/usr/bin/dpkg-query"},
		{"xbps-query -l | wc -l", "xbps-query", "/usr/bin/xbps-query"},
		{"rpm -qa | wc -l", "rpm", "/bin/rpm"},
		{"apk list --installed | wc -l", "apk", "/sbin/apk"},
		{"qlist -I | wc -l", "emerge", "/usr/bin/qlist"},
		{"pkg_info -a | wc -l", "pkg_info", "/usr/sbin/pkg_info"},
		{"pkg info | wc -l", "pkg", "/usr/sbin/pkg"},
		{NULL, NULL, NULL}
	};

	char info[maxl] = "pkg: ";
	int first = 1;
	for(int i = 0; keepers[i].c; i++){
		int cn = try_pk(keepers[i].c, keepers[i].name, keepers[i].bin);
		if(cn > 0){
			if(!first){
				strncat(info, ", ", sizeof(info) - strlen(info) - 1);
			}

			char temp[20];
			snprintf(temp, sizeof(temp), "%d [%s]", cn, keepers[i].name);
			strncat(info, temp, sizeof(info) - strlen(info) - 1);
			first = 0;
		}
	}

	if(first){
		strncat(info, "n/a", sizeof(info) - strlen(info) - 1);
	}

	print_info_line(4, info);
}
void fh_mem(void){
	FILE* f = fopen("/proc/meminfo", "r");
	if(f){
		long total = 0;
		long free = 0;
		long bufs = 0;
		long cached = 0;
		char line[maxl];
		while(fgets(line, sizeof(line), f)){
			if(sscanf(line, "MemTotal: %ld kB", &total) == 1) continue;
			if(sscanf(line, "MemFree: %ld kB", &free) == 1) continue;
			if(sscanf(line, "Buffers: %ld kB", &bufs) == 1) continue;
			if(sscanf(line, "Cached: %ld kB", &cached) == 1) continue;
		}

		fclose(f);
		if(total > 0){
			long used = total - free - bufs - cached;
			char info[maxl];
			snprintf(info, sizeof(info), "mem: %ldm / %ldm", used/1024, total/1024);
			print_info_line(5, info);
		}
	}
}

void fh_swp(void){
	FILE *f = fopen("/proc/meminfo", "r");
	if(f){
		long total = 0;
		long free = 0;
		char line[maxl];
		while(fgets(line, sizeof(line), f)){
			if(sscanf(line, "SwapTotal: %ld kB", &total) == 1) continue;
			if(sscanf(line, "SwapFree: %ld kB", &free) == 1) continue;
		}

		fclose(f);
		if(total > 0){
			char info[maxl];
			snprintf(info, sizeof(info), "swp: %ldm / %ldm", (total-free)/1024, total/1024);
			print_info_line(6, info);
		}
	}
}

void fh_usr(int line){
	char* user = getenv("USER");
	char info[maxl];
	snprintf(info, sizeof(info), "usr: %s", user ? user : "n/a");
	print_info_line(line, info);
}

void fh_dsk(void){
	FILE *f = popen("df -h --output=target,used,size,fstype 2>/dev/null | tail -n +2", "r");
	if(f){
		char mount[maxl], used[maxl], size[maxl], fstype[maxl];
		int line = ascii.h;
		int disk_lines = 0;
		while(fscanf(f, "%s %s %s %s", mount, used, size, fstype) == 4){
			if(strstr(fstype, "ext") || strstr(fstype, "xfs") || strstr(fstype, "btrfs") || strstr(fstype, "f2fs") || strstr(fstype, "vfat") || strstr(fstype, "zfs")){
				to_lwr_case(used);
				to_lwr_case(size);
				char info[maxl];
				snprintf(info, sizeof(info), "dsk: [%s] %s / %s", mount, used, size);
				print_info_line(line + disk_lines, info);
				disk_lines++;
			}
		}

		pclose(f);
		fh_usr(line + disk_lines);
	} else {
		fh_usr(ascii.h);
	}
}

void show_version(void){
	printf("fh-%s\n", version);
	exit(0);
}

void help(void){
	printf("usage: fh [options]..\n");
	printf("options:\n");
	printf("  -v	show version information\n");
	printf("  -h	display this\n");
	exit(0);
}
	
int main(int argc, char *argv[]){
	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], "-h") == 0){
			help();
		} else if(strcmp(argv[i], "-v") == 0){
			show_version();
		}
	}

	fh_sys();
	fh_hst();
	fh_upt();
	fh_pkg();
	fh_mem();
	fh_swp();
	fh_dsk();

	return 0;
}

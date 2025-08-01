/* See LICENSE file for license details */
/* fh - minimal system fetch utility */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/utsname.h>

#define version "0.1"

#define maxl 256
#define offset 0

void fh_sys(void);
void fh_upt(void);
void fh_pkgs(void);
void fh_wm(void);
void fh_cpu(void);
void fh_gpu(void);
void fh_mem(void);
void fh_swp(void);
void fh_user(int line);
void fh_disk(void);
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

	else if(strcmp(dist, "openbsd") == 0){
		ascii.lines[0] = "      _____";
		ascii.lines[1] = "    \\-     -/";
		ascii.lines[2] = " \\_/         \\";
		ascii.lines[3] = " |        O O |";
		ascii.lines[4] = " |_  <   )  3 )";
		ascii.lines[5] = " / \\         /";
		ascii.lines[6] = "    /-_____-\\";
		ascii.h = 7;
	}

	else if(strcmp(dist, "freebsd") == 0){
		ascii.lines[0] = "/\\,-'''''-,/\\";
		ascii.lines[1] = "\\_)       (_/";
		ascii.lines[2] = "|           |";
		ascii.lines[3] = "|           |";
		ascii.lines[4] = " ;         ;";
		ascii.lines[5] = "  '-_____-'";
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
				size_t len = strcspn(id, "\n\"");
				memcpy(dist, id, len);
				dist[len] = '\0';
				break;
			}
		}

		fclose(f);
	}

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

void fh_pkgs(void){
	struct {
		const char* c;
		const char* name;
		const char* bin;
	} keepers[] = {
		{"pacman -Qq | wc -l", "pacman", "/usr/bin/pacman"},
		{"dpkg-query -f '.\\n' -W | wc -l", "dpkg", "/usr/bin/dpkg-query"},
		{"xbps-query -l | wc -l", "xbps", "/usr/bin/xbps-query"},
		{"rpm -qa | wc -l", "rpm", "/bin/rpm"},
		{"apk list --installed | wc -l", "apk", "/sbin/apk"},
		{"qlist -I | wc -l", "emerge", "/usr/bin/qlist"},
		{"pkg_info -a | wc -l", "pkg_info", "/usr/sbin/pkg_info"},
		{"pkg info | wc -l", "pkg", "/usr/sbin/pkg"},
		{NULL, NULL, NULL}
	};

	char info[maxl] = "pkgs: ";
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

void fh_wm(void){
	FILE *f = f = popen("xprop -root _NET_SUPPORTING_WM_CHECK 2>/dev/null", "r");
	if(!f){
		print_info_line(5, "wm: n/a");
		return;
	}

	char line[maxl];
	if(!fgets(line, sizeof(line), f)){
		print_info_line(5, "wm: n/a");
		pclose(f);
		return;
	}

	pclose(f);
	char *id_st = strchr(line, '#');
	if(!id_st){
		print_info_line(5, "wm: n/a");
		return;
	}

	id_st++;
	char w_id[maxl];
	sscanf(id_st, "%s", w_id);
	char c[maxl];
	snprintf(c, sizeof(c), "xprop -id %s _NET_WM_NAME", w_id);
	f = popen(c, "r");
	if(!f){
		print_info_line(5, "wm: n/a");
		return;
	}

	if(!fgets(line, sizeof(line), f)){
		print_info_line(5, "wm: n/a");
		pclose(f);
		return;
	}

	pclose(f);
	char *name_st = strchr(line, '"');
	if(!name_st){
		print_info_line(5, "wm: n/a");
		return;
	}

	name_st++;
	char *name_end = strchr(name_st, '"');
	if(name_end) *name_end = '\0';
	char info[maxl];
	snprintf(info, sizeof(info), "wm: %s", name_st);
	to_lwr_str(info);
	print_info_line(5, info);
}

void fh_cpu(void){
	FILE* f = fopen("/proc/cpuinfo", "r");
	if(!f){
		print_info_line(6, "cpu: n/a");
		return;
	}

	char line[maxl];
	char model[maxl] = {0};
	double mhz = 0.0;
	int cores = 0;
	int threads = 0;
	while(fgets(line, sizeof(line), f)){
		if(strncmp(line, "model name", 10) == 0 && model[0] == '\0'){
			char* colon = strchr(line, ':');
			if(colon){
				snprintf(model, sizeof(model), "%s", colon + 2);
				model[strcspn(model, "\n")] = '\0';
				char* p;
				while((p = strstr(model, "(tm)"))) memmove(p, p + 4, strlen(p + 4) + 1);
				while((p = strstr(model, "(r)"))) memmove(p, p + 3, strlen(p + 3) + 1);
				while((p = strstr(model, "CPU"))) memmove(p, p + 3, strlen(p + 3) + 1);
 				while((p = strstr(model, "Processor"))) memmove(p, p + 9, strlen(p + 9) + 1);
				while((p = strstr(model, "processor"))) memmove(p, p + 9, strlen(p + 9) + 1);
				size_t len = strlen(model);
				while(len > 0 && isspace(model[len - 1])) model[--len] = '\0';
			}
		}

		if(strncmp(line, "cpu MHz", 7) == 0 && mhz == 0.0){
			char* colon = strchr(line, ':');
			if(colon) mhz = atof(colon + 1);
		}

		if(strncmp(line, "cpu cores", 9) == 0 && cores == 0){
			char* colon = strchr(line, ':');
			if(colon) cores = atoi(colon + 1);
		}

		if(strncmp(line, "siblings", 8) == 0 && threads == 0){
			char *colon = strchr(line, ':');
			if(colon) threads = atoi(colon + 1);
		}

		if(model[0] && mhz > 0.0 && cores > 0 && threads > 0) break;
	}

	fclose(f);
	if(model[0]){
		char info[maxl];
		snprintf(info, sizeof(info), "cpu: %s [%dc%dt] @ %.1fg", model, cores, threads, mhz / 1000.0);
		to_lwr_str(info);
		print_info_line(6, info);
	} else {
		print_info_line(6, "cpu: n/a");
	}
}

void fh_gpu(void){
	FILE* f = popen("lspci | grep -iE 'vga|3d|display'", "r");
	if(!f){
		print_info_line(7, "gpu: n/a");
		return;
	}

	char line[maxl];
	if(!fgets(line, sizeof(line), f)){
		print_info_line(7, "gpu: n/a");
		pclose(f);
		return;
	}

	fclose(f);
	char* start = line;
	char radeon_model[maxl] = {0};
	while((start = strchr(start, '['))){
		char* end = strchr(start, ']');
		if(!end) break;
		*end = '\0';
		if(strcasestr(start, "radeon")){
			strncpy(radeon_model, start + 1, sizeof(radeon_model) - 1);
			radeon_model[sizeof(radeon_model) - 1] = '\0';
		}

		*end = ']';
		start = end + 1;
	}

	if(strlen(radeon_model) > 0){
		char info[maxl];
		snprintf(info, sizeof(info), "gpu: amd ati %s", radeon_model);
		to_lwr_str(info);
		print_info_line(7, info);
	} else {
		print_info_line(7, "gpu: n/a");
	}
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
			print_info_line(8, info);
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
			print_info_line(9, info);
		}
	}
}

void fh_user(int line){
	char* user = getenv("USER");
	char info[maxl];
	snprintf(info, sizeof(info), "user: %s", user ? user : "n/a");
	print_info_line(line, info);
}

void fh_disk(void){
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
				snprintf(info, sizeof(info), "disk [%s]: %s / %s", mount, used, size);
				print_info_line(line + disk_lines, info);
				disk_lines++;
			}
		}

		pclose(f);
		fh_user(line + disk_lines);
	} else {
		fh_user(ascii.h);
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
	fh_pkgs();
	fh_wm();
	fh_cpu();
	fh_gpu();
	fh_mem();
	fh_swp();
	fh_disk();

	return 0;
}

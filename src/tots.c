/* vim: set tabstop=8 shiftwidth=8:
 * name: tots.c
 * funx: generate bin ts file with text data in stdin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for strcmp, etc */

#include "common.h"
#include "if.h"

#define RPT_LEVEL       RPT_WARNING /* report level: RPT_OK(0) to RPT_EMERG(-8) */

static FILE *fd_o = NULL;
static char file_o[FILENAME_MAX] = "";

static int deal_with_parameter(int argc, char *argv[]);
static void show_help();
static void show_version();

int main(int argc, char *argv[])
{
        int cnt;
        char tbuf[LINE_LENGTH_MAX + 10]; /* txt data buffer */
        uint8_t bbuf[LINE_LENGTH_MAX / 3 + 10]; /* bin data buffer */
        char *tag;
        char *pt;

        if(0 != deal_with_parameter(argc, argv)) {
                return -1;
        }

        fd_o = fopen(file_o, "wb");
        if(NULL == fd_o) {
                rpt(RPT_ERR, "open \"%s\" failed\n", file_o);
                return -1;
        }

        while(NULL != fgets(tbuf, LINE_LENGTH_MAX, stdin)) {
                pt = tbuf;
                while(0 == next_tag(&tag, &pt)) {
                        if(0 == strcmp(tag, "*ts")) {
                                cnt = next_nbyte_hex(bbuf, &pt, LINE_LENGTH_MAX / 3);
                                //fprintf(stderr, "ts: %d\n", cnt);
                                fwrite(bbuf, cnt, 1, fd_o);
                        }
                        if(0 == strcmp(tag, "*rs")) {
                                cnt = next_nbyte_hex(bbuf, &pt, LINE_LENGTH_MAX / 3);
                                //fprintf(stderr, "rs: %d\n", cnt);
                                fwrite(bbuf, cnt, 1, fd_o);
                        }
                        if(0 == strcmp(tag, "*pes")) {
                                cnt = next_nbyte_hex(bbuf, &pt, LINE_LENGTH_MAX / 3);
                                //fprintf(stderr, "pes: %d\n", cnt);
                                fwrite(bbuf, cnt, 1, fd_o);
                        }
                        if(0 == strcmp(tag, "*es")) {
                                cnt = next_nbyte_hex(bbuf, &pt, LINE_LENGTH_MAX / 3);
                                //fprintf(stderr, "es: %d\n", cnt);
                                fwrite(bbuf, cnt, 1, fd_o);
                        }
                }
        }

        fclose(fd_o);

        return 0;
}

static int deal_with_parameter(int argc, char *argv[])
{
        int i;

        if(1 == argc) {
                /* no parameter */
                fprintf(stderr, "No binary file to write...\n\n");
                show_help();
                return -1;
        }

        for(i = 1; i < argc; i++) {
                if('-' == argv[i][0]) {
                        if(     0 == strcmp(argv[i], "-h") ||
                                0 == strcmp(argv[i], "--help")) {
                                show_help();
                                return -1;
                        }
                        else if(0 == strcmp(argv[i], "-v") ||
                                0 == strcmp(argv[i], "--version")) {
                                show_version();
                                return -1;
                        }
                        else {
                                rpt(RPT_ERR, "Wrong parameter: %s\n", argv[i]);
                                return -1;
                        }
                }
                else {
                        strcpy(file_o, argv[i]);
                }
        }

        return 0;
}

static void show_help()
{
        puts("'tots' read from stdin, translate 'XY ' to 0xXY, send to file.");
        puts("");
        puts("Usage: tots [OPTION] file [OPTION]");
        puts("");
        puts("Options:");
        puts("");
        puts(" -h, --help       print this information, then exit");
        puts(" -v, --version    print my version, then exit");
        puts("");
        puts("Examples:");
        puts("  tots xxx.ts");
        puts("");
        puts("Report bugs to <zhoucheng@tsinghua.org.cn>.");
        return;
}

static void show_version()
{
        char str[100];

        sprintf(str, "tots of tstools %s", TSTOOLS_VERSION);
        puts(str);
        sprintf(str, "Build time: %s %s", __DATE__, __TIME__);
        puts(str);
        puts("");
        puts("Copyright (C) 2009,2010,2011,2012 ZHOU Cheng.");
        puts("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>");
        puts("This is free software; contact author for additional information.");
        puts("There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR");
        puts("A PARTICULAR PURPOSE.");
        puts("");
        puts("Written by ZHOU Cheng.");
        return;
}

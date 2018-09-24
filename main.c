#include "function.h"
#include "opt.h"
#include <stdio.h>
#include <string.h>

#define COL 16
#define BASE 16
#define BSIZE 1024
#define DELIM 0x20
#define LEN 2

#define FCHAR '='

#define OFFLEN (sizeof(fpos_t) * 8 / 4)
#define OFFBASE 16
#define DLENGTH 2

#define DCOLTWOSIDE 8

static void printoutheader(char *carr_buff,int fchar,unsigned int length);
static int
showHelp(const char* path, const char* opt[], const char* optdes[], int ret);
static const char *
basename(const char* ch);
static int
showErr(const char* err[], unsigned int index);

static void
dumpByte(FILE *sptr_fin,char* carr_buff,
    unsigned int ui_col,
    unsigned int ui_base,
    unsigned int ui_len,
    fpos_t start,
    size_t length);
static void
dumpChar(FILE *sptr_fin,char* carr_buff, unsigned int ui_col, fpos_t start, size_t length);
static void
dumpDual(FILE *sptr_fin,char* carr_buff, unsigned int ui_col, fpos_t start, size_t length);

static int
findStdC(int ch, const char* stdc);

static const char carr_hexpref[] = "0x";
static const char carr_DSEPERATE[] = " | ";

static const char* cpa_opt[] = { "-b", "-o", "-d", "-h", "-a",
    "-c:", "-s:", "-l:", "-t", NULL };
enum _opt {
    e_optbin,
    e_optoct,
    e_optdec,
    e_opthex,
    e_optascii,
    e_optcol,
    e_optstart,
    e_optlength,
    e_opttwoside
};
static const char* cpa_optdes[] = { "-b Binary show",
    "-o Octal Show",
    "-d 10 base Show (Decimal)",
    "-h Hex Show",
    "-a ASCII Show",
    "-c:{n} n=number of column",
    "-s:{n} n=offset",
    "-l:{n} n=Length",
    "-t Dual view",
    NULL };

static const char* cpa_err[] = { "Parameter isn't an unsigned interger",
    "File can't be accessed",
    "Column must more than 0",
    NULL };

enum _err {
    e_errpar,
    e_errfile,
    e_errzero
};

static const char carr_stdc[] = { '\0', '\a', '\b', '\f', '\n',
    '\r', '\t', '\v', '\0', '\0' };
static const char carr_stdc_str[] = { '0', 'a', 'b', 'f', 'n',
    'r', 't', 'v', '\0' };

int main(int argc, const char* argv[])
{
	FILE* sptr_fin;
    static char carr_buff[BSIZE];
    unsigned int ui_cindex, ui_pindex, ui_base, ui_col, ui_len, ui_colflag;
    fpos_t ui_start;
	size_t ui_length;
    unsigned int i;
    int i_actIndex;

    if (argc == 1) {
        return showHelp(argv[0], cpa_opt, cpa_optdes, 1);
    }
    /******************* Set Default *********************/

    ui_base = BASE;
    ui_col = DCOLTWOSIDE;
    ui_len = LEN;
    ui_colflag = 0;
    ui_start = 0;
    ui_length = -1;
    i_actIndex = e_opttwoside;

    /******************* Parameter Operation *********************/

    for (ui_cindex = DSTART;
         (ui_pindex = opt_action(
              argc, argv, cpa_opt, carr_buff, BSIZE, DSTART))
         != e_optend;
         ui_cindex++) {

        switch (ui_pindex) {

        case e_optbin:
            ui_base = 2;
            ui_len = 8;

            if (!ui_colflag)
                ui_col = 8;

            i_actIndex = -1;
            break;

        case e_optoct:
            ui_base = 8;
            ui_len = 3;

            if (!ui_colflag)
                ui_col = 8;

            i_actIndex = -1;
            break;

        case e_optdec:
            ui_base = 10;
            ui_len = 3;

            if (!ui_colflag)
                ui_col = 10;

            i_actIndex = -1;
            break;

        case e_opthex:
            ui_base = 16;
            ui_len = 2;

            if (!ui_colflag)
                ui_col = 16;

            i_actIndex = -1;
            break;

        case e_optascii:
            i_actIndex = e_optascii;
            break;

        case e_optcol:
            if (!isUint(carr_buff) && (!isUintHex(&carr_buff[strlen(carr_hexpref)]) || strncmp(carr_buff, carr_hexpref, strlen(carr_hexpref)))) {
                fprintf(stderr, "PARAM: %s\n", carr_buff);
                return showErr(cpa_err, e_errpar);
            }
            i = (!strncmp(carr_buff, carr_hexpref, strlen(carr_hexpref))) ? 16 : 10;

            if (!(ui_col = s2ui(&carr_buff[(i == 16) ? strlen(carr_hexpref) : 0], i)))

            {
                fprintf(stderr, "PARAM: %s\n", carr_buff);
                return showErr(cpa_err, e_errzero);
            }
            ui_colflag = 1;

            break;

        case e_optstart:
            if (!isUint(carr_buff) && (!isUintHex(&carr_buff[strlen(carr_hexpref)]) || strncmp(carr_buff, carr_hexpref, strlen(carr_hexpref)))) {
                fprintf(stderr, "PARAM: %s\n", carr_buff);
                return showErr(cpa_err, e_errpar);
            }
            i = (!strncmp(carr_buff, carr_hexpref, strlen(carr_hexpref))) ? 16 : 10;

            ui_start = s2fpos(&carr_buff[(i == 16) ? strlen(carr_hexpref) : 0], i);

            break;

        case e_optlength:
            if (!isUint(carr_buff) && (!isUintHex(&carr_buff[strlen(carr_hexpref)]) || strncmp(carr_buff, carr_hexpref, strlen(carr_hexpref)))) {
                fprintf(stderr, "PARAM: %s\n", carr_buff);
                return showErr(cpa_err, e_errpar);
            }
            i = (!strncmp(carr_buff, carr_hexpref, strlen(carr_hexpref))) ? 16 : 10;

            ui_length = s2sT(&carr_buff[(i == 16) ? strlen(carr_hexpref) : 0], i);

            break;

        case e_opttwoside:

            i_actIndex = e_opttwoside;

            if (!ui_colflag)
                ui_col = DCOLTWOSIDE;

            break;

        case e_optother:
		
			if (!(sptr_fin = fopen(carr_buff, "rb"))) {
				fprintf(stderr, "FILE: %s\n", carr_buff);
				return showErr(cpa_err, e_errfile);
			}

            switch (i_actIndex) {

            case e_optascii:
                dumpChar(sptr_fin,carr_buff, ui_col, ui_start, ui_length);
                break;

            case e_opttwoside:
                dumpDual(sptr_fin,carr_buff, ui_col, ui_start, ui_length);
                break;

            default:
                dumpByte(sptr_fin,carr_buff, ui_col, ui_base, ui_len, ui_start, ui_length);
            }
			
			fclose(sptr_fin);

            break;
        }
    }

    return 0;
}

static int
showHelp(const char* path, const char* opt[], const char* optdes[], int ret)
{
    unsigned int i;
    fprintf(stderr, "\nUSAGE: %s {option} file\n\n", basename(path));

    fprintf(stderr, "OPTIONS\n");
    for (i = 0; opt[i] && optdes[i]; i++) {
        fprintf(stderr, "%s =\t%s\n", opt[i], optdes[i]);
    }
    fputc('\n', stderr);

    return ret;
}

static const char *
basename(const char* ch)
{
    const char *j;
    for (j=ch; *ch; ++ch) if (*ch == '\\' || *ch == '/') j = ch+1;

    return j;
}

static int
showErr(const char* err[], unsigned int index)
{

    fprintf(stderr, "ERROR NO %u: %s\n", index, err[index]);
    return -1 * (++index);
}

size_t printlinebyte(FILE *sptr_fin,char *carr_buff,
				const unsigned int ui_col,
				const unsigned int ui_base,
				const unsigned int ui_len,
				const fpos_t start,
				const size_t length,
				size_t j)
{
	size_t l;
	int i_ch;
	
	for (l = j; (j < l + ui_col) && ((i_ch = fgetc(sptr_fin)) != EOF && (j < length || length == -1)); ++j)
	{

        ui2s(i_ch, carr_buff, BSIZE, ui_base, ui_len);
        printf("%s%c", carr_buff, DELIM);

	}
	
	return j;
}

static void
dumpByte(FILE *sptr_fin, char* carr_buff,
    unsigned int ui_col,
    unsigned int ui_base,
    unsigned int ui_len,
    fpos_t start,
    size_t length)
{
    size_t j=0;
	
	printoutheader(carr_buff,FCHAR,DLENGTH);


    if (fsetpos(sptr_fin,&start))
        return;

    do 
	{
		if (!(j % ui_col)) {

            sT2s(j + start, carr_buff, BSIZE, OFFBASE, OFFLEN);

            printf("%s: ", carr_buff);
        }
		
		j=printlinebyte(sptr_fin,carr_buff,ui_col,ui_base,ui_len,start,length,j);
		
        putchar('\n');
		
		
		
	}while( !feof(sptr_fin) && (j < length || length == -1));
		
    putchar('\n');
}

size_t printlinechar(FILE *sptr_fin,char *carr_buff,
				const unsigned int ui_col,
				const fpos_t start,
				const size_t length,
				size_t j)
{
	size_t l;
	int i_ch,k;
	
	for (l = j; (j < l + ui_col) && ((i_ch = fgetc(sptr_fin)) != EOF && (j < length || length == -1)); ++j)
	{

        if ((k = findStdC(i_ch, carr_stdc)) > -1)
            printf("\\%c%c", carr_stdc_str[k], DELIM);

        else
            printf(" %c%c", i_ch, DELIM);

	}
	
	return j;
}

static void
dumpChar(FILE *sptr_fin, char* carr_buff, unsigned int ui_col, fpos_t start, size_t length)
{
    size_t j=0;

	printoutheader(carr_buff,FCHAR,DLENGTH);

    if (fsetpos(sptr_fin,&start))
        return;

    do 
	{
		if (!(j % ui_col)) {

            sT2s(j + start, carr_buff, BSIZE, OFFBASE, OFFLEN);

            printf("%s: ", carr_buff);
        }
		
		j=printlinechar(sptr_fin,carr_buff,ui_col,start,length,j);
		
        putchar('\n');
		
		
		
	}while( !feof(sptr_fin) && (j < length || length == -1));
	
    putchar('\n');
}

static void
dumpDual(FILE *sptr_fin, char* carr_buff, unsigned int ui_col, fpos_t start, size_t length)
{
    unsigned int i, k;
    size_t n, j, m, p;
    fpos_t tmp1, tmp2;

	printoutheader(carr_buff,FCHAR,DLENGTH);

    if (fsetpos(sptr_fin,&start))
		return;

	fgetpos(sptr_fin,&tmp1);fgetpos(sptr_fin,&tmp2); j = 0; n = 0;
	
    do {
		
        fgetpos(sptr_fin,&tmp1);
		fsetpos(sptr_fin,&tmp2);
		
		m=j;
		
		if (!(j % ui_col)) {

            sT2s(j + start, carr_buff, BSIZE, OFFBASE, OFFLEN);

            printf("%s: ", carr_buff);
        }
		
		j=printlinebyte(sptr_fin,carr_buff,ui_col,BASE,LEN,start,length,j);
		p=j-m;

        if (!n && p < ui_col && p > 0)
            printf(carr_DSEPERATE);

        else {

            if ((j % ui_col) && (ui_col < length) && (((j <= length) && (length != -1)) || (length == -1)))
                for (i = 0; i < (ui_col - j % ui_col); i++)
                    for (k = 0; k < LEN + 1; k++)
                        printf("%c", DELIM);

            if (p)
                printf(carr_DSEPERATE);
            else
                return;
        }

        fgetpos(sptr_fin,&tmp2);
        fsetpos(sptr_fin,&tmp1);
		

		j=printlinechar(sptr_fin,carr_buff,ui_col,start,length,m);

		putchar('\n');

		
    }while(n++,(!feof(sptr_fin) && (j < length || length == -1)));

}

static int
findStdC(int ch, const char* stdc)
{
    int i;
    for (i = 0; (stdc[i] != 0) || ((stdc[i + 1] != 0)); i++) {
        if (ch == stdc[i])
            return i;
    }

    return -1;
}

static void printoutheader(char *carr_buff,int fchar,unsigned int length)
{
	unsigned int i;
    for (i = 0; i < length; i++)
        putchar(fchar);

    printf(" %s ", basename(carr_buff));

    for (i = 0; i < length; i++)
        putchar('=');
    putchar('\n');
}

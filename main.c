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

#define OFFLEN (sizeof(size_t) * 8 / 4)
#define OFFBASE 16
#define DLENGTH 2

#define DCOLTWOSIDE 8

#define carr_hexpref "0x"
#define carr_DSEPERATE " | "

static int
showHelp(const char* path, const char* opt[], const char* optdes[], int ret);
static const char *
basename(const char* ch);
static int
showErr(const char* err[], unsigned int index);

static void
dumpByte(FILE *sptr_fin,char* carr_buff, const unsigned int ui_col, const unsigned int ui_base, const unsigned int ui_len, const fpos_t start, const size_t length);

static void
dumpChar(FILE *sptr_fin,char* carr_buff, const unsigned int ui_col, const fpos_t start, const size_t length,const char *carr_stdc,const char *carr_stdc_str);

static void
dumpDual(FILE *sptr_fin, char* carr_buff, const unsigned int ui_col, const unsigned int ui_base, const unsigned int ui_len, const fpos_t start, const size_t length,const char *carr_stdc,const char *carr_stdc_str);

static const char* cpa_opt[] = { "-b", "-o", "-d", "-h",
    "-c:", "-s:", "-l:", "-B" ,"-A", "-T" , NULL };
enum _opt {
    e_optbin,
    e_optoct,
    e_optdec,
    e_opthex,
    e_optcol,
    e_optstart,
    e_optlength,
	e_optbyte,
    e_optascii,
    e_opttwoside
};
static const char* cpa_optdes[] = {
	" Binary Representation",
    " Octal Representation",
    " 10 base Representation",
    " Hex Representation",
    "{n} n=number of column",
    "{n} n=offset",
    "{n} n=Length",
	" Byte View",
    " ASCII View",
    " Dual view",
    NULL };

static const char* cpa_err[] = { 
	"Parameter isn't an unsigned interger",
    "File can't be accessed",
    "Column must more than 0",
    NULL };

enum _err {
    e_errpar,
    e_errfile,
    e_errzero
};

static const char carr_stdc[] = { '\0', '\a', '\b', '\f', '\n','\r', '\t', '\v', '\0', '\0' };

static const char carr_stdc_str[] = { '0', 'a', 'b', 'f', 'n','r', 't', 'v', '\0' };

int main(int argc, const char* argv[])
{
	static char carr_buff[BSIZE];
	
	FILE* sptr_fin;
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
                ui_col = DCOLTWOSIDE/2;

            break;

        case e_optoct:
            ui_base = 8;
            ui_len = 3;
			
            break;

        case e_optdec:
            ui_base = 10;
            ui_len = 3;		

            break;

        case e_opthex:
            ui_base = 16;
            ui_len = 2;

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
			
        case e_optbyte:
            i_actIndex = e_optbyte;
            break;
			
        case e_optascii:
            i_actIndex = e_optascii;
            break;

        case e_opttwoside:

            i_actIndex = e_opttwoside;

            break;
	

        case e_optother:
		
			if (!(sptr_fin = fopen(carr_buff, "rb"))) {
				fprintf(stderr, "FILE: %s\n", carr_buff);
				return showErr(cpa_err, e_errfile);
			}

            switch (i_actIndex)
			{

				case e_optascii:
					dumpChar(sptr_fin,carr_buff, ui_col, ui_start, ui_length, carr_stdc, carr_stdc_str);
					break;

				case e_opttwoside:
					dumpDual(sptr_fin,carr_buff, ui_col, ui_base, ui_len, ui_start, ui_length, carr_stdc, carr_stdc_str);
					break;

				default:
					dumpByte(sptr_fin,carr_buff, ui_col, ui_base, ui_len, ui_start, ui_length);
            }
			
			fclose(sptr_fin);

        }
    }

    return 0;
}

static int
showHelp(const char* path, const char* opt[], const char* optdes[], int ret)
{
    unsigned int i;
    fprintf(stderr, "\nUSAGE: %s {option} file1 file2 ..file(n)\n\n", basename(path));

    fprintf(stderr, "OPTIONS\n\n");
	
    for (i = 0; opt[i] && optdes[i]; i++)
        fprintf(stderr, "%s%s\n", opt[i], optdes[i]);

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

static void printoutheader(const char *carr_buff, const int fchar, const unsigned int length)
{
	unsigned int i;
	
    for (i = 0; i < length; i++)
        putchar(fchar);

    printf(" %s ", basename(carr_buff));

    for (i = 0; i < length; i++)
        putchar('=');
	
    putchar('\n');
}

static void printaddress(char *carr_buff,const unsigned int ui_col,const fpos_t start,const size_t raddress)
{
	if (!(raddress % ui_col)) {

        sT2s(raddress + start, carr_buff, BSIZE, OFFBASE, OFFLEN);

        printf("%s: ", carr_buff);
    }
}

static size_t printlinebyte(FILE *sptr_fin,char *carr_buff,
				const unsigned int ui_col,
				const unsigned int ui_base, const unsigned int ui_len,
				const size_t length,
				size_t raddress)
{
	const size_t beginraddress=raddress;
	int ch;
	
	for ( ; (raddress < beginraddress + ui_col) && ((ch = fgetc(sptr_fin)) != EOF && (raddress < length || length == -1)); ++raddress)
	{

        ui2s(ch, carr_buff, BSIZE, ui_base, ui_len);
        printf("%s%c", carr_buff, DELIM);

	}
	
	return raddress;
}

static void
dumpByte(FILE *sptr_fin, char* carr_buff, const unsigned int ui_col, const unsigned int ui_base,
    const unsigned int ui_len,
    const fpos_t start,
    const size_t length)
{
    size_t raddress=0;
	int ch;
	
	printoutheader(carr_buff,FCHAR,DLENGTH);


    if (fsetpos(sptr_fin,&start))
        return;

    while((ch=fgetc(sptr_fin))!=EOF && (raddress < length || length == -1)) {
		
		ungetc(ch,sptr_fin);
		
		printaddress(carr_buff,ui_col,start,raddress);
		
		raddress=printlinebyte(sptr_fin,carr_buff,ui_col,ui_base,ui_len,length,raddress);
		
        putchar('\n');
			
	}

}

static int
findStdC(int ch, const char* stdc)
{
    int i;
    for (i = 0; (stdc[i] != 0) || ((stdc[i + 1] != 0)); i++)
        if (ch == stdc[i])
            return i;


    return -1;
}

static size_t printlinechar(FILE *sptr_fin,
				const unsigned int ui_col,
				const size_t length,
				size_t raddress,const char *carr_stdc,const char *carr_stdc_str)
{
	const size_t beginraddress = raddress;
	int ch,k;
	
	for ( ; (raddress < beginraddress + ui_col) && ((ch = fgetc(sptr_fin)) != EOF && (raddress < length || length == -1)); ++raddress)
	{

        if ((k = findStdC(ch, carr_stdc)) > -1)
            printf("\\%c%c", carr_stdc_str[k], DELIM);

        else
            printf(" %c%c", ch, DELIM);

	}
	
	return raddress;
}

static void
dumpChar(FILE *sptr_fin, char* carr_buff, const unsigned int ui_col, const fpos_t start, const size_t length,const char *carr_stdc,const char *carr_stdc_str)
{
    size_t raddress=0;
	int ch;

	printoutheader(carr_buff,FCHAR,DLENGTH);

    if (fsetpos(sptr_fin,&start))
        return;

    while((ch=fgetc(sptr_fin))!=EOF && (raddress < length || length == -1)) {
		
		ungetc(ch,sptr_fin);
		
		printaddress(carr_buff,ui_col,start,raddress);
		
		raddress=printlinechar(sptr_fin,ui_col,length,raddress,carr_stdc,carr_stdc_str);
		
        putchar('\n');
		
	}
	
}

static void
dumpDual(FILE *sptr_fin, char* carr_buff, const unsigned int ui_col,
		const unsigned int ui_base, const unsigned int ui_len,
		const fpos_t start, const size_t length,
		const char *carr_stdc,const char *carr_stdc_str)
{
    unsigned int i, j;
    size_t raddress, beginraddress, column;
    fpos_t begpos, curpos;
	int ch;

	printoutheader(carr_buff,FCHAR,DLENGTH);

    if (fsetpos(sptr_fin,&start))
		return;

	fgetpos(sptr_fin,&curpos); raddress = 0;
	
    while((ch=fgetc(sptr_fin))!=EOF && (raddress < length || length == -1)) {
		
		ungetc(ch,sptr_fin);
		
        fgetpos(sptr_fin,&begpos);
		fsetpos(sptr_fin,&curpos);
		
		beginraddress=raddress;
		
		printaddress(carr_buff,ui_col,start,raddress);
		
		raddress=printlinebyte(sptr_fin,carr_buff,ui_col,ui_base,ui_len,length,raddress);
		column=raddress-beginraddress;

        if (!beginraddress && column && column < ui_col)
            printf(carr_DSEPERATE);

        else {

            if ((raddress % ui_col) && (ui_col < length) && (((raddress <= length) && (length != -1)) || (length == -1)))
                for (i = 0; i < (ui_col - raddress % ui_col); i++)
                    for (j = 0; j < ui_len + 1; j++)
                        printf("%c", DELIM);

            if (column)
                printf(carr_DSEPERATE);
            else
                return;
        }

        fgetpos(sptr_fin,&curpos);
        fsetpos(sptr_fin,&begpos);
		
		raddress=printlinechar(sptr_fin,ui_col,length,beginraddress,carr_stdc,carr_stdc_str);

		putchar('\n');

		
    }
	

}


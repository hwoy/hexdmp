#include <stdio.h>
#include "opt.h"
#include "function.h"

#define COL 16
#define BASE 16
#define BSIZE 1024
#define DELIM ','
#define LEN 2

#define OFFLEN 8
#define OFFBASE 16

static int showHelp (const char *path, const char *opt[],
		     const char *optdes[], int ret);
static unsigned int basename (const char *ch);
int showErr (const char *err[], unsigned int index);


enum _opt
{
  e_optbin, e_optoct, e_optten, e_opthex, e_optcol
};

enum _err
{
  e_errpar, e_errfile
};



static const char *cpa_opt[] = { "-b", "-o", "-t", "-h", "-c", NULL };
static const char *cpa_optdes[] =
  { "Binary show", "Octal Show", "10 base Show", "Hex Show", "Col", NULL };

static const char *cpa_err[] =
  { "Parameter isn't an unsigned interger", "File can't be accessed", NULL };


int
main (int argc, const char *argv[])
{
  static char carr_buff[BSIZE];
  unsigned int ui_cindex, ui_pindex, ui_base, ui_col, ui_len, ui_colflag;
  unsigned int i, j;
  int i_ch;
  FILE *sptr_fin;

  if (argc == 1)
    {
      return showHelp (argv[0], cpa_opt, cpa_optdes, 1);
    }
/******************* Set Default *********************/

  ui_base = BASE;
  ui_col = COL;
  ui_len = LEN;
  ui_colflag = 0;

/******************* Parameter Operation *********************/


  for (ui_cindex = DSTART;
       (ui_pindex =
	opt_action (argc, argv, cpa_opt, carr_buff, BSIZE,
		    DSTART)) != e_optend; ui_cindex++)
    {

      switch (ui_pindex)
	{

	case e_optbin:
	  ui_base = 2;
	  ui_len = 8;

	  if (!ui_colflag)
	    ui_col = 8;
	  break;

	case e_optoct:
	  ui_base = 8;
	  ui_len = 3;

	  if (!ui_colflag)
	    ui_col = 8;
	  break;

	case e_optten:
	  ui_base = 10;
	  ui_len = 3;

	  if (!ui_colflag)
	    ui_col = 10;
	  break;

	case e_opthex:
	  ui_base = 16;
	  ui_len = 2;

	  if (!ui_colflag)
	    ui_col = 16;
	  break;

	case e_optcol:
	  if (!isUint (carr_buff))
	    {
	      fprintf (stderr, "PARAM: %s\n", carr_buff);
	      return showErr (cpa_err, e_errpar);
	    }
	  ui_col = s2ui (carr_buff,10);
	  ui_colflag = 1;

	  break;

	case e_optother:
	  if (!(sptr_fin = fopen (carr_buff, "rb")))
	    {
	      fprintf (stderr, "FILE: %s\n", carr_buff);
	      return showErr (cpa_err, e_errfile);
	    }
	  i = 0;
	  j = 0;
	  while ((i_ch = fgetc (sptr_fin)) != EOF)
	    {
	      if (!((j++) % ui_col))
		{
		  ui2s (j - 1, carr_buff, BSIZE, OFFBASE, OFFLEN);
		  printf ("%s: ", carr_buff);
		}

	      ui2s (i_ch, carr_buff, BSIZE, ui_base, ui_len);
	      printf ("%s%c", carr_buff, DELIM);

	      if ((ui_col) && (++i > ui_col - 1))
		{
		  putchar ('\n');
		  i = 0;
		}
	    }
	  putchar ('\n');
	  fclose (sptr_fin);
	  return 0;


	}

    }


  return 0;
}

static int
showHelp (const char *path, const char *opt[], const char *optdes[], int ret)
{
  unsigned int i;
  fprintf (stderr, "\nUSAGE: %s {option} file\n\n", path + basename (path));

  fprintf (stderr, "OPTIONS\n");
  for (i = 0; opt[i] && optdes[i]; i++)
    {
      fprintf (stderr, "%s =\t%s\n", opt[i], optdes[i]);
    }
  fputc ('\n', stderr);

  return ret;
}

static unsigned int
basename (const char *ch)
{
  unsigned int i, j;
  for (i = 0, j = 0; ch[i]; i++)
    {
      if (ch[i] == '\\' || ch[i] == '/')
	{
	  j = i;
	}
    }
  return (j == 0) ? 0 : j + 1;
}

int
showErr (const char *err[], unsigned int index)
{

  fprintf (stderr, "ERROR NO %u: %s\n", index, err[index]);
  return index + 1;
}

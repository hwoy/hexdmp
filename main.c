#include <stdio.h>
#include <string.h>
#include "opt.h"
#include "function.h"

#define COL 16
#define BASE 16
#define BSIZE 1024
#define DELIM 0x20
#define LEN 2

#define FCHAR '='

#define OFFLEN (sizeof(unsigned int)*8/4)
#define OFFBASE 16

#define DLENGTH 2


static int showHelp (const char *path, const char *opt[],
		     const char *optdes[], int ret);
static unsigned int basename (const char *ch);
int showErr (const char *err[], unsigned int index);
void dumpByte (char *carr_buff, unsigned int ui_col, unsigned int ui_base,
	       unsigned int ui_len, unsigned int start, unsigned int length);
void dumpChar (char *carr_buff, unsigned int ui_col, unsigned int start,
	       unsigned int length);

int findStdC (int ch, const char *stdc);


enum _opt
{
  e_optbin, e_optoct, e_optten, e_opthex, e_optascii, e_optcol, e_optstart,
  e_optlength
};

enum _err
{
  e_errpar, e_errfile, e_errzero
};


static const char carr_hexpref[] = "0x";


static const char *cpa_opt[] =
  { "-b", "-o", "-t", "-h", "-a", "-c:", "-s:", "-l:", NULL };


static const char *cpa_optdes[] =
  { "Binary show", "Octal Show", "10 base Show", "Hex Show", "ASCII Show",
  "Col -c:{n} n=number of column", "-s:{n} Start to offset n",
  "-l{n} n=Length of byte for watching", NULL
};

static const char *cpa_err[] =
  { "Parameter isn't an unsigned interger", "File can't be accessed",
  "Column must more than 0", NULL
};


static const char carr_stdc[] =
  { '\0', '\a', '\b', 'f', '\n', '\r', '\t', '\v', '\0', '\0' };
static const char carr_stdc_str[] =
  { '0', 'a', 'b', 'f', 'n', 'r', 't', 'v', '\0' };

int
main (int argc, const char *argv[])
{
  static char carr_buff[BSIZE];
  unsigned int ui_cindex, ui_pindex, ui_base, ui_col, ui_len, ui_colflag,
    ui_asciiflag, ui_start, ui_length;
  unsigned int i;

  if (argc == 1)
    {
      return showHelp (argv[0], cpa_opt, cpa_optdes, 1);
    }
/******************* Set Default *********************/

  ui_base = BASE;
  ui_col = COL;
  ui_len = LEN;
  ui_colflag = 0;
  ui_asciiflag = 0;
  ui_start = 0;
  ui_length = (unsigned int) -1;

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

	case e_optascii:
	  ui_asciiflag = 1;
	  break;

	case e_optcol:
	  if (!isUint (carr_buff))
	    {
	      fprintf (stderr, "PARAM: %s\n", carr_buff);
	      return showErr (cpa_err, e_errpar);
	    }

	  if (!(ui_col = s2ui (carr_buff, 10)))

	    {
	      fprintf (stderr, "PARAM: %s\n", carr_buff);
	      return showErr (cpa_err, e_errzero);
	    }
	  ui_colflag = 1;

	  break;

	case e_optstart:
	  if (!isUint (carr_buff)
	      && (!isUintHex (&carr_buff[strlen (carr_hexpref)])
		  || strncmp (carr_buff, carr_hexpref,
			      strlen (carr_hexpref))))
	    {
	      fprintf (stderr, "PARAM: %s\n", carr_buff);
	      return showErr (cpa_err, e_errpar);
	    }
	  i =
	    (!strncmp (carr_buff, carr_hexpref, strlen (carr_hexpref))) ? 16 :
	    10;
	  ui_start =
	    s2ui (&carr_buff[(i == 16) ? strlen (carr_hexpref) : 0], i);

	  break;

	case e_optlength:
	  if (!isUint (carr_buff)
	      && (!isUintHex (&carr_buff[strlen (carr_hexpref)])
		  || strncmp (carr_buff, carr_hexpref,
			      strlen (carr_hexpref))))
	    {
	      fprintf (stderr, "PARAM: %s\n", carr_buff);
	      return showErr (cpa_err, e_errpar);
	    }
	  i =
	    (!strncmp (carr_buff, carr_hexpref, strlen (carr_hexpref))) ? 16 :
	    10;
	  ui_length =
	    s2ui (&carr_buff[(i == 16) ? strlen (carr_hexpref) : 0], i);

	  break;

	case e_optother:

	  if (ui_asciiflag)
	    dumpChar (carr_buff, ui_col, ui_start, ui_length);
	  else
	    dumpByte (carr_buff, ui_col, ui_base, ui_len, ui_start,
		      ui_length);
	  break;


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

void
dumpByte (char *carr_buff, unsigned int ui_col, unsigned int ui_base,
	  unsigned int ui_len, unsigned int start, unsigned int length)
{
  unsigned int i, j;
  int i_ch;
  FILE *sptr_fin;
  if (!(sptr_fin = fopen (carr_buff, "rb")))
    {
      fprintf (stderr, "FILE: %s\n", carr_buff);
      showErr (cpa_err, e_errfile);
      return;
    }

  for (i = 0; i < DLENGTH; i++)
    putchar (FCHAR);

  printf (" %s ", carr_buff + basename (carr_buff));

  for (i = 0; i < DLENGTH; i++)
    putchar ('=');
  putchar ('\n');


  for (i = 0, j = 0; (i_ch = fgetc (sptr_fin)) != EOF; j++)
    {

      if (j < start)
	continue;

      if (j >= (start + length) && (length != (unsigned int) -1))
	{
	  putchar ('\n');
	  return;
	}

      if (!((j - start) % ui_col))
	{
	  ui2s (j, carr_buff, BSIZE, OFFBASE, OFFLEN);
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

}

void
dumpChar (char *carr_buff, unsigned int ui_col, unsigned int start,
	  unsigned int length)
{
  unsigned int i, j;
  int i_ch;
  int k;
  FILE *sptr_fin;
  if (!(sptr_fin = fopen (carr_buff, "rb")))
    {
      fprintf (stderr, "FILE: %s\n", carr_buff);
      showErr (cpa_err, e_errfile);
      return;
    }


  for (i = 0; i < DLENGTH; i++)
    putchar (FCHAR);

  printf (" %s ", carr_buff + basename (carr_buff));

  for (i = 0; i < DLENGTH; i++)
    putchar ('=');
  putchar ('\n');


  for (i = 0, j = 0; (i_ch = fgetc (sptr_fin)) != EOF; j++)
    {

      if (j < start)
	continue;

      if (j >= (start + length) && (length != (unsigned int) -1))
	{
	  putchar ('\n');
	  return;
	}
      if (!((j - start) % ui_col))
	{
	  ui2s (j, carr_buff, BSIZE, OFFBASE, OFFLEN);
	  printf ("%s: ", carr_buff);
	}
      if ((k = findStdC (i_ch, carr_stdc)) > 0)
	printf ("\\%c%c", carr_stdc_str[k], DELIM);

      else
	printf ("%c%c%c", i_ch, DELIM, DELIM);

      if ((ui_col) && (++i > ui_col - 1))
	{
	  putchar ('\n');
	  i = 0;
	}
    }
  putchar ('\n');
  fclose (sptr_fin);

}

int
findStdC (int ch, const char *stdc)
{
  int i;
  for (i = 0; (stdc[i] != 0) || ((stdc[i + 1] != 0)); i++)
    {
      if (ch == stdc[i])
	return i;
    }

  return -1;
}

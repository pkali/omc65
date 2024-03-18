// compiled with gcc (Ubuntu 5.5.0-12ubuntu1~16.04) 5.5.0 20171010
// gcc -x c  OMC_MAIN.C -o omc
/*#define atariST*/
#define BUFLEN 120
#define asem_col 19
#define _MAX_PATH 260        // this is a hack to replace win32 value
#define strnicmp strncasecmp // this is a hack to replace deprecated strnicmp
#define _getcwd getcwd       // ditto
#define _chdir chdir         // ditto

#define lnf 10
char crt = 13, tab = '\t';

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <curses.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

long myfilelength(FILE *f) { // don't use that. Learn POSIX API
  long prev = ftell(f);
  fseek(f, 0L, SEEK_END);
  long sz = ftell(f);
  fseek(f, prev, SEEK_SET); // go back to where we were
  return sz;
}

void strup(char *s) {
  while (*s) {
    *s = toupper(*s);
    ++s;
  }
}

struct mnem {
  uint8_t mnem[4];
  uint8_t mlen;
  uint8_t modes[13];
};
struct mnem mnemoniki[56] = {
    /*
    ins/len/wewn/bxx /akum/hash/ Q  / Z  /Q,x /Z,x /Q,y /Z,y/(Z,x)/(Z),y/(Q) */
    {"adc", 0, 0xff, 0xff, 0xff, 0x69, 0x6d, 0x65, 0x7d, 0x75, 0x79, 0xff, 0x61, 0x71, 0xff},
    {"and", 0, 0xff, 0xff, 0xff, 0x29, 0x2d, 0x25, 0x3d, 0x35, 0x39, 0xff, 0x21, 0x31, 0xff},
    {"asl", 0, 0xff, 0xff, 0x0a, 0xff, 0x0e, 0x06, 0x1e, 0x16, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"bcc", 2, 0xff, 0x90, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"bcs", 2, 0xff, 0xb0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"beq", 2, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"bit", 0, 0xff, 0xff, 0xff, 0xff, 0x2c, 0x24, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"bmi", 2, 0xff, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"bne", 2, 0xff, 0xd0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"bpl", 2, 0xff, 0x10, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"brk", 1, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"bvc", 2, 0xff, 0x50, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"bvs", 2, 0xff, 0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"clc", 1, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"cld", 1, 0xd8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"cli", 1, 0x58, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"clv", 1, 0xb8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"cmp", 0, 0xff, 0xff, 0xff, 0xc9, 0xcd, 0xc5, 0xdd, 0xd5, 0xd9, 0xff, 0xc1, 0xd1, 0xff},
    {"cpx", 0, 0xff, 0xff, 0xff, 0xe0, 0xec, 0xe4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"cpy", 0, 0xff, 0xff, 0xff, 0xc0, 0xcc, 0xc4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"dec", 0, 0xff, 0xff, 0xff, 0xff, 0xce, 0xc6, 0xde, 0xd6, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"dex", 1, 0xca, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"dey", 1, 0x88, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"eor", 0, 0xff, 0xff, 0xff, 0x49, 0x4d, 0x45, 0x5d, 0x55, 0x59, 0xff, 0x41, 0x51, 0xff},
    {"inc", 0, 0xff, 0xff, 0xff, 0xff, 0xee, 0xe6, 0xfe, 0xf6, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"inx", 1, 0xe8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"iny", 1, 0xc8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"jmp", 3, 0xff, 0xff, 0xff, 0xff, 0x4c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x6c},
    {"jsr", 3, 0xff, 0xff, 0xff, 0xff, 0x20, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"lda", 0, 0xff, 0xff, 0xff, 0xa9, 0xad, 0xa5, 0xbd, 0xb5, 0xb9, 0xff, 0xa1, 0xb1, 0xff},
    {"ldx", 0, 0xff, 0xff, 0xff, 0xa2, 0xae, 0xa6, 0xff, 0xff, 0xbe, 0xb6, 0xff, 0xff, 0xff},
    {"ldy", 0, 0xff, 0xff, 0xff, 0xa0, 0xac, 0xa4, 0xbc, 0xb4, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"lsr", 0, 0xff, 0xff, 0x4a, 0xff, 0x4e, 0x46, 0x5e, 0x56, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"nop", 1, 0xea, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"ora", 0, 0xff, 0xff, 0xff, 0x09, 0x0d, 0x05, 0x1d, 0x15, 0x19, 0xff, 0x01, 0x11, 0xff},
    {"pha", 1, 0x48, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"php", 1, 0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"pla", 1, 0x68, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"plp", 1, 0x28, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"rol", 0, 0xff, 0xff, 0x2a, 0xff, 0x2e, 0x26, 0x3e, 0x36, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"ror", 0, 0xff, 0xff, 0x6a, 0xff, 0x6e, 0x66, 0x7e, 0x76, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"rti", 1, 0x40, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"rts", 1, 0x60, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"sbc", 0, 0xff, 0xff, 0xff, 0xe9, 0xed, 0xe5, 0xfd, 0xf5, 0xf9, 0xff, 0xe1, 0xf1, 0xff},
    {"sec", 1, 0x38, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"sed", 1, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"sei", 1, 0x78, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"sta", 0, 0xff, 0xff, 0xff, 0xff, 0x8d, 0x85, 0x9d, 0x95, 0x99, 0xff, 0x81, 0x91, 0xff},
    {"stx", 0, 0xff, 0xff, 0xff, 0xff, 0x8e, 0x86, 0xff, 0xff, 0x96, 0xff, 0xff, 0xff, 0xff},
    {"sty", 0, 0xff, 0xff, 0xff, 0xff, 0x8c, 0x84, 0x94, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"tax", 1, 0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"tay", 1, 0xa8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"tsx", 1, 0xba, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"txa", 1, 0x8a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"txs", 1, 0x9a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"tya", 1, 0x98, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
char mnem_hash_tab[0x200];
/*tablica numerow mnemonikow dla kolejnych wartosci fcji faszujacej
  inicjowana przez init_tabs
*/

char line_bufor[BUFLEN + 2], strbuf[BUFLEN];
char *bufor; /*wsk. bufor linii*/

uint16_t bptr; /* wsk. bajtu w buforze wyjsciowy */
uint16_t buf_out_len;

int16_t col;
long line;
int licz_list, char_out;
int numcnt, page_cnt;
unsigned char prn_ln_cnt;
int prn_lin; /*znacznik 0 gdy linia byla juz printowana*/
int objflg, listflg, errflg, ejectflg, mlistflg, clistflg, numflg, xrefflg;
int lstf; /*znacznik czy listowac ,ust. przez param. assembl.*/
int err_num_only;
char set_dat[5], tab_dat[3];
char title_dat[BUFLEN], page_dat[BUFLEN];
int asmflg;        /*1-asemblowac 0 nie ,zmieniany przez .if .else .endif */
int i_stack[15];   /* stos komend dla asemblacji warunkowej */
int asm_stack[15]; /* stos decyzji dla asemblacji warunkowej */
int isp;           /*wskaznik stosu dla .if */
#define maxfnr 8
FILE *filestack[maxfnr];
int fsp;
char sciezka[_MAX_PATH + 1];
char *fpthstack[maxfnr];
long linestack[maxfnr];
long line_inc;
char *filepath;

int phase;
unsigned char localcnt;

#define max_rept_block 0x2000
uint16_t adres, beg_rept;
int reptflg;
uint16_t cnt_rept, rept_len;
char (*rept_buf)[];

struct lab {
  char lenght, *gdzie;
  uint16_t labvalue;
  int def : 1;
  int dup : 1;     /*dup label indicator*/
  int ref : 1;     /*znacznik dla .REF*/
  int localnr : 8; /*numer obszaru lokalnego,tylko dla lokalnych*/
  int tmp : 1;     /* 0 etyk. zwykla, 1 .= */
  struct lab *next;
};
/* def  1 jesli etykieta ma sensowna wartosc
   dup  1 jesli (w drugim przebiegu) wystapilo juz
           podstawienie wartosci na etykiete
   ref  1 gdy w pierszej fazie byla dana etykieta i wytapilo .REF
*/
struct lab_list {
  struct lab *firstlab;
  struct lab *lastlab;
} lab_ptrs[0x0800];
/*tablica wsk. list dla kolejnych wartosci dla funkcji haszujacej
 */

void print_line(int mhead);
void analizing(void);
void putcode(void);

// void macro_analizing(struct macro *mptr);
struct macro *is_macro(char *mname, int ile);
/* spr. czy jest takie macro
 */

void macro_analizing(struct macro *mptr);

int inside_macro;
/* znacznik czy jest. akt. asemblowany macrorozkaz
   uzywany tez do spr. czy w porz. linii bylo macro
*/
int macro_nest;
/* znacznik zagl. macro uzywane do printowania w drugiej fazie*/

#define max_psp 128
int param[max_psp];        /*stos parametrow*/
char *text_param[max_psp]; /* stos parametrow textowych*/
char is_param[max_psp];    /*stos znacznikow 0 nieokr. 1 zwykly 2 textowy*/
int psp, first_par;        /*wskazniki na stos parametrow*/

struct macroline {
  char *linebody;
  struct macroline *nxt;
};
struct macro {
  unsigned char namelen;
  char *mname;
  struct macroline *mbody, *last_line;
  struct macro *next;
};
struct mac_list {
  struct macro *firstmacro;
  struct macro *lastmacro;
} mac_ptrs[0x800];

FILE *source, *dest;
FILE *flist, *ferr;

#define OUTPUT_STDOUT 1
#define OUTPUT_FILE 2
#define OUTPUT_PRN 3
int g_list_output;
int g_err_output;
unsigned int err_cnt;
char *errpath; /* wsk na sciezke dla pliku z bledami*/
int fatal;
int adresbytes; /*0 tylko kod,1 z bajtami adresowymi form. Atari xl*/

/* texty */
char our5oft[] = "\n OMC - Our 5oft MaCro assembler for 6502, ver. 1.2, (c) "
                 "1993 by Our 5oft.\n";
char _macro[] = "macro", _endm[] = "endm";
char _local[] = "local";
char _byte[] = "byte", _word[] = "word", _dbyte[] = "dbyte", _opt[] = "opt";
char _float[] = "float";
char _include[] = "include", _error[] = "error", _set[] = "set", _tab[] = "tab";
char _incbin[] = "incbin";
char _title[] = "title", _page[] = "page";
char _if[] = "if", _else[] = "else", _endif[] = "endif";
char _rept[] = "rept", _endr[] = "endr";
int hextab[256];
char fcltab[256], labtab[256], fclnum[256], hash_tab[256];

//#include "tab_ini.c"
int hash_mnem(uint8_t *co) {
  /*haszowanie mneminikow*/
  int h;
  h = (hash_tab[*co] << 4);
  h += hash_tab[*(co + 1)];
  h += (hash_tab[*(co + 2)] - 3) << 3;
  h -= hash_tab[*(co + 2)] & 1;
  return h & 0x1ff;
}
void init_mnem_hash_tab(void) {
  int i, h;
  for (i = 0; i < 0x200; i++)
    mnem_hash_tab[i] = 0;
  /*zero dla wszystkich wartosci nie generowanych przez mnemoniki
    wsk. na mnemonik adc
  */
  for (i = 0; i < 56; i++) {
    h = hash_mnem(mnemoniki[i].mnem);
    mnem_hash_tab[h] = (char)i;
  }
}

void init_tabs(void) {
  int k;
  /*----- hex tab ------*/
  for (k = 0; k < '0'; k++)
    hextab[k] = 0x7f;
  for (k = '0'; k <= '9'; k++)
    hextab[k] = k - '0';
  for (k = '9' + 1; k < 'A'; k++)
    hextab[k] = 0x7f;
  for (k = 'A'; k <= 'F'; k++)
    hextab[k] = k - 'A' + 10;
  for (k = 'G'; k < 'a'; k++)
    hextab[k] = 0x7f;
  for (k = 'a'; k <= 'f'; k++)
    hextab[k] = k - 'a' + 10;
  for (k = 'g'; k <= 0xff; k++)
    hextab[k] = 0x7f;
  /*-----first character of label-fcltab----- */

  for (k = 0; k <= 255; k++)
    fcltab[k] = 0;

  for (k = '?'; k <= 'Z'; k++)
    fcltab[k] = 1;
  for (k = 'a'; k <= 'z'; k++)
    fcltab[k] = 1;

  /*----- first char of lab or number-------- */
  for (k = 0; k <= 255; k++)
    fclnum[k] = 0;
  for (k = '0'; k <= '9'; k++)
    fclnum[k] = 1;
  fclnum[39] = 1;
  fclnum['$'] = 1;
  fclnum['%'] = 1;
  fclnum[':'] = 1;
  for (k = '?'; k <= 'Z'; k++)
    fclnum[k] = 1;
  for (k = 'a'; k <= 'z'; k++)
    fclnum[k] = 1;

  /* -------could be a label-labtab--------- */
  for (k = 0; k <= 255; k++)
    labtab[k] = 0;

  labtab['.'] = 1;
  for (k = '0'; k <= '9'; k++)
    labtab[k] = 1;
  for (k = '?'; k <= 'Z'; k++)
    labtab[k] = 1;
  for (k = 'a'; k <= 'z'; k++)
    labtab[k] = 1;
  /* ------- Hash tab--------- */
  for (k = 0; k <= 255; k++)
    hash_tab[k] = 0;

  hash_tab['.'] = 1;
  for (k = '0'; k <= '9'; k++)
    hash_tab[k] = k - '0' + 2;
  for (k = '?'; k <= 'Z'; k++)
    hash_tab[k] = k - '?' + 12;
  for (k = 'a'; k <= 'z'; k++)
    hash_tab[k] = k - 'a' + 14;

  for (k = 0; k < 0x0800; k++) {
    lab_ptrs[k].firstlab = NULL;
    lab_ptrs[k].lastlab = NULL;
    mac_ptrs[k].firstmacro = NULL;
    mac_ptrs[k].lastmacro = NULL;
  }
  init_mnem_hash_tab();
}

// end include tab_ini.c

char asctoscr(char c)
/* zamienia kod ASCII znaku na kod ekranowy
 */
{
  char aincc[] = {0x40, 0x00, 0x20, 0x60};
  char c1;
  c1 = (c >> 5) & 3;
  return aincc[c1] | (c & 0x9f);
}

void error(const char *text) {

  if (!errflg && !fatal)
    return;
  if (err_cnt++ == 0 && (ferr == NULL || g_err_output == OUTPUT_PRN)) {
    if (ferr == NULL) {
      if ((ferr = fopen(errpath, "w")) == NULL) {
        fprintf(flist, "FATAL ERROR - can't open error file\n");
        fatal = 1;
        return;
      }
    }
    if (!err_num_only) {
      fprintf(ferr, "%s", our5oft);
      fprintf(ferr, "%s", "error list:\n");
    }
  }

  if ((!listflg || flist != ferr) && !fatal && !err_num_only) {
    if (inside_macro)
      fprintf(ferr, "  M  ");
    fprintf(ferr, "%s", bufor);
  } else
    print_line(0);
  if (fatal)
    fprintf(ferr, "FATAL ERROR - %s\n", text);
  else
    fprintf(ferr, " -ERROR! %s %s %ld: %s\n", filepath, "line", line_inc, text);
  exit(1);
}

/*allokacja pamieci ze sprawdzeniem*/
void *m_alloc(size_t size) {
  void *ret;
  ret = malloc(size);
  if (ret == NULL) {
    fatal = 1;
    error("Out of memory");
  }
  return ret;
}

//#include "labels.h"
int16_t hash_it(char *co, int16_t ile) {
  /*Hashowanie stringu wskazanego przez co, o dlugosci ile
   */
  int16_t h;
  h = 0;
  while (ile--) {
    h = h << 1;
    h += hash_tab[*co];
    co++;
  };
  return h & 0x07ff;
}
int thesame(const char *s1, const char *s2, char ile)
/* porownywanie bez wzgledu na wielkosc liter, zwraca 1 gdy takie same */
{
  register char i, c1, c2;
  i = 0;
  do {
    if ((c1 = s1[i]) != (c2 = s2[i])) {
      if (c1 < 91)
        c1 += 32;
      if (c2 < 91)
        c2 += 32;
      if (c1 != c2)
        return 0;
    }
  } while ((++i) < ile);
  return 1;
}

void addlabel(char *eptr, char len, int hash, int16_t labval, int def, int tmp)

/* Dolanczanie nowej etykiety do listy
   def 1 jesli jest to defincja
       0 jesli jest to odwolanie do etykiety
   tmp 0 zwykla etykieta
       1 etykieta tymczasowa(zmienna) .=
*/

{
  struct lab *labptr;
  struct lab_list *lb_lst;
  char *nameptr;

  labptr = (struct lab *)m_alloc(sizeof(struct lab));

  lb_lst = &lab_ptrs[hash];
  if (lb_lst->firstlab == NULL)
    lb_lst->firstlab = labptr;
  else
    lb_lst->lastlab->next = labptr;
  lb_lst->lastlab = labptr;

  nameptr = (char *)m_alloc(len);
  labptr->lenght = len;
  labptr->gdzie = nameptr;
  labptr->next = NULL;
  labptr->labvalue = labval;
  labptr->def = (def == 1); /* gdy def=2 to nie znamy wartosci */
  if ((phase == 1) || !def)
    labptr->dup = 0;
  else
    labptr->dup = 1;
  labptr->ref = 0;
  labptr->localnr = localcnt; /* ma znaczenie tylko dla lokalnych */
  labptr->tmp = tmp;
  memmove((void *)nameptr, (void *)eptr, len); /*memmove (dest,src,len)*/
                                               /*wypisywanie etykiety*/
  /*
     memmove((void*)strbuf,(void*)eptr,len);
     strbuf[len]=0;
     fprintf(flist,"%s=$%x\n",strbuf,labval);
  */
}

int ref_label(char *eptr, char ile)
/* sprawdza czy jest etykieta wskazywana przez eptr o dlug. ile
   jesli jest w pierwszej fazie to ustawia znaczn. ref i zwr. 1
   jesli jest w drugiej fazie do zwraca znaczn. ref.(0 lub 1)
   jesli nie to 0
*/
{
  struct lab *labptr;

  labptr = lab_ptrs[hash_it(eptr, ile)].firstlab;
  while (labptr != NULL) {
    /* spr. czy taka sam dlugosc,taka sam nazwa
       i jezeli localna to czy jestesmy w jej obszarze
       (na liscie moze byc kilka etykiet lokalnych o takiej samej
        nazwie a roznych obszarach zdefiniowania)
    */
    if (labptr->lenght == ile && thesame(eptr, labptr->gdzie, ile) &&
        (*labptr->gdzie != '?' || labptr->localnr == localcnt)) {
      if (phase == 2)
        return labptr->ref;
      labptr->ref = 1;
      return 1;
    }
    labptr = labptr->next;
  }
  return 0; /*nie ma*/
}

int is_label(char *eptr, char ile, int16_t *value, int16_t val, int def, int tmp)

/* sprawdza czy jest etykieta wskazywana przez eptr o dlug. ile
    -jesli nie ma to dolancza
    -jesli nie ma lub jest niezdefiniowana to zwraca 0
    -jesli jest zdefiniowana zwraca 1 i
     wartosc etykiety poprzez wskaznik value

   def 1-definicja 2-defincja bez wartosci
       0-sprawdzenie(odwolanie)
   value wskaznik na zwracana wartosc uzywany gdy etykieta byla zdef..
*/
{
  struct lab *labptr;
  int hash; /*wartoscfunkcji haszujacej*/

  labptr = lab_ptrs[hash = hash_it(eptr, ile)].firstlab;
  while (labptr != NULL) {
    /* spr. czy taka sam dlugosc,taka sama nazwa
       i jezeli localna to czy jestesmy w jej obszarze
       (na liscie moze byc kilka etykiet lokalnych o takiej samej
        nazwie a roznych obszarach zdefiniowania)
    */
    if (labptr->lenght == ile && thesame(eptr, labptr->gdzie, ile) &&
        (*labptr->gdzie != '?' || labptr->localnr == localcnt)) {
      if (def) /*definicja*/
      {
        if (labptr->def) {
          if (phase == 2) {
            if (labptr->dup && (!labptr->tmp || !tmp))
              error("Duplicate label");
            else
              labptr->dup = 1;
          }
          *value = labptr->labvalue; /* dotychczasowa wartosc */
          labptr->labvalue = val;
          return 1; /* zdefiniowana,bylo odwolanie*/
        } else {
          if (def != 2) {
            labptr->def = 1; /*definicja*/
            labptr->labvalue = val;
            labptr->tmp = tmp;
          }
          if (phase == 2)
            labptr->dup = 1;
          return 0; /* nie byla zdefiniowana */
        }
      } else /*sprawdzenie*/
      {
        if (!labptr->def)
          return 0;                /* nie zdefiniowana */
        *value = labptr->labvalue; /*zwraca wartosc*/
        return 1;                  /*zdefiniowana*/
      }
    }
    labptr = labptr->next;
  }
  addlabel(eptr, ile, hash, val, def, tmp);

  return 0;
}

void clearlabels(void) {
  int k;
  struct lab *labptr, *firstlab;

  for (k = 0; k < 0x0800; k++) {
    firstlab = lab_ptrs[k].firstlab;
    /*  if (firstlab!=NULL&&listflg) fprintf(flist,"------------------\n");
     */
    while (firstlab != NULL) {
      /*
      char l;
            kb_wait();
            memmove ((void *)strbuf,(void
      *)firstlab->gdzie,(l=firstlab->lenght)); if (listflg) {strbuf[l]=0;
               fprintf(flist,"%s=$%x\n",strbuf,firstlab->labvalue);
              }
      */
      labptr = firstlab;
      firstlab = firstlab->next;
      free(labptr->gdzie);
      free(labptr);
    }
  }
}

struct lab *lab_insert(struct lab *orderlist, struct lab *labptr) {
  /*adds label to label list in alfabetical order.*/
  int cmp;
  struct lab *lbpt;
  lbpt = orderlist;
  if (lbpt == NULL) {
    labptr->next = NULL;
    return labptr;
  }
  if ((cmp = strnicmp(lbpt->gdzie, labptr->gdzie, (lbpt->lenght > labptr->lenght) ? labptr->lenght : lbpt->lenght)) >
          0 ||
      cmp == 0 && lbpt->lenght >= labptr->lenght) {
    labptr->next = lbpt;
    return labptr;
  }
  while (lbpt->next != NULL &&
         ((cmp = strnicmp(lbpt->next->gdzie, labptr->gdzie,
                          (lbpt->next->lenght > labptr->lenght) ? labptr->lenght : lbpt->next->lenght)) < 0 ||
          cmp == 0 && lbpt->next->lenght < labptr->lenght))
    lbpt = lbpt->next;
  labptr->next = lbpt->next;
  lbpt->next = labptr;
  return orderlist;
}

void printlabels(void) {
  int k;
  char l, cnt;
  struct lab *labptr, *firstlab, *orderlist;
  orderlist = NULL;
  for (k = 0; k < 0x0800; k++) {
    firstlab = lab_ptrs[k].firstlab;
    while (firstlab != NULL) {
      labptr = firstlab;
      firstlab = firstlab->next;
      orderlist = lab_insert(orderlist, labptr);
    }
  }
  fprintf(flist, "\n defined labels:\n");
  cnt = 0;
  while (orderlist != NULL) {
    if (!fatal) {
      memmove((void *)strbuf, (void *)orderlist->gdzie, (l = orderlist->lenght));
      strbuf[l] = 0;
      fprintf(flist, " %04X %s", orderlist->labvalue, strbuf);
      if (!((++cnt) & 3))
        fprintf(flist, "\n");
      else
        for (; l < 14; l++)
          fprintf(flist, " ");
    }
    labptr = orderlist;
    orderlist = orderlist->next;
    free(labptr->gdzie);
    free(labptr);
  }
}
// end include labels.c

/*obliczanie wartosci wyrazen */
//#include "argvalue.c"
/*
oblicza wartosc wyrazen z liczb typu int.
 v1.51 z nawiasami"[]",operatorami starszego i mlodszego bajtu"<>"
 i liczbami ujemnymi i operatorami .def i .ref,
 klejnosc operatorow jak w MAC\65
 ze wskaznikiem na bufor linii
*/
char os[40], hist[80];
int nsp, osp, hsp;
uint16_t ns[40];
#define unarny(c) ((c) == '<' || (c) == '>' || (c) == '!' || (c) == '~')

int level(char j) {

  switch (j) {
  case '<':
  case '>':
    return 1;
  case ':':
    return 2;
  case '@':
    return 3;
  case '=':
  case 'r':
  case 'm':
  case 'w':
  case '{':
  case '}':
    return 4;
  case '|':
  case '&':
  case '^':
    return 5;
  case '+':
  case '-':
  case '!':
    return 6;
  case '*':
  case '/':
    return 7;
  case '~':
    return 8;
  case '[':
  case ']':
    return 9;

  default:
    return 0;
  }
}

/* sprawdza czy wskazany ciag znakow jest etykieta
   jest tak to zwraca 1 i jej wartosc poprzez wskaznik value
   jesli nie to zwraca 0
*/
int findlabel(int16_t *j, int ile, int16_t *value) {
  char lablen, *beg;

  lablen = 1;
  beg = &bufor[(*j)++];
  while (labtab[bufor[(*j)]] && (*j) < ile) {
    (*j)++;
    lablen++;
  }
  return (is_label(beg, lablen, value, 0, 0, inside_macro));
  /*0-odwolanie do etykiety, w macroinsr. kazda jest tymczasowa */
}

char getoperator(int16_t *j) {
  char c, *bf;

  bf = &bufor[(*j)];
  if ((*bf) == '.') {
    if (thesame(bf + 1, "or", 2)) {
      (*j) += 3;
      return ':';
    } /* logical OR */
    if (thesame(bf + 1, "and", 3)) {
      (*j) += 4;
      return '@';
    } /* logical AND */
    if (thesame(bf + 1, "not", 3)) {
      (*j) += 4;
      return '~';
    }         /* logical NOT */
    return 0; /* zly operator */
  }
  if (thesame(bf, "<=", 2)) {
    (*j) += 2;
    return '{';
  }
  if (thesame(bf, ">=", 2)) {
    (*j) += 2;
    return '}';
  }
  if (thesame(bf, "<>", 2)) {
    (*j) += 2;
    return 'r';
  }

  /* jezeli poprzednio byla liczba to < i > oznacz. opr. porownania
   */
  if (hsp != 0 && ((c = hist[hsp - 1]) == 'n' || c == ']'))
    switch (*bf) {
    case '<':
      (*j)++;
      return 'm';
    case '>':
      (*j)++;
      return 'w';
    }
  switch (*bf) {
  case '<':
  case '>':
  case '=':
  case '|':
  case '&':
  case '^':
  case '+':
  case '-':
  case '*':
  case '/':
  case '[':
  case ']':
    return bufor[(*j)++];
  }
  return 0; /*zly operator */
}

int paramnr(int16_t *j, int ile, int16_t *nr) {
  /* odczytuje nr. parametru macra i zwraca poprzez wsk. nr
     zwraca 1 gdy dobrze
     0 gdy blad
  */
  char c, firstchar;

  if (bufor[*j] == '(') {
    (*j)++;
    firstchar = *j;
    while ((c = bufor[*j]) != ')' && (*j) < ile)
      (*j)++;
    if (c != ')')
      return 0;
    return is_label(&bufor[firstchar], (*j) - firstchar, nr, 0, 0, 0);
  }
  if (hextab[bufor[(*j)]] > 9)
    return 0;
  *nr = 0;
  while (hextab[c = bufor[*j]] <= 9 && (*j) < ile) {
    *nr = *nr * 10 + c & 0x0f;
    (*j)++;
  }
  return 1;
}

int getnumber(int16_t *j, int ile, int16_t *result) {
  /* czyta liczbe z bufora
   jesli odczytal to zwraca 1 oraz wynik przez wskaznik result
   jesli blad to zwraca 0
  */

  int16_t wart, w;
  char c;
  wart = 0;
  switch (c = bufor[*j]) {
  case 39: /* apostrof */
    if ((++(*j)) == ile)
      return 0;
    *result = bufor[(*j)++];
    return 1;
  case '$':
    if ((++(*j)) == ile || hextab[bufor[*j]] == 0x7f)
      return 0;
    while ((*j) < ile && (w = hextab[bufor[*j]]) != 0x7f) {
      (*j)++;
      wart = (wart << 4) + w;
    }
    break;
  case ':': /* binary */
    if ((++(*j)) == ile || hextab[bufor[*j]] > 1)
      return 0;
    while ((*j) < ile && (w = hextab[bufor[*j]]) <= 1) {
      (*j)++;
      wart = (wart << 1) | w;
    }
    break;

  case '%': /*macro parametr*/
    if (!inside_macro)
      return 0; /* to nie jest macro*/
    (*j)++;
    if (!paramnr(j, ile, &w))
      return 0;
    wart = w + first_par;
    if (wart >= psp) {
      if (phase == 2)
        error("Bad parameter");
      return 0;
    }
    if (!is_param[wart])
      return 0;
    *result = param[wart];
    return 1;

  default: /*liczba dziesietna*/
    if (hextab[bufor[*j]] >= 10)
      return 0;
    while ((c = bufor[*j] - '0') >= 0 && c < 10 && (*j) < ile) {
      (*j)++;
      wart = wart * 10 + c;
    }
  }
  *result = wart;
  return 1;
}

void calc(int *np, int *op) {
  int16_t wart, wart2;
  switch (os[--(*op)]) { /* level 1 LSB & MSB */
  case '<':
    ns[(*np) - 1] = ns[(*np) - 1] & 0xff;
    break;
  case '>':
    ns[(*np) - 1] = (uint16_t)ns[(*np) - 1] >> 8;
    break;

  /* level 2 logical operators */
  case ':':
    wart = ns[--(*np)];
    wart2 = ns[--(*np)];
    ns[(*np)++] = wart || wart2; /* OR*/
    break;
  case '@':
    wart = ns[--(*np)];
    wart2 = ns[--(*np)];
    ns[(*np)++] = wart && wart2; /*AND*/
    break;
  case '~':
    ns[(*np) - 1] = !ns[(*np) - 1];
    break; /*NOT*/

  /* level eqution operators */
  case '=':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] == ns[(*np)]);
    break;
  case 'r':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] != ns[(*np)]);
    break;
    /*
    pobranie ze stosu w odwrotnej kolejnosci dlatego zamiast np. < jest >
    */
  case 'm':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] < ns[(*np)]);
    break;
  case 'w':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] > ns[(*np)]);
    break;
  case '{':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] <= ns[(*np)]);
    break;
  case '}':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] >= ns[(*np)]);
    break;

  /*  bit operators */
  case '&':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] & ns[(*np)]);
    break; /* AND*/
  case '|':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] | ns[(*np)]);
    break; /*  OR*/
  case '^':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] ^ ns[(*np)]);
    break; /* EOR*/

  /* addition & substraction */
  case '+':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] + ns[(*np)]);
    break;
  case '-':
    wart = ns[--(*np)];
    ns[(*np) - 1] = ns[(*np) - 1] - wart;
    break;

  /* multiplication & division */
  case '*':
    --(*np);
    ns[(*np) - 1] = (ns[(*np) - 1] * ns[(*np)]);
    break;
  case '/':
    wart = ns[--(*np)];
    if (wart == 0) {
      error("Divide by zero!");
      wart = 1;
    }
    ns[(*np) - 1] = ns[(*np) - 1] / wart;
    break;

  /* negacja */
  case '!':
    ns[(*np) - 1] = 0 - ns[(*np) - 1];
    break;
  }
}

int evaluate(int pocz, int ile, int16_t *wynik) {
  /* jezeli mozna obliczyc wartosc wyrazenia to zwraca 1,i wartosc
     poprzez wskaznik wynik
     w przeciwnym wypadku zwraca 0, nie zmiania danej wskazywanej przez
     wynik*/

  int16_t i, last, n, lev, firstchar;
  char op, c, errlab;
  i = pocz;
  osp = nsp = ns[0] = last = hsp = errlab = 0;
  while (i < ile) {
    if (bufor[i] == ' ') {
      i++;
      continue;
    }
    if (!fclnum[bufor[i]]) {
      switch (op = getoperator(&i)) {
      case 0:
        if (thesame(&bufor[i], ".def", 4)) /* operator .def*/
        {
          i += 4;
          while (bufor[i] == ' ' && i < ile)
            i++;
          firstchar = i;
          while (labtab[bufor[i]] && i < ile)
            i++;
          if (i == firstchar)
            return 0; /*zla skladnia*/
          ns[nsp++] = is_label(&bufor[firstchar], i - firstchar, &firstchar, 0, 0, 0);
          hist[hsp++] = 'n';
          break;
        }
        if (thesame(&bufor[i], ".ref", 4)) /* operator .def*/
        {
          i += 4;
          while (bufor[i] == ' ' && i < ile)
            i++;
          firstchar = i;
          while (labtab[bufor[i]] && i < ile)
            i++;
          if (i == firstchar)
            return 0; /*zla skladnia*/
          ns[nsp++] = ref_label(&bufor[firstchar], i - firstchar);
          hist[hsp++] = 'n';
          break;
        }
        return 0; /* zly operator */

      case '[':
        os[osp++] = hist[hsp++] = '[';
        while (bufor[i] == '[') {
          i++;
          os[osp++] = '[';
          hist[hsp++] = '[';
        }
        last = 0;
        break;

      case ']':
        while (os[--osp] != '[') {
          osp++;
          calc(&nsp, &osp);
        }
        last = level(os[--osp]);
        osp++;
        break;

      case '*':
        if (i == pocz + 1 || ((c = hist[hsp - 1]) != 'n' && c != ']')) {
          ns[nsp++] = adres;
          hist[hsp++] = 'n';
          break;
        }

      case '+':
        if (i == pocz + 1 || ((c = hist[hsp - 1]) != 'n' && c != ']'))
          break;
      case '-':
        if (i == pocz + 1 || ((c = hist[hsp - 1]) != 'n' && c != ']'))
          op = '!';

      default:
        if ((lev = level(op)) <= last && !unarny(op) && osp > 0 && os[osp - 1] != '[')
          calc(&nsp, &osp);
        if (lev < last && !unarny(op)) {
          while (osp > 0 && os[osp - 1] != '[' && level(os[osp]) >= lev) {
            calc(&nsp, &osp);
          }
        }
        os[osp++] = hist[hsp++] = op;
        last = level(op);
      }
    } else {
      if (fcltab[c = bufor[i]]) {
        errlab |= (!findlabel(&i, ile, &n)); /*1 gdy nie mozna wyliczyc*/
      } else if (!getnumber(&i, ile, &n))
        return 0; /* nie mozna wyliczyc*/
      ns[nsp++] = n;
      hist[hsp++] = 'n'; /* number */
    }
  }
  while (osp > 0 && nsp > 0) {
    if (os[osp - 1] == '[')
      return 0; /*nie pozamykane nawiasy*/
    calc(&nsp, &osp);
  }
  if (nsp != 1)
    return 0; /* nie mozna wyliczyc za duzo liczb na stosie*/

  *wynik = ns[0];
  if (errlab) {
    if (phase == 2)
      error("Not defined label");
    return 0;
  }
  return 1;
}

// end include argvalue.c

int adrmode(char first, char last)
/* zwraca numer rozpoznanego trybu adresowania
 */
{
  switch (bufor[first]) {
  case 'a':
  case 'A':
    if (last - first == 1)
      return 2;
    if ((last - first == 2) && bufor[last] == ' ')
      return 2;
    break;
  case '#':
    return 3;
  case '(':
    if (thesame(&bufor[last - 3], "),y", 3))
      return 11; /* (aa),y  */
    if (thesame(&bufor[last - 3], ",x)", 3))
      return 10; /* (aa,x)  */
    if (bufor[last - 1] == ')')
      return 12; /* (aaaa)  */
  }
  if (thesame(&bufor[last - 2], ",x", 2))
    return 6; /*  aaaa,x */
  if (thesame(&bufor[last - 2], ",y", 2))
    return 8; /*  aaaa,y */
  return 4;   /* aaaa */
}

char arglen(char first, char last)
/* zwraca dlugosc (1 albo 2 bajty) argumentu
   jezeli nie mozna wyznaczyc dlugosci to zwraca 2
*/
{
  int16_t argval;

  switch (bufor[first]) {
  case '#':
  case '(':
    return 1;
  case 'a':
  case 'A':
    if ((last - first) == 1)
      return 0;
    // dodalej spr. ';' - poprawka bledu z lsr a i komentarzem
    if (((last - first) == 2) && (bufor[last] == ' ') || bufor[last] == ';')
      return 0;
  }
  if (evaluate(first, last, &argval)) /* jezeli mozna wyznaczyc wartosc */
    if (!(argval & 0xff00))
      return 1; /* strona zarowa */

  return 2;
}
char mnemnr(char first) {
  char m;
  m = mnem_hash_tab[hash_mnem((uint8_t *)&bufor[first])];
  if (thesame((char *)&bufor[first], (char *)&mnemoniki[m].mnem, 3))
    return m;
  return -1;
}

char instrlen(char first)
/* zwraca dlugosc instrukcji lub zero jesli nie mozna jej jednoznacznie
   okreslic
*/
{
  char m;
  m = mnem_hash_tab[hash_mnem((uint8_t *)&bufor[first])];
  if (thesame((char *)&bufor[first], (char *)&mnemoniki[m].mnem, 3))
    return mnemoniki[m].mlen;
  return -1;
}

int cnt_byte(void)
/*  zwraca dlugosc danych wprowadzanych za pomoca dyrektyw
 .byte .sbyte .cbyte
*/
{
  char c;
  int16_t ile, wart;

  ile = 0;
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  if (c == '+') {
    if (bufor[++col] == '$')
      while (hextab[bufor[++col]] != 0x7f) {
      }
    else
      while (hextab[bufor[col]] < 10)
        col++;
    if (bufor[col++] != ',')
      return 0; /*zly format wyrazenia*/
  }
  while ((c = bufor[col]) != crt) {
    if (c == crt || c == ';' || c == ',')
      return 0; /* zly format wyrazenia */
    switch (bufor[col]) {
    case 39: /* apostrof */
      col++;
      if (bufor[col] != crt) {
        ile++;
        col++;
        break;
      }
      return 0; /* zly format */
    case '"':
      while ((c = bufor[++col]) != '"' && c != crt)
        ile++;
      col++;
      break;
    default:
      if (c == '%' && bufor[col + 1] == '$' && inside_macro) { /*text parameter*/
        col += 2;
        if (paramnr(&col, BUFLEN, &wart) && wart + first_par < psp) {
          if (is_param[first_par + wart] == 2)
            ile += param[first_par + wart];
          else
            ile += param[first_par];
        }
      } else { /* expression*/
        while ((c = bufor[++col]) != crt && c != ';' && c != ',')
          ;
        ile++;
      }
    }
    if (bufor[col] != ',')
      return ile;
    col++;
  }
  return ile;
}

int cnt_word(void)
/* zwraca dlugosc danych wprowadzonych za pomoca
   dyrektyw .word i .dbyte
*/
{
  char c;
  int ile;

  ile = 0;
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  if (c == crt || c == ',' || c == ';')
    return 0; /*zly format wyrazenia */
  while ((c = bufor[col]) != crt && c != ';') {
    while ((c = bufor[++col]) != crt && c != ';' && c != ',')
      ;
    ile += 2;
  }
  if (bufor[col] != ',')
    return ile;
  col++;
  return ile;
}

void print_line(int mhead) {
  int nest;

  if (!mlistflg && !char_out && inside_macro && !mhead)
    return;
  if (char_out && !licz_list) {
    char_out = 0;
    fprintf(flist, "\n");
  }
  nest = asem_col - char_out;
  while (nest--)
    fprintf(flist, " ");
  if (numflg)
    fprintf(flist, "%d ", numcnt);
  if (inside_macro) {
    nest = macro_nest;
    while (nest--)
      fprintf(flist, " ");
    fprintf(flist, "M  ");
  }
  fprintf(flist, "%s", bufor);
  /* w prn_lin znacznik zeby nie powtarzac*/
  prn_lin = 0;
  char_out = 0;
  prn_ln_cnt++;
  if (prn_ln_cnt == set_dat[4]) {
    fprintf(flist, "\vPage %d   %s\n", ++page_cnt, title_dat);
    prn_ln_cnt = 0;
  }
}

void listcode(unsigned char c) {
  if (listflg && (clistflg || asmflg)) {
    if (char_out && !licz_list) {
      fprintf(flist, "\n");
      char_out = 0;
    }
    if (licz_list == 4) {
      print_line(0);
      licz_list = 0x08;
    }
    if (!(licz_list & 0x03)) {
      fprintf(flist, "%x: ", adres);
      char_out += 6;
    }
    fprintf(flist, "%02x ", c);
    licz_list++;
    if (!(licz_list & 0x03) && licz_list != 4) {
      fprintf(flist, "\n");
      char_out = 0;
    } else
      char_out += 3;
  }
}

int output;
/* 0 none
   1 destination file binary
   2 destination file hex
*/
void put_out(unsigned char c) {
  static char licz;

  switch (output) {
  case 1:
    fputc(c, dest);
    break;
  case 2:
    fprintf(dest, "%02X ", c);
    licz++;
    if ((licz & 0x0f) == 0x00)
      fprintf(dest, "\r");
  }
}

static struct {
  unsigned int first, last;
  char (*bytes)[];
} buf_out;

void clr_buf_out(void) {
  uint16_t i;
  if (bptr) { /* bptr=0 gdy nie wyslano jescze zadnego bajtu */
    if (adresbytes) {
      put_out(buf_out.first & 0xff);
      put_out(buf_out.first >> 8);
      put_out(buf_out.last & 0xff);
      put_out(buf_out.last >> 8);
    }
    for (i = 0; i < bptr; i++)
      put_out((*buf_out.bytes)[i]);
    buf_out.first = adres;
    bptr = 0;
  }
}

void putbyte(unsigned char c) {
  uint16_t i;
  if (!objflg) {
    adres++;
    return;
  }
  if (reptflg) {
    if (rept_len < max_rept_block)
      (*rept_buf)[rept_len] = c;
    rept_len++;
    return;
  }

  if (!bptr) /*pierwsze wywolanie*/
  {
    if (adresbytes) {
      put_out(0xff);
      put_out(0xff);
    }
    buf_out.first = adres;
  } else if (bptr == buf_out_len || buf_out.last != adres - 1) {
    if (adresbytes) {
      put_out(buf_out.first & 0xff);
      put_out(buf_out.first >> 8);
      put_out(buf_out.last & 0xff);
      put_out(buf_out.last >> 8);
    }
    for (i = 0; i < bptr; i++)
      put_out((*buf_out.bytes)[i]);
    buf_out.first = adres;
    bptr = 0;
  }
  buf_out.last = adres;
  (*buf_out.bytes)[bptr++] = c;
  listcode(c);
  adres++;
}

void putinstr(char m) {
  char c, mode, firstchar, lastchar, *mod;
  int16_t argument;

  if ((c = mnemoniki[m].modes[0]) != (char)0xff) {
    putbyte(c);
    return;
  } /* bez argumentowy */

  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  if (c == crt || c == ';') {
    error("bad operand");
  } else {
    firstchar = col;
    while (((c = bufor[col]) != ' ' || bufor[col + 1] != ' ') && c != tab && c != crt && c != ';')
      col++;
    if (bufor[col - 1] == ' ')
      col--;
    if ((c = mnemoniki[m].modes[1]) != (char)0xff) /*skok wzgledny*/
    {
      if (!evaluate(firstchar, col, &argument))
        error("can't evaluate expession");
      argument -= adres + 2;
      putbyte(c);
      putbyte(argument & 0xff);
      if (argument > 127 || argument < -128)
        error("branch range");
    } else { /* pozostale instrukcje rozpoznanie trybu adresowania */
      mode = adrmode(firstchar, col);
      mod = (char *)mnemoniki[m].modes;
      switch (mode) {
      case 2: /* akumulatora */
        if ((c = mod[2]) == (char)0xff)
          error("bad addressing mode");
        else {
          putbyte(c);
          break;
        }
      case 3: /* hash */
        if ((c = mod[3]) == (char)0xff) {
          error("bad addressing mode");
        } else
          putbyte(c);
        {
          if (evaluate(firstchar + 1, col, &argument)) {
            if (argument < 0x100 && argument > -0x80) {
              putbyte(argument & 0xff);
            } else {
              putbyte(0);
              error("operand is not one byte size");
            }
          } else {
            putbyte(0);
            error("can't evaluate expression");
          }
        }
        break;
      case 4:
      case 6:
      case 8:
        lastchar = (mode == 4) ? col : col - 2;
        if (evaluate(firstchar, lastchar, &argument)) {
          if (!(argument & 0xff00)) {
            if ((c = mod[mode + 1]) != (char)0xff) {
              putbyte(c);
              putbyte(argument & 0xff);
              break;
            }
          }
          if ((c = mod[mode]) != (char)0xff) {
            putbyte(c);
            putbyte(argument & 0xff);
            putbyte(argument >> 8);
          } else {
            error(" bad addressing mode");
          }
        } else {
          putbyte(mod[mode]);
          putbyte(0);
          putbyte(0);
          error(" can't evaluate expression");
        }
        break;
      case 10:
      case 11:
        if ((c = mod[mode]) != (char)0xff) {
          if (evaluate(firstchar + 1, col - 3, &argument)) {
            if (!(argument & 0xff00)) {
              putbyte(c);
              putbyte(argument & 0xff);
            } else {
              putbyte(c);
              putbyte(0);
              error("operand is not zero page address");
            }
          } else {
            putbyte(c);
            putbyte(0);
            error("can't evaluate expression");
          }
        } else {
          error("bad addressing mode");
        }
        break;
      case 12:
        if ((c = mod[mode]) != (char)0xff) {
          if (evaluate(firstchar + 1, col - 1, &argument)) {
            putbyte(c);
            putbyte(argument & 0xff);
            putbyte(argument >> 8);
          } else {
            putbyte(c);
            putbyte(0);
            putbyte(0);
            error("can't evaluate expression");
          }
        } else {
          error("bad addressing mode");
        }
      } /*switch*/
    }   /* if */
  }     /* if */
}

void local(void) {
  /* dyrektywa local
   */
  localcnt++;
  if (!localcnt && phase == 2)
    error("Too many local fields");
  /* max 256 local fields (in XL/XE Mac\65 ware 62 local fields)
   */
}

void optexec(void) {
  /* analiza argumentu dyrektywy .opt
   */
  char c;
  int licz, set;

  licz = 0;
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  while ((c = bufor[col]) != crt) {
    set = 1;
    if (thesame(&bufor[col], "no", 2)) {
      col += 2;
      set = 0;
      while ((c = bufor[col]) == ' ' || c == tab)
        col++;
      if (c == crt)
        break; /* bad opt format*/
    }
    do {
      if (thesame(&bufor[col], "obj", 3)) {
        objflg = set;
        col += 3;
        licz++;
        break;
      }

      if (thesame(&bufor[col], "list", 4)) {
        listflg = set & lstf;
        col += 4;
        licz++;
        break;
      }

      if (thesame(&bufor[col], "err", 3)) {
        errflg = set;
        col += 3;
        licz++;
        break;
      }

      if (thesame(&bufor[col], "eject", 5)) {
        ejectflg = set;
        col += 5;
        licz++;
        break;
      }

      if (thesame(&bufor[col], "mlist", 5)) {
        mlistflg = set;
        col += 5;
        licz++;
        break;
      }

      if (thesame(&bufor[col], "clist", 5)) {
        clistflg = set;
        col += 5;
        licz++;
        break;
      }

      if (thesame(&bufor[col], "num", 3)) {
        numflg = set;
        col += 3;
        licz++;
        break;
      }

      if (thesame(&bufor[col], "xref", 4)) {
        xrefflg = set;
        col += 4;
        licz++;
        break;
      }

      licz = 0; /* blad */
    } while (0);
    if (bufor[col++] != ',' && licz > 0)
      return; /*O.K.*/
    if (!licz)
      break;
  }
  error("Bad syntax in .opt");
}

void put_byte(int type)
/* wyprowadza dane wprowadzone za pomoca dyrektyw
 .byte,sbyte,cbyte
 type odpowiednio rowne 0,1,2.
*/
{
  char c, firstchar;
  int16_t add, wart, cnt, par;
  char *text;

  add = 0;
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  if (c == '+') {
    col++;
    if (!getnumber(&col, BUFLEN, &add) || bufor[col++] != ',') {
      error("bad offset in .byte");
      return;
    }
  }
  while ((c = bufor[col]) != crt) {
    switch (c) {
    case 39: /* apostrof */
      if ((c = bufor[++col]) != crt) {
        if (type == 1)
          c = asctoscr(c); /*.sbyte*/
        putbyte(add + c & UINT8_C(0xff));
      } else
        error("endline after ' ");
      c++;
      break;

    case '"':
      while ((c = bufor[++col]) != '"' && c != crt) {
        if (type == 1)
          c = asctoscr(c); /* .sbyte */
        if (type == 2)
          if (bufor[col + 1] == '"')
            c ^= 0x80; /*.cbyte*/
        putbyte(add + c & 0xff);
      }
      if (c == crt)
        error("brak cudzyslowu zamykajacego");
      else
        col++;
      break;

    default:
      if (c == '%' && bufor[col + 1] == '$' && inside_macro) { /*text parameter*/
        col += 2;
        if (!paramnr(&col, BUFLEN, &wart))
          error("Bad text macro parameter");
        else {
          cnt = param[par = first_par + wart];
          if (par >= psp)
            error("Bad macro parameter");
          else {
            text = text_param[par];
            if (is_param[par] != 2) {
              text = text_param[first_par];
              cnt = param[first_par];
            }
            while (cnt--) {
              c = *text;
              if (type == 1)
                c = asctoscr(c); /* .sbyte */
              if (type == 2)
                if (bufor[col + 1] == '"')
                  c ^= 0x80;
              /*.cbyte*/
              putbyte(c);
              text++;
            }
          }
        }
      } else { /* wyrazenie*/
        firstchar = col;
        while ((c = bufor[++col]) != crt && c != ';' && (c != ' ' || bufor[col + 1] != ' ') && c != tab && c != ',')
          ;
        evaluate(firstchar, col, &wart);
        if (wart < -128 || wart > 255)
          error("should be one byte size");
        else {
          if (type == 1)
            wart = (int)asctoscr(wart & 0xff); /*.sbyte*/
          putbyte(wart + add & 0xff);
        }
      }
    } /*switch*/
    if (bufor[col++] != ',')
      return;
  } /*while*/
}

void put_word(int dbyteflg)
/*
wyprowadza dane wprowadzone za pomoca dyrektyw
.word i .dbyte
dbyteflg rowny 1 gdy uzyto dyrektywy dbyte
dbyteflg rowny 0 gdy uzyto dyrektywy word
*/
{
  char c;
  int16_t wart, firstchar;

  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  while ((c = bufor[col]) != crt) {
    firstchar = col;
    while ((c = bufor[++col]) != crt && c != ';' && (c != ' ' || bufor[col + 1] != ' ') && c != tab && c != ',')
      ;
    evaluate(firstchar, col, &wart);
    if (!dbyteflg) { /*.word*/
      putbyte(wart & 0xff);
      putbyte(wart >> 8);
    } else { /*.dbyte*/
      putbyte(wart >> 8);
      putbyte(wart & 0xff);
    }
    if (bufor[col++] != ',')
      return;
  }
}

void read_float(int pocz, int ile, char *wynik)
/* Czyta z bufora liczbe zmiennoprzecinkowa i przeksztalca do
formatu Atari XL/XE
*/
{
  int i;
  char c, sgn, exp, fch, fi, nib, wart;
  for (i = 0; i < 6; i++)
    wynik[i] = 0;
  i = pocz;
  sgn = 0x40;
  if (bufor[i] == '-') {
    i++;
    sgn = 0xc0;
  } else if (bufor[i] == '+')
    i++;
  while (bufor[i] == '0')
    i++;
  fch = bufor[i];
  fi = 1;
  nib = 4;
  exp = -2;
  while ((c = bufor[i]) >= '0' && c <= '9' && i < ile) {
    i++;
    if (fi < 6) {
      wynik[fi] |= (c & 0x0f) << nib;
      if (!nib)
        fi++;
      nib = 4 - nib;
    }
    exp++;
  }

  if (c == '.') {
    i++;
    while (fch == '.' && bufor[i] == '0' && i < ile) {
      i++;
      exp--;
    }
    while ((c = bufor[i]) >= '0' && c <= '9' && i < ile) {
      i++;
      if (fi < 6) {
        wynik[fi] |= (c & 0x0f) << nib;
        if (!nib)
          fi++;
        nib = 4 - nib;
      }
    }
  }
  if (c == 'e' || c == 'E') {
    fch = 1;
    wart = 0;
    switch (bufor[++i]) {
    case '-':
      fch = -1;
    case '+':
      i++;
    }
    while ((c = bufor[i]) >= '0' && c <= '9') {
      i++;
      wart = wart * 10 + (c & 0x0f);
    }
    exp += wart * fch;
  }
  if (exp & 0x01) {
    for (c = 5; c > 1; c--)
      wynik[c] = (wynik[c] >> 4) | (wynik[c - 1] << 4);
    wynik[1] >>= 4;
    exp++;
  }
  wynik[0] = sgn + (exp / 2);
  wart = 0;
  for (c = 1; c < 6; c++)
    wart |= wynik[c];
  if (!wart)
    wynik[0] = 0;
  if (i == pocz || (i == pocz + 1 && bufor[pocz] == '.')) {
    error("Bad .float syntax");
    for (c = 0; c < 6; c++)
      wynik[c] = 0;
  }
}

void put_float(void)
/*
wyprowadza dane wprowadzone za pomoca dyrektywy .float
*/
{
  char c;
  char fltbuf[6];
  int firstchar;

  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  while ((c = bufor[col]) != crt) {
    firstchar = col;
    while ((c = bufor[++col]) != crt && c != ';' && (c != ' ' || bufor[col + 1] != ' ') && c != tab && c != ',')
      ;
    read_float(firstchar, col, fltbuf);
    for (c = 0; c < 6; c++)
      putbyte(fltbuf[c]);
    if (bufor[col++] != ',')
      return;
  }
}

int getlabel(void)
/* Pobiera etykiete i probuje dolaczyc do listy etykiet
   wykorzystywana w pierwszym przebiegu
   zwraca 1 gdy bylo podstawienie wartosci przez '='
   w przeciwnym wypadku 0
*/
{
  char c, lastchar, firstchar;
  int16_t labval;
  int tmp;
  if (!fcltab[bufor[col]])
    return 0;

  while (labtab[bufor[col]])
    col++;
  lastchar = col;
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  if (c != '=' && (c != '.' || bufor[col + 1] != '=')) { /* Podstawienie adresu na etykiete */
    if (asmflg)                                          /* spr. znacznika semblacji warunkowej */
      (void)is_label((char *)bufor, lastchar, &labval, adres, 1, inside_macro);
    /*                                          1-definicja,
           w macrorozkazie wszystkie etykiety sa tymczasowe.
    */
    return 0;
  } else {
    if (tmp = (c == '.'))
      col++; /* tmp =1 gdy .= else tmp =0 */
    col++;
    while ((c = bufor[col]) == ' ' || c == tab)
      col++;
    firstchar = col;
    while (((c = bufor[col]) != ' ' || bufor[col + 1] != ' ') && c != tab && c != crt && c != ';')
      col++;
    /* Podstawienie wartosci na etykiete */
    if (asmflg) /* spr. znacznika asemblacji warunkowej */
    {
      if (evaluate(firstchar, col, &labval))
        /*jezeli evaluate=1 to definicja etykiety 1*/
        (void)is_label((char *)bufor, lastchar, &labval, labval, 1, tmp);
      else
        /*jezeli evaluate=0 to podstawienie wartosci nieokreslonej 2 */
        (void)is_label((char *)bufor, lastchar, &labval, 0, 2, tmp);
    }
  }
  return 1;
}

int checklabel(void)
/* Pobiera etykiete i
    -sprawdza czy nie ma bladu fazy
    -sprawdza czy etykieta nie byla juz wczesniej zdefiniowana
   wykorzystywana w drugim przebiegu
   zwraca 1 gdy bylo podstawienie wartosci przez '='
   w przeciwnym wypadku 0
*/
{
  char c, lastchar, firstchar;
  int16_t labval, lbv;
  int tmp;
  if (!fcltab[bufor[col]])
    return 0;

  while (labtab[bufor[col]])
    col++;
  lastchar = col;
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  if (c != '=' && (c != '.' || bufor[col + 1] != '=')) { /* spr i ew. dodaje etykiete */
    if (asmflg)                                          /* spr znacznika asemblacji warunkowej */
    {
      if (is_label((char *)bufor, lastchar, &lbv, adres, 1, inside_macro) &&
          lbv != adres) /*gdy macrorozkaz to wszystkie sa localne*/
        error("Phase error");
      /*wartosc etykiety jest rozna od wartosci w poprzednim
        przebiegu
      */
    }
    return 0;
  } else {
    if (tmp = (c == '.'))
      col++; /* tmp =1 gdy .= else tmp =0 */
    col++;
    while ((c = bufor[col]) == ' ' || c == tab)
      col++;
    firstchar = col;
    while (((c = bufor[col]) != ' ' || bufor[col + 1] != ' ') && c != tab && c != crt && c != ';')
      col++;
    if (asmflg) {
      if (evaluate(firstchar, col, &labval)) {
        if (is_label((char *)bufor, lastchar, &lbv, labval, 1, tmp) && lbv != labval && !tmp)
          error("Phase error");
      } else
        (void)is_label((char *)bufor, lastchar, &lbv, labval, 2, tmp);
      /* 2 - podstawienie wartosci nieokreslonej */
    }
  }
  return 1;
}

void getinstruction(void) {
  char c, i, ile;
  unsigned char len;
  struct macro *mptr;

  if ((c = bufor[col]) == crt || c == ';')
    return;
  i = col;
  while ((c = bufor[col]) != ' ' && c != tab && c != crt)
    col++;
  if ((ile = col - i) == 3 && (len = instrlen(i)) != 0xff)
    switch (len) {
    case 0: /* dlugosc nie znana*/
    {
      while ((c = bufor[col]) == ' ' || c == tab)
        col++;
      if (c == crt) {
        error("no operand");
        return;
      }
      i = col;
      while (((c = bufor[col]) != ' ' || bufor[col + 1] != ' ') && c != ',' && c != tab && c != crt &&
             c != ';' ////dodalej spr. ';' - poprawka bledu z lsr a i komentarzem
      )
        col++;
      len = arglen(i, col) + 1;
    }
    default:
      adres += len;
    }
  else /* maybe a macro? */
  {
    if ((mptr = is_macro(&bufor[i], ile)) != NULL)
      macro_analizing(mptr);
    /* else error("Undefined macro") bylby w drugiej fazie */;
  }
}

void asm_if(void)
/* dyrektywa .if */
{
  char c;
  int16_t value;
  int firstchar;

  if (isp >= 14) {
    error(".if/.endif stack full");
    return;
  }
  if (!asmflg) {
    i_stack[++isp] = 0; /* 0 .if */
    asm_stack[isp] = 0;
    return;
  }
  /* asmflg=1 */
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  firstchar = col;
  while (((c = bufor[col]) != ' ' || bufor[col + 1] != ' ') && c != tab && c != crt && c != ';')
    col++;

  if (evaluate(firstchar, col, &value)) {
    asmflg = value;
    i_stack[++isp] = 0; /* 0 .if */
    asm_stack[isp] = asmflg;
  } else {
    asmflg = 0;
    i_stack[++isp] = 0; /* 0 .if */
    asm_stack[isp] = asmflg;
    error("Bad .if syntax");
  }
}

void asm_endif(void)
/* dyrektywa .endif */
{
  if (!isp) {
    error(".if/.endif stack empty");
    return;
  }
  asmflg = asm_stack[--isp];
}

void asm_else(void)
/* dyrektywa else */
{
  if (!isp) {
    error(" .if/.endif stack empty");
    return;
  }
  if (i_stack[isp]) /* 1 .else */
  {
    if (asm_stack[isp - 1])
      error("Nested .else");
    return; /* nie ma bledu gdy poprzednio asmflg=0 */
  }
  i_stack[isp] = 1; /* 1 .else */
  asmflg = (!asm_stack[isp]) & asm_stack[isp - 1];
  /* 1 gdy po if bylo zero i poprzednio nie bylo zero */
  asm_stack[isp] = asmflg;
}

void include(int what) {
  /* 0 include,  1 incbin phase 1 ,2  incbin phase 2 */
  FILE *plik;
  char r[] = "r", rb[] = "rb";
  char c, *type, *pth;
  char firstchar;
  unsigned long flen;
  if (!what)
    type = &r[0];
  else
    type = &rb[0];
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  if (c == '#') {
    col++;
    firstchar = col;
    while (((c = bufor[col]) != ' ' || bufor[col + 1] != ' ') && c != ';' && c != crt && c != tab)
      col++;
    c = bufor[col];
    char *fname = (char *)alloca(col + 1 - firstchar);
    strncpy(fname, &bufor[firstchar], col - firstchar);
    fname[col - firstchar] = 0;
    strup(fname);
    if ((plik = fopen(fname, type)) == NULL) {
      /*error*/
      if (phase == 2)
        error("can't open file");
      bufor[col] = c;
      return;
    }
    switch (what) {
    case 0: /*include*/
      if (fsp == maxfnr) {
        error("include stack full");
        fclose(plik);
      } else {
        pth = (char *)m_alloc(strlen(sciezka) + 1);
        strcpy(pth, sciezka);
        filestack[fsp++] = source;
        fpthstack[fsp - 1] = filepath;
        linestack[fsp - 1] = line_inc;
        line_inc = 0;
        filepath = pth;
        source = plik;
      }
      break;
    default: /*incbin*/
      flen = myfilelength(plik);
      if (phase == 2)
        while (flen--)
          putbyte(fgetc(plik) & 0xff);
      else
        adres += flen;
      fclose(plik);
    }
  } else
    error("Bad syntax");
}

void rept(int ph) {
  char c;
  col += 4;
  if (reptflg) {
    if (ph)
      error("Nested rept");
    return;
  }
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  if (!getnumber(&col, BUFLEN, (int16_t *)&cnt_rept)) {
    if (ph)
      error("bad syntax");
    return;
  }
  reptflg = 1;
  if (!ph)
    beg_rept = adres;
  else {
    rept_buf = (char(*)[])m_alloc(max_rept_block);
    rept_len = 0;
  }
}

void endr(int ph) {
  int16_t i;
  col += 4;
  if (!reptflg) {
    if (ph)
      error("Unexpected .endr");
    return;
  }
  reptflg = 0;
  if (!ph)
    adres += adres - beg_rept <= max_rept_block ? (cnt_rept - 1) * (adres - beg_rept) : max_rept_block;
  else {
    if (rept_len > max_rept_block) {
      error("rept block longer than $2000");
      rept_len = max_rept_block;
    }
    while (cnt_rept-- > 0) {
      i = 0;
      while (i < rept_len)
        putbyte((*rept_buf)[i++]);
    }
  }
}

void err_tit_pag(char what)
/* dyrektywy: .error - 0,.title - 1,page - 2 */
{
  char c, *text;

  while ((c = bufor[col++]) == ' ' || c == tab)
    ;
  if (c == '"') {
    text = &bufor[col];
    while ((c = bufor[col]) != '"' && c != crt)
      col++;
    if (c != crt) {
      bufor[col] = 0;
      switch (what) {
      case 0:
        error(text);
        break;
      case 1:
        memmove(title_dat, text, BUFLEN);
        break;
      case 2:
        memmove(page_dat, text, BUFLEN);
        break;
      }
      bufor[col] = c;
      return;
    }
  }
  switch (what) {
  case 0:
    error("Bad syntax in error");
    break;
  case 1:
    error("Bad syntax in .title");
    break;
  case 2:
    error("Bad syntax in .page");
  }
}

void page(void) {
  /* dyrektywa .page - wysow nowej strony dokonczyc!!!*/
  err_tit_pag(2); /*page*/
  if (listflg)
    fprintf(flist, "\vPage %d   %s\r\n%s\r\n", ++page_cnt, title_dat, page_dat);
  prn_ln_cnt = 0;
}

unsigned char get_byte_number(void) {
  /*czyta z bufora liczbe dziesietna*/
  unsigned char wart;
  char c;
  wart = 0;
  while ((c = bufor[col]) >= '0' && c <= '9') {
    wart = wart * 10 + (c & 0x0f);
    col++;
  }
  return wart;
}

void set(void) {
  /* dyrektywa .set */
  char c;
  unsigned char nr, wart;

  while ((c = bufor[col++]) == ' ' || c == tab)
    ;
  if (c >= '0' && c <= '4' && bufor[col] == ',') {
    nr = c & 0x0f;
    col++;
    if ((c = bufor[col]) >= '0' && c <= '9') {
      wart = get_byte_number();
      if (bufor[col] != ',') {
        set_dat[nr] = wart;
        return;
      }
    }
  }
  error("Bad syntax in .set");
}

void tab_(void) {
  /* dyrektywa .tab */
  char c, nr;
  unsigned char tab_dt[3];
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  for (nr = 0; nr < 3; nr++) {
    if ((c = bufor[col]) < '0' && c > '9')
      break;
    tab_dt[nr] = get_byte_number();
    if (bufor[col++] != ',')
      break;
  }
  if (nr == 2) {
    for (nr = 0; nr < 3; nr++)
      tab_dat[nr] = tab_dt[nr];
    return;
  }
  error("Bad syntax in .tab");
}

int readln(void) {
  char c;
  int i;
  i = 0;
  prn_lin = 1;
  if (fgets(line_bufor, BUFLEN, source) == NULL) {
    line_bufor[0] = crt;
    return 0;
  };
  while ((c = line_bufor[i]) != crt && c != lnf && c && i < BUFLEN - 2)
    i++;
  line_bufor[i] = crt;
  line_bufor[i + 1] = lnf;
  line_bufor[i + 2] = 0;
  line++;
  line_inc++;
  return i + 2;
}

//#include "macros.c"

int read_param(void) {
  char c, firstchar;
  int16_t wart;
  int licz;
  char *text;

  licz = 0;
  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  while ((c = bufor[col]) != crt) {
    switch (c) {
    case '"':
      firstchar = ++col;
      while ((c = bufor[col]) != '"' && c != crt)
        col++;
      if (c == crt)
        if (phase == 2)
          error("Bad macro text parameter");
      if (psp == max_psp) {
        if (phase == 2)
          error("Macro parameter stack full");
        return licz;
      }
      param[psp] = (wart = col - firstchar);
      text = (char *)m_alloc(wart);
      memmove(text, &bufor[firstchar], wart);
      text_param[psp] = text;
      is_param[psp++] = 2; /*textowy*/
      break;

    default:
      firstchar = col;
      while ((c = bufor[++col]) != crt && c != ';' && (c != ' ' || bufor[col + 1] != ' ') && c != tab && c != ',')
        ;
      if (psp == max_psp) {
        if (phase == 2)
          error("Macro parameter stack full");
        return licz;
      }
      if (evaluate(firstchar, col, &wart)) {
        param[psp] = wart;
        is_param[psp++] = 1; /*zwykly*/
      } else
        is_param[psp++] = 0; /* nieokreslony*/
    }                        /*switch*/
    licz++;
    if (bufor[col++] != ',')
      return licz;
  } /*while*/
  return licz;
}

void macro_def(void) {
  /* definicja makrorozkazu
   */
  char c, firstchar;
  int ile;
  struct macro *mptr;
  struct mac_list *mc_lst;

  while ((c = bufor[col]) == ' ' || c == tab)
    col++;
  firstchar = col;
  if (!fcltab[c]) {
    error("Bad macro name");
    return;
  }
  col++;
  while (labtab[bufor[col]])
    col++;
  mptr = (struct macro *)m_alloc(sizeof(struct macro));
  mptr->namelen = col - firstchar;
  mptr->mname = (char *)m_alloc(mptr->namelen);
  memmove(mptr->mname, &bufor[firstchar], mptr->namelen);
  mptr->mbody = NULL;
  mptr->last_line = NULL;
  do {
    if (!(ile = readln())) {
      error("Missing .endm");
      break;
    }
    col = 0;
    while ((c = line_bufor[col]) == ' ' || c == tab)
      col++;
    if (thesame(&line_bufor[col], ".endm", 5))
      break;
    if (mptr->mbody == NULL) {
      mptr->mbody = (struct macroline *)m_alloc(sizeof(struct macroline));
      mptr->last_line = mptr->mbody;
    } else {
      mptr->last_line->nxt = (struct macroline *)m_alloc(sizeof(struct macroline));
      mptr->last_line = mptr->last_line->nxt;
    }
    mptr->last_line->nxt = NULL;
    mptr->last_line->linebody = (char *)m_alloc(ile + 1);
    memmove(mptr->last_line->linebody, bufor, ile + 1);
  } while (1);
  mptr->next = NULL;
  mc_lst = &mac_ptrs[hash_it(mptr->mname, mptr->namelen)];

  if (mc_lst->firstmacro == NULL)
    mc_lst->firstmacro = mptr;
  else
    mc_lst->lastmacro->next = mptr;

  mc_lst->lastmacro = mptr;
}

void macro_skip(void) {
  /* ominiecie Macro definicji - w drugiej fazie */
  char c;

  do {
    if (listflg && (clistflg || asmflg))
      print_line(0);
    if (!readln())
      break;
    col = 0;
    while ((c = bufor[col]) == ' ' || c == tab)
      col++;
    if (thesame(&bufor[col], ".endm", 5))
      break;
  } while (1);
}

struct macro *is_macro(char *mname, int ile)
/* spr. czy jest takie macro
 */
{
  struct macro *mptr;

  mptr = mac_ptrs[hash_it(mname, ile)].firstmacro;
  while (mptr != NULL) {
    if (mptr->namelen == ile && thesame(mname, mptr->mname, ile))
      return mptr; /* jest */
    mptr = mptr->next;
  }
  return NULL; /* nie ma */
}

void macro_analizing(struct macro *mptr) {
  char *prev_bufor;
  struct macroline *mbody;
  int first_param, prev_first;

  inside_macro = 1;
  first_param = psp++;
  prev_first = first_par;
  first_par = first_param;
  param[first_par] = read_param();                        /* param zero ilosc parametrow*/
  text_param[first_par] = (char *)m_alloc(mptr->namelen); /*oraz nazwa makro*/
  memmove(text_param[first_par], mptr->mname, mptr->namelen);
  is_param[first_par] = 2;

  prev_bufor = bufor;
  for (mbody = mptr->mbody; mbody != NULL; mbody = mbody->nxt) {
    bufor = mbody->linebody;
    analizing(); /* i jej analiza phase 1*/
  }
  bufor = prev_bufor;
  while (psp > first_param) {
    psp--;
    if (is_param[psp] == 2)
      free(text_param[psp]);
  }
  first_par = prev_first;
}

void put_macro(char firstchar) {
  struct macroline *mbody;
  char *prev_bufor;
  struct macro *mptr;
  int16_t adres_tmp;
  int first_param, prev_first;
  char col_tmp;
  if ((mptr = is_macro(&bufor[firstchar], col - firstchar)) != NULL) {
    if (listflg && (clistflg || asmflg))
      print_line(1);
    prev_bufor = bufor;
    col_tmp = col;
    macro_nest++;
    phase = 1;
    adres_tmp = adres;
    macro_analizing(mptr); /* dodatkowy przebieg */
    adres = adres_tmp;
    phase = 2;
    bufor = prev_bufor;
    col = col_tmp;
    first_param = psp++;
    prev_first = first_par;
    first_par = first_param;
    param[first_par] = read_param();
    /* param zero ilosc parametrow*/
    text_param[first_par] = (char *)m_alloc(mptr->namelen);
    /*oraz nazwa makro*/
    memmove(text_param[first_par], mptr->mname, mptr->namelen);
    is_param[first_par] = 2;
    prev_bufor = bufor;
    for (mbody = mptr->mbody; mbody != NULL; mbody = mbody->nxt) {
      bufor = mbody->linebody;
      prn_lin = 1; /*nie byla printowana*/
      putcode();   /* i putcode*/
      numcnt++;
      if (listflg && (clistflg || asmflg) && prn_lin) {
        /* w prn_lin znacznik zeby nie powtarzac*/
        print_line(0);
        prn_lin = 0;
      }
    }
    bufor = prev_bufor;
    while (psp > first_param) {
      psp--;
      if (is_param[psp] == 2)
        free(text_param[psp]);
    }
    first_par = prev_first;
    macro_nest--;
  } else {
    error("Undefined macro");
  }
}

void clearmacros(void) {
  /* niszczenie listy Macro instrukcji
   */
  struct macro *mptr, *firstmacro;
  struct macroline *lptr;
  int k;
  for (k = 0; k < 0x800; k++) {
    firstmacro = mac_ptrs[k].firstmacro;
    while (firstmacro != NULL) {
      free(firstmacro->mname);
      while (firstmacro->mbody != NULL) {
        lptr = firstmacro->mbody;
        firstmacro->mbody = firstmacro->mbody->nxt;
        free(lptr->linebody); /* zawartosc linii*/
        free(lptr);           /*struct macroline*/
      }
      mptr = firstmacro;
      firstmacro = firstmacro->next;
      free(mptr); /*struct macro*/
    }
  }
}

struct macro *mac_insert(struct macro *orderlist, struct macro *mptr) {
  int cmp;
  struct macro *mcrpt;
  mcrpt = orderlist;
  /*adds label to label list in alfabetical order.*/
  if (mcrpt == NULL) {
    mptr->next = NULL;
    return mptr;
  }
  if ((cmp = strnicmp(mcrpt->mname, mptr->mname, (mcrpt->namelen > mptr->namelen) ? mptr->namelen : mcrpt->namelen)) >
          0 ||
      cmp == 0 && mcrpt->namelen >= mptr->namelen) {
    mptr->next = mcrpt;
    return mptr;
  }
  while (mcrpt->next != NULL &&
         ((cmp = strnicmp(mcrpt->next->mname, mptr->mname,
                          (mcrpt->next->namelen > mptr->namelen) ? mptr->namelen : mcrpt->next->namelen)) < 0 ||
          cmp == 0 && mcrpt->next->namelen < mptr->namelen))
    mcrpt = mcrpt->next;
  mptr->next = mcrpt->next;
  mcrpt->next = mptr;
  return orderlist;
}

void printmacros(void) {
  int k;
  char l, cnt;
  struct macro *mptr, *firstmacro, *orderlist;
  struct macroline *lptr;
  orderlist = NULL;
  for (k = 0; k < 0x0800; k++) {
    firstmacro = mac_ptrs[k].firstmacro;
    while (firstmacro != NULL) {
      mptr = firstmacro;
      firstmacro = firstmacro->next;
      orderlist = mac_insert(orderlist, mptr);
    }
  }

  fprintf(flist, "\n defined macros:\n");
  cnt = 0;
  while (orderlist != NULL) {
    if (!fatal) {
      memmove((void *)strbuf, (void *)orderlist->mname, (l = orderlist->namelen));
      strbuf[l] = 0;
      fprintf(flist, " %s", strbuf);
      if (!((++cnt) & 3))
        fprintf(flist, "\n");
      else
        for (; l < 19; l++)
          fprintf(flist, " ");
    }
    free(orderlist->mname);
    while (orderlist->mbody != NULL) {
      lptr = orderlist->mbody;
      orderlist->mbody = orderlist->mbody->nxt;
      free(lptr->linebody); /* zawartosc linii*/
      free(lptr);           /*struct macroline*/
    }
    mptr = orderlist;
    orderlist = orderlist->next;
    free(mptr); /*struct macro*/
  }
}

// end include macros.c

void analizing(void) {
  char c, firstchar;
  char *adrbuf;
  col = 0;
  if (!getlabel()) {
    while ((c = bufor[col]) == ' ' || c == tab)
      col++;
    if (!asmflg && c != '.')
      return; /* gdy asemblacja wylanczona */
    switch (c) {
    case '*':
      if (!col)
        return; /*komentarz*/
      col++;
      while ((c = bufor[col++]) == ' ' || c == tab)
        ;
      if (c != '=') {
        error("can't evaluate new value for  *=");
        return;
      }
      while ((c = bufor[col]) == ' ' || c == tab)
        col++;
      firstchar = col;
      while (((c = bufor[col]) != ' ' || bufor[col + 1] != ' ') && c != ';' && c != crt && c != tab)
        col++;
      if (!evaluate(firstchar, col, (int16_t *)&adres))
        ;
      /*  error ("can't evaluate new value for *=");*/
      break;
    case '.': /*dyrektywa*/
      col++;
      adrbuf = &bufor[col];
      if (thesame(adrbuf, _if, 2)) {
        col += 2;
        asm_if();
        break;
      }
      if (thesame(adrbuf, _endif, 5)) {
        col += 5;
        asm_endif();
        break;
      }
      if (thesame(adrbuf, _else, 4)) {
        col += 4;
        asm_else();
        break;
      }
      if (!asmflg)
        break;
      if (thesame(adrbuf, _byte, 4)) {
        col += 4;
        adres += cnt_byte();
        break;
      }
      if (thesame(&bufor[col + 1], _byte, 4)) { /*.sbyte , .cbyte i .dbyte*/
        if (thesame(adrbuf, "s", 1)) {
          col += 5;
          adres += cnt_byte();
          break;
        }
        if (thesame(adrbuf, "c", 1)) {
          col += 5;
          adres += cnt_byte();
          break;
        }
        if (thesame(adrbuf, "d", 1)) {
          col += 5;
          adres += cnt_word();
          break;
        }
      }
      if (thesame(adrbuf, _word, 4)) {
        col += 4;
        adres += cnt_word();
        break;
      }
      if (thesame(adrbuf, _local, 5)) {
        col += 5;
        local();
        break;
      }
      if (thesame(adrbuf, _include, 7)) {
        col += 7;
        include(0);
        break;
      }
      if (thesame(adrbuf, _macro, 5)) {
        col += 5;
        macro_def();
        break;
      }
      if (thesame(adrbuf, _incbin, 6)) {
        col += 6;
        include(1);
        break;
      }
      if (thesame(adrbuf, _rept, 4)) {
        rept(0);
        break;
      }
      if (thesame(adrbuf, _endr, 4)) {
        endr(0);
        break;
      }
      if (thesame(adrbuf, _float, 5)) {
        col += 5;
        adres += 3 * cnt_word();
        break;
      }
      break;
    default:
      getinstruction();
    }
  }
}

void putcode(void) {
  char c, firstchar;
  char *adrbuf;

  col = licz_list = 0;
  /*
  switch(bufor[col])
    { case crt: case ';': return;
      case ' '          :break;
      default: while ((c=bufor[col])!=' '&& c!=tab&& c!=crt) col++;
    }
  */
  if (!checklabel()) {
    while ((c = bufor[col]) == ' ' || c == tab)
      col++;

    if (!asmflg && c != '.')
      return; /* gdy asemblacja wylanczona */
    switch (c) {
    case '*':
      if (!col)
        return; /*komentarz*/
      col++;
      while ((c = bufor[col++]) == ' ' || c == tab)
        ;
      if (c != '=') {
        error("can't evaluate new value for *=");
        return;
      }
      while ((c = bufor[col]) == ' ' || c == tab)
        col++;
      firstchar = col;
      while (((c = bufor[col]) != ' ' || bufor[col + 1] != ' ') && c != ';' && c != crt && c != tab)
        col++;
      if (!evaluate(firstchar, col, (int16_t *)&adres))
        error("cant' evaluate new value for *=");
      if (reptflg)
        error("*= inside rept");
      break;

    case '.': /* dyrektywa */
      col++;
      adrbuf = &bufor[col];
      if (thesame(adrbuf, _if, 2)) {
        col += 2;
        asm_if();
        break;
      }
      if (thesame(adrbuf, _endif, 5)) {
        col += 5;
        asm_endif();
        break;
      }
      if (thesame(adrbuf, _else, 4)) {
        col += 4;
        asm_else();
        break;
      }
      if (!asmflg)
        break;
      if (thesame(adrbuf, _byte, 4)) {
        col += 4;
        put_byte(0);
        break;
      }
      if (thesame(&bufor[col + 1], _byte, 4)) { /*.sbyte , .cbyte i .dbyte*/
        if (thesame(adrbuf, "s", 1)) {
          col += 5;
          put_byte(1);
          break;
        }
        if (thesame(adrbuf, "c", 1)) {
          col += 5;
          put_byte(2);
          break;
        }
        if (thesame(adrbuf, "d", 1)) {
          col += 5;
          put_word(1);
          break;
        }
      }
      if (thesame(adrbuf, _word, 4)) {
        col += 4;
        put_word(0);
        break;
      }
      if (thesame(adrbuf, _opt, 3)) {
        col += 3;
        optexec();
        break;
      }
      if (thesame(adrbuf, _local, 5)) {
        col += 5;
        local();
        break;
      }
      if (thesame(adrbuf, _include, 7)) {
        col += 7;
        include(0);
        break;
      }
      if (thesame(adrbuf, _macro, 5)) {
        col += 5;
        macro_skip();
        break;
      }
      if (thesame(adrbuf, _incbin, 6)) {
        col += 6;
        include(2);
        break;
      }
      if (thesame(adrbuf, _rept, 4)) {
        rept(1);
        break;
      }
      if (thesame(adrbuf, _endr, 4)) {
        endr(1);
        break;
      }
      if (thesame(adrbuf, _float, 5)) {
        col += 5;
        put_float();
        break;
      }
      if (thesame(adrbuf, _error, 5)) {
        col += 5;
        err_tit_pag(0);
        break;
      }
      if (thesame(adrbuf, _title, 5)) {
        col += 5;
        err_tit_pag(1);
        break;
      }
      if (thesame(adrbuf, _page, 4)) {
        col += 4;
        page();
        break;
      }

      if (thesame(adrbuf, _set, 3)) {
        col += 3;
        set();
        break;
      }
      if (thesame(adrbuf, _tab, 3)) {
        col += 3;
        tab_();
        break;
      }
      if (thesame(adrbuf, _endm, 4)) {
        error("Bad .endm");
        break;
      }

      break;
    default:
      if ((c = bufor[col]) == crt || c == ';')
        return;
      firstchar = col;
      while ((c = bufor[col]) != ' ' && c != tab && c != crt)
        col++;
      if ((col - firstchar) == 3 && (c = mnemnr(firstchar)) != -1)
        putinstr(mnemnr(firstchar));
      else
        put_macro(firstchar);
    }
  }
}

int sourceend(void) {
  /* zwraca 1 gdy koniec zrodlowki
            0 gdy jeszcze jest
  */

  while (feof(source)) /* koniec pliku */
  {
    if (fsp == 0)
      return 1; /* koniec bo nie ma plikow na stosie */
    fclose(source);
    source = filestack[--fsp]; /* plik ze stosu i dalej */
    free(filepath);
    filepath = fpthstack[fsp];
    line_inc = linestack[fsp];
  } /* moze nowy plik tez sie skonczyl?*/
  return 0;
}
void prn_info(void) {
  fprintf(stderr, "\n\n");
  fprintf(stderr, "  Usage:        OMC_6502 [Options] source [destination]\n");
  fprintf(stderr, "  Options:\n");
  fprintf(stderr, "    -L          - no assembly listing\n");
  fprintf(stderr, "    -S          - symbols listing\n");
  fprintf(stderr, "    -O          - no output\n");
  fprintf(stderr, "    -H          - hex output\n");
  fprintf(stderr, "    -A          - only code output, no addres bytes\n");
  fprintf(stderr, "    -I#filespec - assembly listing to specified file\n");
  fprintf(stderr, "    -IP         - assembly listing to printer\n");
  fprintf(stderr, "    -E#filespec - error listing to specified file\n");
  fprintf(stderr, "    -EP         - error listing to printer\n");
  fprintf(stderr, "    -N          - no error lines listing\n");
  fprintf(stderr, "    -M          - standard (252 bytes) buffer out size\n");
  fprintf(stderr, "  While assembling:\n");
  fprintf(stderr, "     S           - start, stop listing\n");
  fprintf(stderr, "    ESC          - break assembling\n");
  fprintf(stderr, "\n\n");
  fprintf(stderr, " press any key to quit.\n");
}

void setflags(int lstf) {
  objflg = 1;
  listflg = lstf;
  errflg = ejectflg = mlistflg = clistflg = xrefflg = 1;
  numflg = 0;
  set_dat[0] = 4;
  set_dat[1] = 0;
  set_dat[2] = 80;
  set_dat[3] = 12;
  set_dat[4] = 66;
  tab_dat[0] = 8;
  tab_dat[1] = 12;
  tab_dat[2] = 28;
  title_dat[0] = page_dat[0] = 0;
}

void get_pth(char *pth) { _getcwd(pth, _MAX_PATH); }

int set_pth(char *pth) { return _chdir(pth); }

int main(int argc, const char *argv[]) {
  char *param;
  static char srcpth[_MAX_PATH + 1], dstpth[_MAX_PATH + 1], thispth[_MAX_PATH + 1];
  const char endinfo[] = "\n %ld lines assembled, %d assembly error(s).\n";
  int i, j, k, bsl, symbol;
  printf("%s", our5oft);
  symbol = fatal = err_num_only = 0;
  output = lstf = adresbytes = 1;
  get_pth(thispth); /*zapmietanie aktulnej sciezki*/
  init_tabs();
  bufor = line_bufor;
  bptr = 0;
  buf_out_len = 0xff00;
  psp = first_par = 0;
  err_cnt = 0;
  flist = stdout; /*fopen("listing.out","w");*/
  ferr = flist;   /*NULL*/
  g_list_output = OUTPUT_STDOUT;
  g_err_output = OUTPUT_STDOUT;
  asmflg = 1;
  i_stack[0] = 0;
  asm_stack[0] = 1;
  if (argc <= 1) {
    prn_info();
    return 0;
  } else {
    /* otwieramy source */
    for (i = 1; i < argc && (*(param = (char *)argv[i]) == '-' || *param == '/') && !fatal; i++)
      ;
    if (i == argc && !fatal) {
      fatal = 1;
      error("source not specified");
    } else if (!fatal) {
      param = (char *)argv[i];
      k = 0;
      //--------------------
      for (j = 0; param[j] != 0; j++) {
        if ((srcpth[j] = param[j]) == '\\')
          k = j;
      }
      //--------------------
      srcpth[j] = 0;
      bsl = k;
      for (; k < j && param[k] != '.'; k++)
        ;
      if (k == j)
        memcpy(&srcpth[k], ".S65", 5);
      source = fopen(srcpth, "r");
      if (bsl) {
        srcpth[bsl] = 0;
        set_pth(srcpth); /*path to source is main path*/
        srcpth[bsl] = '\\';
      }
      if (source == NULL) {
        fatal = 1;
        error("can't open source file");
      }
      //       strcpy(srcpth,sciezka);
      strcpy(sciezka, srcpth);
    } /*bierzemy parametry*/
    for (i = 1; i < argc && (*(param = (char *)argv[i]) == '-' || *param == '/') && !fatal; i++) {
      param++;
      switch (*param) {
      case 'L':
      case 'l':
        lstf = 0;
        break;
      case 'O':
      case 'o':
        output = 0;
        break;
      case 'H':
      case 'h':
        output = 2;
        break;
      case 'A':
      case 'a':
        adresbytes = 0;
        break;
      case 'S':
      case 's':
        symbol = 1;
        break;
      case 'N':
      case 'n':
        err_num_only = 1;
      case 'M':
      case 'm':
        buf_out_len = 252;
        break;
      case 'I':
      case 'i':
        param++;
        if (*param == 'P' || *param == 'p') {
          g_list_output = OUTPUT_PRN;
          flist = stdout;
          break;
        }
        if (*param == '#') {
          g_list_output = OUTPUT_FILE;
          flist = fopen(param + 1, "w");
          if (flist != NULL) {
            fprintf(flist, "%s", our5oft);
            break;
          }
        }
        flist = stdout;
        fatal = 1;
        error("can't open listing file");
        break;
      case 'E':
      case 'e':
        param++;
        if (*param == 'P' || *param == 'p') {
          g_err_output = OUTPUT_PRN;
          ferr = stdout;
          break;
        }
        if (*param == '#') {
          g_err_output = OUTPUT_FILE;
          errpath = param + 1;
          ferr = NULL;
          break;
        }
        fatal = 1;
        error("bad -E? parameter");
        break;
      default:
        fatal = 1;
        error("bad parameter");
      }
    }
    /* otwieramy destnation*/
    if (++i == argc && !fatal) { /*no destination*/
      for (j = 0; (dstpth[j] = srcpth[j]) != 0; j++) {
        if (dstpth[j] == '.')
          k = j;
      }

      memcpy(&dstpth[k], ".COM", 5);
    } else if (!fatal) {
      param = (char *)argv[i];
      for (j = 0; (dstpth[j] = param[j]) != 0; j++)
        ;
      dstpth[j] = 0;
    }
    if (output && !fatal) {
      dest = fopen(dstpth, "wb");
      if (dest == NULL) {
        fatal = 1;
        error("can/t open destination file");
      }
    }
    strcpy(dstpth, sciezka);
  }
  buf_out.bytes = (char(*)[])m_alloc(buf_out_len);
  if (!fatal) {
    setflags(lstf);
    line = adres = fsp = 0;
    /*pierwsza linia ma nr 1, bo po readln jest line++*/

    page_cnt = 0;
    prn_ln_cnt = set_dat[4] - 1;
    prn_lin = 0;
    char_out = asem_col;
    fprintf(flist, "   source  :%s\n", srcpth);
    if (output)
      fprintf(flist, "destination:%s\n", dstpth);
    else
      fprintf(flist, "  no output\n");
    filepath = srcpth;
    line_inc = reptflg = 0;
    print_line(0);
    phase = 1;
    fprintf(flist, "\n Phase 1\n");
    localcnt = 0;
  }
  while (!fatal && !sourceend()) {
    inside_macro = 0;
    (void)readln();
    analizing();
  }
  if (!fatal) {
    phase = 2;
    fprintf(flist, " Phase 2\n");
    localcnt = 0;
    macro_nest = 0;
    numcnt = 100;
    setflags(lstf);
    asmflg = 1;
    i_stack[0] = 0;
    asm_stack[0] = 1;
    line = 0;
    fseek(source, 0, SEEK_SET);
    filepath = srcpth;
    line_inc = reptflg = 0;
  }
  while (!fatal && !sourceend()) {
    inside_macro = 0;
    if (!readln())
      continue;
    putcode();
    if (listflg && !inside_macro && (clistflg || asmflg) && prn_lin)
      print_line(0);
    numcnt++;
  }
  if (reptflg)
    error(".endr expected");
  /* oproznienie bufora wyjsciowego */
  clr_buf_out();
  if (output && dest != NULL)
    fclose(dest);
  if (source != NULL)
    fclose(source);
  if ((listflg || symbol) && !fatal) {
    printlabels();
    printmacros();
  } else {
    clearlabels();
    clearmacros();
  }
  fprintf(flist, endinfo, line, err_cnt);
  if (ferr != flist && ferr != NULL)
    fprintf(ferr, endinfo, line, err_cnt);
  fclose(flist);
  if (ferr != NULL)
    fclose(ferr);
  set_pth(thispth); /*odtworzenie sciezki */
  return 0;
}

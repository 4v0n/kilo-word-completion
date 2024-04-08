/*
  This file contains code from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file handles syntax highlighting
*/

#include <ctype.h>
#include <data.h>
#include <stdlib.h>
#include <string.h>
#include <terminal.h>

char *C_HL_extensions[] = {".c", ".h", ".cpp"};

char *C_HL_keywords[] = {"switch",    "if",      "while",   "for",    "break",
                         "continue",  "return",  "else",    "struct", "union",
                         "typedef",   "static",  "enum",    "class",  "case",
                         "int|",      "long|",   "double|", "float|", "char|",
                         "unsigned|", "signed|", "void|",   NULL};

char *LaTeX_HL_extensions[] = {".tex"};

char *LaTeX_HL_keywords[] = {
  "\\documentclass|", "\\usepackage|", "\\begin|", "\\end|", 
  "\\section|", "\\subsection|", "\\subsubsection|", "\\paragraph|", 
  "\\subparagraph|", "\\tableofcontents", "\\listoffigures", "\\listoftables", 
  "\\chapter|",
  "\\maketitle", "\\author|", "\\date|", "\\title|", 
  "\\newpage", "\\newline", "\\textbf|", "\\textit|", 
  "\\underline|", "\\emph|", "\\texttt|", "\\textsc|", 
  "\\small", "\\normalsize", "\\large", "\\Large", 
  "\\LARGE", "\\huge", "\\Huge", "\\label|", 
  "\\ref|", "\\pageref|", "\\footnote|", "\\bibliography|", 
  "\\bibliographystyle|", "\\cite|", "\\newcommand|", "\\renewcommand|", 
  "\\newenvironment|", "\\renewenvironment|", "\\pagebreak", "\\linebreak", 
  "\\include|", "\\input|", "\\appendix", "\\abstract", 
  "\\textsuperscript|", "\\textsubscript|", "\\includegraphics|", "\\url|", 
  "\\hyperref|", "\\item", "\\itemize", "\\enumerate", 
  "\\description", "\\matrix|", "\\pmatrix|", "\\bmatrix|", 
  "\\vmatrix|", "\\Vmatrix|", "\\cases|", "\\align|", 
  "\\equation|", "\\sqrt|", "\\frac|", "\\sum|", 
  "\\product|", "\\integrate|", "\\limit|", "\\infinity", 
  "\\partial|", "\\nabla|", "\\exists|", "\\forall|", 
  "\\emptyset", "\\in", "\\notin", "\\subset", 
  "\\supset", "\\union", "\\intersect", "\\setminus", 
  "\\to", "\\gets", "\\Rightarrow|", "\\Leftarrow|", 
  "\\Leftrightarrow|",
  "\\\\", "&", "$", "$$", "\\[", "\\]",
  NULL
};

struct KV LaTeX_shortcuts[] = {
  {"itemize","\\begin{itemize}|"},
  {"beg", "\\begin|"},
};

enum IDs { C = 0, LaTeX };

struct editorSyntax HLDB[] = {
    {C, "c", C_HL_extensions, C_HL_keywords, NULL, "//", "/*", "*/",
     HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS},
    {LaTeX, "LaTeX", LaTeX_HL_extensions, LaTeX_HL_keywords, LaTeX_shortcuts, "%",
     "\\begin{comment}", "\\end{comment}", HL_HIGHLIGHT_STRINGS}};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

int is_separator(int c) {
  struct editorConfig *E = getEditorConfig();

  switch (E->syntax->id) {
  case C:
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
    break;
  case LaTeX:
    return isspace(c) || c == '\0' || strchr("{}[]()^_~", c) != NULL;
    break;
  }

  return -1;
}

// Updates the syntax highlighting for a row of text
void editorUpdateSyntax(erow *row) {
  struct editorConfig *E = getEditorConfig();

  // resize highlight array to match render size
  row->hl = realloc(row->hl, row->rsize);
  memset(row->hl, HL_NORMAL, row->rsize);

  // abort if syntax is not defined
  if (E->syntax == NULL)
    return;

  // syntax highlighting shortcuts
  char **keywords = E->syntax->keywords;
  char *scs = E->syntax->singleline_comment_start;
  char *mcs = E->syntax->multiline_comment_start;
  char *mce = E->syntax->multiline_comment_end;

  // calculate lengths of comment start and end markers
  int scs_len = scs ? strlen(scs) : 0;
  int mcs_len = mcs ? strlen(mcs) : 0;
  int mce_len = mce ? strlen(mce) : 0;

  // initialise state variables
  int prev_sep = 1;
  int in_string = 0;
  int in_comment = (row->idx > 0 && E->row[row->idx - 1].hl_open_comment);

  int i = 0;
  while (i < row->rsize) {
    char c = row->render[i];
    unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;

    // handle single line comments
    if (scs_len && !in_string && !in_comment) {
      if (!strncmp(&row->render[i], scs, scs_len)) {
        // highlight whole row
        memset(&row->hl[i], HL_COMMENT, row->rsize - i);
        break;
      }
    }

    // handle multi-line comments
    if (mcs_len && mce_len && !in_string) {
      if (in_comment) {
        row->hl[i] = HL_MLCOMMENT;
        if (!strncmp(&row->render[i], mce, mce_len)) {
          // highlight row until comment end
          memset(&row->hl[i], HL_MLCOMMENT, mce_len);
          i += mce_len; // skip past comment end marker
          in_comment = 0;
          prev_sep = 1;
          continue;
        } else {
          i++;
          continue;
        }
      } else if (!strncmp(&row->render[i], mcs, mcs_len)) {
        // highlight start of multiline comment
        memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
        i += mcs_len;
        in_comment = 1;
        continue;
      }
    }

    // Handle strings
    if (E->syntax->flags & HL_HIGHLIGHT_STRINGS) {
      if (in_string) {
        row->hl[i] = HL_STRING;                // highlight character
        if (c == '\\' && i + 1 < row->rsize) { // escape sequence
          row->hl[i + 1] = HL_STRING;          // highlight next character
          i += 2;                              // skip escaped char
          continue;
        }

        // end if string if matching quote found
        if (c == in_string)
          in_string = 0;

        i++;
        prev_sep = 1;
        continue;

      } else {
        if (c == '"' || c == '\'') { // start of string
          in_string = c;             // mark quote char used to start string
          row->hl[i] = HL_STRING;
          i++;
          continue;
        }
      }
    }

    // handle numbers
    if (E->syntax->flags & HL_HIGHLIGHT_NUMBERS) {
      // if character is a digit or . in float
      if ((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) ||
          (c == '.' && prev_hl == HL_NUMBER)) {
        row->hl[i] = HL_NUMBER; // highlight char
        i++;
        prev_sep = 0;
        continue;
      }
    }

    // handle keywords
    if (prev_sep) {
      int j;
      for (j = 0; keywords[j]; j++) {
        int klen = strlen(keywords[j]);         // keyword length
        int kw2 = keywords[j][klen - 1] == '|'; // check if special keyword
        if (kw2)
          klen--; // Adjust length for special keyword

        if (!strncmp(&row->render[i], keywords[j], klen) &&
            is_separator(row->render[i + klen])) {
          // highlight keyword
          memset(&row->hl[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
          i += klen;
          break;
        }
      }
      if (keywords[j] != NULL) {
        prev_sep = 0; // next char is not seperator
        continue;
      }
    }

    prev_sep = is_separator(c);
    i++;
  }

  // check if multi line comment state has changed
  int changed = (row->hl_open_comment != in_comment);
  row->hl_open_comment = in_comment; // update open comment flag
  // update syntax highlighting for next row
  if (changed && row->idx + 1 < E->numrows)
    editorUpdateSyntax(&E->row[row->idx + 1]);
}

int editorSyntaxToColour(int hl) {
  switch (hl) {
  case HL_COMMENT:
  case HL_MLCOMMENT:
    return CYAN;
  case HL_KEYWORD1:
    return YELLOW;
  case HL_KEYWORD2:
    return GREEN;
  case HL_STRING:
    return MAGENTA;
  case HL_NUMBER:
    return RED;
  case HL_MATCH:
    return BLUE;
  default:
    return WHITE;
  }
}

// selects the appropriate syntax highlighting rules based on the file's
// extension
void editorSelectSyntaxHighlight() {
  struct editorConfig *E = getEditorConfig();

  E->syntax = NULL;
  if (E->filename == NULL)
    return; // no syntax highlighting available

  char *ext = strrchr(E->filename, '.');

  // iterate over list of highlighting databases
  for (unsigned int j = 0; j < HLDB_ENTRIES; j++) {
    struct editorSyntax *s = &HLDB[j];
    unsigned int i = 0;

    // loop through each filematch pattern
    while (s->filematch[i]) {
      int is_ext = (s->filematch[i][0] == '.');

      // if match
      if ((is_ext && ext && !strcmp(ext, s->filematch[i])) ||
          (!is_ext && strstr(E->filename, s->filematch[i]))) {
        E->syntax = s;

        // update syntax highlighting for each row
        int filerow;
        for (filerow = 0; filerow < E->numrows; filerow++) {
          editorUpdateSyntax(&E->row[filerow]);
        }

        return;
      }
      i++;
    }
  }
}
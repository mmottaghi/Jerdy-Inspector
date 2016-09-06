/***********************************************************************************
		Copyright (C) 2016 Mohammad D. Mottaghi

	Under the terms of the MIT license, permission is granted to anyone to use, copy, 
	modify, publish, redistribute, and/or sell copies of this source code for any 
	commercial and non-commercial purposes, subject to the following restrictions:
		
	1. The above copyright notice and this permission notice shall not be removed 
	from any source distribution.
	
	2. The origin of this file shall not be misrepresented; The name of the original 
	author shall be cited in any copy, or modified version of this source code.
	
	3. If used in a product, acknowledgment in the product documentation would be
	appreciated, but is not required.

	4. Modified versions must be plainly marked as such, and must not be 
	misrepresented as being the original source code.

	This source code is provided "as is", without warranty of any kind, express or 
	implied, including but not limited to the warranties of merchantability, fitness 
	for a particular purpose and noninfringement. In no event shall the author
	or copyright holders be liable for any claim, damages or other liability, 
	whether in an action of contract, tort or otherwise, arising from, out of or 
	in connection with this source code or the use or other dealings in it.

		Mohammd Mottaghi Dastjerdi (mamad[a~t]cs.duke.edu)
		Sep. 1,2016

***********************************************************************************/

#pragma hdrstop

#include "jStrScan.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

CCharRange::CCharRange(const char* mixed) /* mixture of intervals and single characters*/
{
  if (long len = strlen(mixed))
  { // extract ranges e.g.  0-9A-Zb-g
    char_set = new char[++len];
    intervals = new char[len]; // stores ranges in the form of char-pairs terminated by zero

    char* csp=char_set, *intp=intervals;
    if (mixed[1] != '-')
      *csp++ = *mixed++;
    for (char chr ; (chr=*mixed)!=0 ; mixed++)
      if (chr=='-')
      {
        *intp++ = mixed[-1];
        *intp++ = mixed[1];
        mixed++;
        --csp;
      }
      else
        *csp++ = chr;
    *csp=0;
    *intp=0;
  }
  else
  {
    char_set = NULL;
    intervals = NULL;
  }
}
//------------------------------------------------------------------------------
bool CCharRange::Contains(char chr) const
{
  if (!strchr(char_set, chr))
  {
    for (char *lb = intervals ; *lb ; ++ ++lb)
      if (chr >= *lb && chr <= *(lb+1))
        return true;
    return false;
  }
  return true;
}
//==============================================================================
void CjStringSpan::ReplaceEscapeSequences(char* str)
{
  for (char *rest=str, *end=str+strlen(str), *bslash ; (bslash = strchr(rest, '\\'))!=0 ;)
  {
    switch(bslash[1])
    {
      case '}':  //   \}
      case '>':  //   \>
      case '\\':  //   \\
      case '$':  //   \$
      case '`':  //   \`
        bslash[0]=bslash[1];
        break;
      case 'r':
        bslash[0] = '\r';
        break;
      case 't':
        bslash[0] = '\t';
        break;
      case 'n':
        bslash[0] = '\n';
        break;
      default:
        rest++;
        continue;
    }
    memcpy(bslash+1, bslash+2, --end-bslash);
    rest=bslash+1;
  }
}
//------------------------------------------------------------------------------
SStringClip CjStringSpan::Clip(bool trim, long endOffset) const
{
  const char* left = start_mark, * right = end_mark - endOffset;
  if (trim && start_mark[0])
  {
    while(true) // trim right
      if (--right==start_mark || *right!=32)
      {
        right++;
        break;
      }
    for ( ; *left==' ' ; left++); // trim left
  }

  SStringClip s;
  s.Set(left, (long)right - (long)left /* String Length*/);
  return s;
}
//------------------------------------------------------------------------------
const char * CjStringSpan::Extract(bool replaceEscapes, bool trim, long *strLen, long endOffset, char *destination)
{
  SStringClip span = this->Clip(trim, endOffset);

  char *extracted = (char*)span.Clone(destination);
  if (extracted && replaceEscapes)
      ReplaceEscapeSequences(extracted);
  if (strLen)
    *strLen = span.length;

  return extracted;
}
//==============================================================================
void CjStrScan::Initialize(int buf_size, const char * source)
{
  is_copy = false;
  if (buf_size > 0)
  {
    buf = new char[buf_size+1];
    buf[0]=0;
  }
  else
    buf = NULL;
  start_mark = end_mark = source;
  inline_comment_symbol=-1;
}
//------------------------------------------------------------------------------
long CjStrScan::WhiteSpaceLen()
{
  const char *s=end_mark;
  for(char c=*s ; c&&(c=='\t' || c=='\r' || c=='\n' || c==' '); c=*++s);
  return (long)s-(long)end_mark;
}
//------------------------------------------------------------------------------
void CjStrScan::SetCommentSymbols(char inline_sym, char block_start_sym, const char* block_end_sym)
{
  inline_comment_symbol = inline_sym;
  block_comment_start_symbol = block_start_sym;
  strncpy(block_comment_end_seq, block_end_sym, 3);
}
//------------------------------------------------------------------------------
void CjStrScan::ApplyJump(const char* position, bool *search_reportback)
{
  if (*position)
    end_mark = position;
  else
  {
    if (search_reportback) // i.e., don't raise an exception
    {
      if (search_reportback == (bool *)-1)
        end_mark = --position;
      else
        *search_reportback = false;
    }
    else
      throw sseMarkNotFound;
  }
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::JumpTo(char mark, bool *exception_or_false, long offset)
{
  const char* match_position = end_mark+offset;
  for ( ; *match_position ; match_position++)
    if (*match_position == mark)
      break;
  ApplyJump(match_position, exception_or_false);
  return *this;
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::JumpTo(const char* mark, bool *exception_or_false, long offset)
{
  long cmpSpan = strlen(mark)-1;
  const char* match_position = end_mark+offset;
  for ( ; *match_position ; match_position++)
    if (*match_position == *mark && strncmp(mark+1, match_position+1, cmpSpan)==0)
      break;
  ApplyJump(match_position, exception_or_false);
  return *this;
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::RemoveChars(const char *skippedChars)
{
  char *wp = buf;
  for (const char *p=this->buf ; *p ; p++)
    if (!strchr(skippedChars, *p))
      *wp++ = *p;
  *wp=0;
  return *this;
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::Skip(const CCharRange &validRange, bool autoStart)
{
  if (autoStart)
    start_mark = end_mark;
  for ( ; *end_mark ; end_mark++)
    if (!validRange.Contains(*end_mark))
      break;
  return *this;
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::Skip(const char* validSet, bool autoStart)
{
  if (autoStart)
    start_mark = end_mark;
  for ( ; *end_mark ; end_mark++)
    if (!strchr(validSet, *end_mark))
      break;
  return *this;
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::SkipNot(const CCharRange &invalidRange, const char *escapeSet, bool autoStart)
{
  if (autoStart)
    start_mark = end_mark;
  for (; *end_mark ; end_mark++)
    if (*end_mark=='\\' && (end_mark[1]=='\\' || strchr(escapeSet, end_mark[1])))
      end_mark++;
    else if (invalidRange.Contains(*end_mark))
      break;
  return *this;
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::SkipNot(const char* stopperSet, const char *escapeSet, bool autoStart)
{
  if (autoStart)
    start_mark = end_mark;
  for (; *end_mark ; end_mark++)
    if (*end_mark=='\\' && (end_mark[1]=='\\' || strchr(escapeSet, end_mark[1])))
      end_mark++;
    else if (strchr(stopperSet, *end_mark))
      break;
  return *this;
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::Read(const CCharRange &validRange, bool skip_leading_whitespaces)
{
  if (skip_leading_whitespaces)
    SkipSpaces();

  Skip(validRange);
  Extract(false, false, NULL, 0, buf);
  return *this;
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::Read(const char *validRanges, bool skip_leading_whitespaces)
{
  CCharRange acceptables(validRanges);
  return Read(acceptables, skip_leading_whitespaces);
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::ReadNot(const char *invalidRangeMixed, const char *escapeSet, bool skip_leading_whitespaces)
{
  CCharRange unacceptables(invalidRangeMixed);
  return ReadNot(unacceptables, escapeSet, skip_leading_whitespaces);
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::ReadNot(const CCharRange &invalidRange, const char *escapeSet, bool skip_leading_whitespaces)
{
  if (skip_leading_whitespaces)
    SkipSpaces();
  if (escapeSet==NULL)
    escapeSet = "";

  SkipNot(invalidRange, escapeSet);
  Extract(false, false, NULL, 0, buf);
  return *this;
}
//------------------------------------------------------------------------------
CjStrScan& CjStrScan::SkipSpaces()
{
  for ( ; Skip(WhiteSpaceLen()), Peek() == inline_comment_symbol ; )
    if (At(1) == block_comment_start_symbol)         // e.g. for c:  * in /*
      Skip(2).JumpTo(block_comment_end_seq, (bool*)-1).Skip(block_comment_end_seq[1]==0? 1:2); // e.g.  */
    else
      JumpTo('\n', (bool*)-1).Skip(1);
  return *this;
}
//------------------------------------------------------------------------------
const char* CjStrScan::ExtractString(const char* wrappers, const CCharRange* validRangeIfBare, bool trim, long *strLenHolder)
{
  const char* extractedString;

  if (!Peek())
    extractedString = NULL;
  else if (strchr(wrappers, Peek()))
  { // e.g.  ' some value '
    char escapeSet[] = {Peek(),0};
    extractedString = Skip(1).SkipNot(escapeSet, escapeSet).Extract(true, trim, strLenHolder);
    Skip(1);
  }
  else // bare value (not wrapped)
    extractedString = validRangeIfBare? Skip(*validRangeIfBare).Extract(true, trim, strLenHolder) : NULL;

  if (!extractedString && strLenHolder)
    *strLenHolder = 0;
  return extractedString;
}



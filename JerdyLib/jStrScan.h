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

#include<string.h>
#include<mem.h>
#ifndef StrScanH
#define StrScanH
//---------------------------------------------------------------------------

enum enStrScanErr
{
  sseNoError,
  sseMarkNotFound
};
//---------------------------------------------------------------------------
class CCharRange
{
  private:
    char* char_set;
    char* intervals;// lb-up pairs, terminated by zero
  public:
    CCharRange(const char* mixedIntervalCharSet); // splits single chars from intervals
    ~CCharRange() {delete []char_set; delete []intervals;}
    bool Contains(char chr) const;
};
//---------------------------------------------------------------------------
struct SStringClip
{ // embedded in another string, this string is not necessarily terminated by zero
    const char*   start;
    unsigned long length;

    static SStringClip Clip(const char* s, long l) {SStringClip sp; sp.start = s; sp.length = l; return sp;}
    void operator<<(const char* s) {start=s; length=strlen(s);}
    bool operator==(const SStringClip& s2)  const {return length==s2.length && strncmpi(start, s2.start, length)==0;}
    void Set(const char* str, unsigned long len) {start = str; length = len;}
    const char*   Data()   const {return start;}
    const char*   Clone(char *destination_buffer=NULL)  const
    {// returns an ASCII-Z string
      if (length==0)
        return NULL;
      char* clone = destination_buffer? destination_buffer:new char[length+1];
      memcpy(clone, start, length);
      clone[length]=0;
      return clone;
    }
    bool operator<(const SStringClip& s2) const
    {
      int cmp = strncmpi(start, s2.start, length);
      return cmp==0? length < s2.length : cmp<0;
    }
};
//---------------------------------------------------------------------------
class CjStringSpan
{
  protected:
    const char* start_mark;
    const char* end_mark;

    CjStringSpan() {start_mark = end_mark = NULL;}

  public:
    CjStringSpan(const char*s, const char*e) {start_mark = s; end_mark = e;}
    CjStringSpan(const char*whole_string) {start_mark = whole_string; end_mark = whole_string+strlen(whole_string);}
    static void ReplaceEscapeSequences(char* str);
    SStringClip Clip(bool trim=false, long endOffset=0) const;
    const char * Extract(bool replaceEscapes = false, bool trim = false, long *strLen = NULL, long endOffset=0, char *destination=NULL);
    long StrLength()   const { return (long)end_mark - (long)start_mark; }
    const char * Str() const { return start_mark; }
};
//---------------------------------------------------------------------------
class CjStrScan : public CjStringSpan
{
  private:
    bool  is_copy; // if true, the destructor will NOT  delete []buf
    char  inline_comment_symbol;
    char  block_comment_start_symbol;
    char  block_comment_end_seq[3];
    char* buf;

    void Initialize(int buf_size, const char * source);
    void ApplyJump(const char* position, bool *search_reportback);

  public:
    CjStrScan() {Initialize(-1, NULL);}
    CjStrScan(const char *src)               {Initialize(strlen(src), src);}
    CjStrScan(int buf_size, const char *src) {Initialize(buf_size, src);}
    CjStrScan(const CjStrScan &original)      {*this=original; is_copy = true;}
    CjStrScan(const CjStrScan &original, const char *src) {*this=original; Seek(src);}
    ~CjStrScan()                             {if (!is_copy) delete []buf;}

    bool IsAtEnd() const {return *end_mark==0;}
    char At(int i) const {return end_mark[i];}
    char Peek()    const {return *end_mark;}
    bool Peek(char c) const  {return *end_mark==c;}
    char Peek(const char* search) const {const char*found = strchr(search, *end_mark); return found? *found:0; } // checks to see if the Cursor matches any of the given characters in 'search'
    const char* Cursor() const {return end_mark;}
    CjStringSpan Buf()    const {return CjStringSpan(buf);}
    void EmptyBuffer()         {buf[0]=0;}
    long WhiteSpaceLen();
    void ReplaceEscapeSequences();
    void SetCommentSymbols(char inline_sym, char block_start_sym, const char* block_end_sym);
    const char* ExtractString(const char* wrappers, const CCharRange* validRangeIfBare, bool trim=false, long *strLenHolder=NULL);
    CjStrScan& StartStr(long offset=0) {start_mark = end_mark+offset; return *this;}
    CjStrScan& SkipSpaces();
    CjStrScan& Skip(const CCharRange &validRange, bool autoStart = true);
    CjStrScan& Skip(const char* validSet, bool autoStart = true);
    CjStrScan& SkipNot(const CCharRange &stopperRange, const char *escapeSet="", bool autoStart = true);
    CjStrScan& SkipNot(const char* stopperSet, const char *escapeSet="", bool autoStart = true);
    CjStrScan& Skip(int n_chars)     {end_mark += n_chars; return *this;}
    CjStrScan& Seek(const char* pos) {if (pos) start_mark=end_mark=pos; return *this;}
    CjStrScan& JumpTo(char mark, bool * exception_or_false = NULL, long offset=0); // NULL means: throw exception if 'mark' not found
    CjStrScan& JumpTo(const char* mark, bool * exception_or_false = NULL, long offset=0); // NULL means: throw exception if 'mark' not found
    CjStrScan& JumpToEnd(long offset=0) {JumpTo('\x0', (bool*)-1); return *this;}
    CjStrScan& Read   (const CCharRange &validRange, bool skip_leading_whitespaces = true);
    CjStrScan& Read   (const char *validRangeMixed, bool skip_leading_whitespaces = true);
    CjStrScan& ReadNot(const char *invalidRangeMixed, const char *escapeSet=NULL, bool skip_leading_whitespaces = true);
    CjStrScan& ReadNot(const CCharRange &invalidRange, const char *escapeSet=NULL, bool skip_leading_whitespaces = true);
    CjStrScan& RemoveChars(const char *skippedChars);
    CjStrScan& ExtractClip(SStringClip& resulting_clip /*out*/, bool trim=false, long endOffset=0) {resulting_clip = Clip(trim, endOffset); return *this;}
};
#endif


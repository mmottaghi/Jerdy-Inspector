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
		Sep. 1, 2016

***********************************************************************************/

#include<mem.h>
#include<string.h>
#ifndef JStrRegH
#define JStrRegH
//---------------------------------------------------------------------------

#define BUCKET_SETS     16
#define BUCKET_SET_SIZE 16

//----------------------------------------------------------------------------
class CSharedObject
{
  private:
    bool shared;
  public:
    CSharedObject(bool is_shared) {shared = is_shared;}
    bool IsShared() const {return shared;}
};
//----------------------------------------------------------------------------
class CjStringBank : public CSharedObject
{
  private:
    const char* strings;  // set of all strings back-to-back (one byte before each string is reserved for the length)
    long total_strings_length;
    struct SBookmark
    { // for each letter (e.g. H) specifies where in 'strings' the correponding set starts (e.g. height, href, ...)
        short count;
        const char* set_start;
    } set_bookmarks[32];

  public:
    CjStringBank(const char* static_string_set, bool is_shared = false);
    const char* Exists(const char* str) const {return Exists(str, strlen(str));}
    const char* Exists(const char* str, long str_len) const;
    long Range()       const {return total_strings_length;}
    const char* Data() const {return strings;}
};
//----------------------------------------------------------------------------
class CBucket
{ // this class must be explicitly destroyed by calling the 'Free' function
  private:
    short signature;
    const char* key; // explicitly deleted by calling the 'Free' function

  public:
    CBucket() {key=NULL;}
    void  Free(const char* static_string_range_start, long static_string_range_length);

    bool  IsEmpty()        const {return key==NULL;}
    bool  HasCollision()   const {return signature > 0;}
    long  CollisionCount() const {return signature;}
    void  SetKey(short new_sig, const char* new_key);
    const char* Match(short cand_sig, const char* candidate_key, long key_length);
};
//----------------------------------------------------------------------------
class CjStringRegistry : public CSharedObject
{ // used for strings with multiple references: a Hash Table which maps each string to a unique identifier; enabling faster comparisons and lookups
  private:
    CBucket*  bucket_sets[BUCKET_SETS];
    const CjStringBank*  static_strings;

    struct SValueLocator
    {
      short bucket;
      short set;
      short signature;
    };
    SValueLocator HashFunction(const char* key, long key_len) const;
    void Initialize(const char* set_of_static_strings, const CjStringBank* string_bank);

  public:
    CjStringRegistry(const char*  concatenated_strings, bool shared=false) : CSharedObject(shared) {Initialize(concatenated_strings, NULL); }
    CjStringRegistry(const CjStringBank* static_str_bank, bool shared=false): CSharedObject(shared) {Initialize(NULL, static_str_bank); }
    ~CjStringRegistry();

    const char* Find(const char* attr, unsigned long attr_len) const;
    const char* Register(const char* attr, unsigned long attr_len);
    long CollisionCount();
};
#endif

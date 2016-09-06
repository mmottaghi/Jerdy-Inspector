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

#include "jStrReg.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

CjStringBank::CjStringBank(const char* static_string_set, bool is_shared): CSharedObject(is_shared)
{
  strings = static_string_set;
  long active_set = -1;
  for (const char* str = strings ;  ; str += 2+*str) // '*str' contains the str-len, and the last char is zero
  {// initializes the bookmarks, also sets string lengths
    if (!*str)
    {
      total_strings_length = 1 + (long)str - (long)strings;
      for (long r=active_set+1 ; r < 32 ; r++) // the rest of the sets are initialized to NULL
        ((SBookmark*)set_bookmarks)[r].set_start = NULL;
      break;
    }

    total_strings_length += *str; // the first char of each str contains its length and the last char is zero

    if ((*(str+1)&0x1F) == active_set) //if 'str' falls in the active set, increment its member count
      ((SBookmark*)set_bookmarks)[active_set].count++;
    else
    {
      if (long incr = (*(str+1)&0x1F) - active_set-1)
        for (long ii=0 ; ii < incr ; ii++)
          ((SBookmark*)set_bookmarks)[++active_set].set_start = NULL; // unsed sets, in between, are initialized to NULL
      active_set = *(str+1)&0x1F;
      ((SBookmark*)set_bookmarks)[active_set].count=1;
      ((SBookmark*)set_bookmarks)[active_set].set_start = str;
    }
  }
}
//---------------------------------------------------------------------------
const char* CjStringBank::Exists(const char* needle, long needle_len) const
{
  const char* set_with_same_initial_as_needle = set_bookmarks[*needle&0x1F].set_start;
  if (set_with_same_initial_as_needle)
    for (const char* candidate=set_with_same_initial_as_needle; *candidate ; candidate+= *candidate+2)
      if (*candidate==needle_len && strncmpi(needle, candidate+1, needle_len)==0)
        return candidate+1;
  return NULL;
}
//===========================================================================
const char* CBucket::Match(short cand_sig, const char* candidate_key, long key_len)
{
  if (!IsEmpty())
  {
    if (HasCollision())
    {
      CBucket * collision_bucket = (CBucket *)key;
      for (long i=signature ;  i > 0 ; i--, collision_bucket++)
        if (collision_bucket->Match(cand_sig, candidate_key, key_len))
          return collision_bucket->key;
    }
    else if (signature==cand_sig && strncmpi(key, candidate_key, key_len)==0)
      return key;
  }
  return NULL;
}
//---------------------------------------------------------------------------
void CBucket::SetKey(short new_sig, const char* new_key)
{
  CBucket* target_bucket;
  if (IsEmpty())
    target_bucket = this;
  else
  {  // collision occurs ('key' is initialized to NULL in the constructor)
    long collision_count;
    CBucket* old_collision_resolver;
    if (HasCollision()) // collisions already existed
    {
      collision_count = signature;
      old_collision_resolver = (CBucket*)key;
    }
    else
    {
      collision_count = 1;
      old_collision_resolver = this;
    }

    CBucket* new_collision_resolver = new CBucket[collision_count+1];
    memcpy(new_collision_resolver, old_collision_resolver, collision_count*sizeof(CBucket));
    if (collision_count > 1)
      delete []old_collision_resolver;

    signature = collision_count + 1;
    key = (const char*)new_collision_resolver;
    target_bucket = &new_collision_resolver[collision_count];
  }

  target_bucket->signature = new_sig;
  target_bucket->key = new_key;
}
//---------------------------------------------------------------------------
void  CBucket::Free(const char* static_string_range_start, long static_string_range_length)
{// deletes 'key' if it is NOT withing the specified range
  if (IsEmpty())
    return;
  if (HasCollision())
  {
    CBucket * collision_bucket = (CBucket *)key;
    for (long i=signature ;  i > 0 ; i--, collision_bucket++)
      collision_bucket->Free(static_string_range_start, static_string_range_length);
    delete [](CBucket *)key;
  }
  else if (key < static_string_range_start || key >= static_string_range_start + static_string_range_length)
    delete []key; // some keys must not be deleted because they might point to a static string
}
//===========================================================================
void CjStringRegistry::Initialize(const char* set_of_static_strings, const CjStringBank* string_bank)
{
  static_strings = string_bank? string_bank : new CjStringBank(set_of_static_strings);
  memset(bucket_sets, 0, sizeof(bucket_sets));
}
//---------------------------------------------------------------------------
CjStringRegistry::~CjStringRegistry()
{
  for (long s=BUCKET_SETS-1 ; s >= 0 ; s--)
  {
    CBucket* bucket = bucket_sets[s];
    if (!bucket)
      continue;
    for (long b=BUCKET_SET_SIZE ; b > 0 ; b--, bucket++)
      bucket->Free(static_strings->Data(), static_strings->Range());
    delete []bucket_sets[s];
  }
  if (!static_strings->IsShared())
    delete static_strings;
}
//---------------------------------------------------------------------------
const char* CjStringRegistry::Find(const char* attrib, unsigned long attr_len) const
{
  SValueLocator attr_hash = HashFunction(attrib, attr_len);
  if (CBucket* target_set = bucket_sets[attr_hash.set])
    return target_set[attr_hash.bucket].Match(attr_hash.signature, attrib, attr_len);
  return NULL;
}
//---------------------------------------------------------------------------
CjStringRegistry::SValueLocator CjStringRegistry::HashFunction(const char* key, long key_len) const
{// signature (hash) of the key
  char letter_1st  = 0x1F & *key;
  char letter_2nd  = 0x1F & *(key+1);
  char last_letter = 0x1F & *(key+key_len-1);

  SValueLocator h;
  h.signature = 0x8000 | (letter_1st<<10) | (last_letter<<5) | (0x001F&(short)key_len);
  char hash_value = (letter_1st << 4) | ((0x03 & letter_2nd) << 2) | (0x03 & last_letter);
  asm { // rotate right hash_value
    push ecx
    push eax
    mov al, hash_value

    mov ecx, key_len
    ror al, cl

    mov hash_value, al
    pop eax
    pop ecx
  }
  h.set = (hash_value>>4) & 0xF;
  h.bucket = 0x0F&hash_value;
  return h;
}
//---------------------------------------------------------------------------
const char* CjStringRegistry::Register(const char* attr, unsigned long len)
{
  const char* attrib_location = Find(attr, len);
  if (attrib_location)
    return attrib_location;

  SValueLocator hash = HashFunction(attr, len);
  CBucket* target_set = bucket_sets[hash.set];
  if (!target_set)
    target_set = bucket_sets[hash.set] = new CBucket[BUCKET_SET_SIZE];

  attrib_location = static_strings->Exists(attr, len);
  if (!attrib_location)
  { // non-standard attribute (allocate memory for it and free it on desctruction)
    attrib_location = new char[len+1];
    strncpy((char*)attrib_location, attr, len);
    ((char*)attrib_location)[len]=0;
  }

  target_set[hash.bucket].SetKey(hash.signature, attrib_location);
  return attrib_location;
}
//---------------------------------------------------------------------------
long CjStringRegistry::CollisionCount()
{
  long count=0;
  for (long s=BUCKET_SETS-1 ; s >= 0 ; s--)
  {
    CBucket* bucket = bucket_sets[s];
    if (!bucket)
      continue;
    for (long b=BUCKET_SET_SIZE ; b > 0 ; b--, bucket++)
      if (!bucket->IsEmpty() && bucket->HasCollision())
        count += bucket->CollisionCount();
  }
  return count;
}

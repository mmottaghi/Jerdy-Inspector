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

#ifndef HJerdyContainersH
#define HJerdyContainersH

#define EACH_(Typ,v, list) (Typ const *v = list->Begin() ; v!=list->End() ; v++)
#define EACH(Typ,v, list) (Typ const *v = list.Begin() ; v!=list.End() ; v++)

#include<stdlib.h>

template <class T> class CIterator
{
    T* _begin;
    T* _end;
    T* current;
  public:
    CIterator(T* begin, T* end)   {_begin = current = begin; _end = end;}
    CIterator(T* begin, long count) {_begin = current = begin; _end = begin+count;}

    T*    Current()   const {return current;}
    long  CountLeft() const {return ((long)_end - (long)current)/sizeof(T);}
    bool  MoreLeft()  const {return current < _end;}
    void  Next()            {current++;}
    T& operator()()  const {return *current;}
    T& At(long i) const  {return this->operator[](i);}
    T& operator[](long i) const {return  _begin[i];}
};
//------------------------------------------------------------------------------
template<class T, bool auto_delete = true>
class CShVectorRO // Shrinkable vector (Read Only): realloc-based array of pointers to type T
{// T MUST not require custom copy constructors (ATTENTION! no copy constructors are called upon data migration)
  protected:
    T* data;
    T* dataEnd;

  public:
    CShVectorRO()       {data = dataEnd = NULL;}
    ~CShVectorRO()      {if (auto_delete) Free();}
    virtual void Free()       { if (data) {free(data); data = NULL;} dataEnd = NULL; }
    T const * Begin() const {return data;}
    T const * End()   const {return dataEnd;}
    long Count()      const {return ((unsigned long)dataEnd - (unsigned long)data)/sizeof(T);}
    T* At(long i)     const {return data+i;}
    T& operator[](long i) const {return *(data+i);}
    CIterator<T> Iterator() const {return CIterator<T>(data, dataEnd);}
};
//===========================================================================
template<class T, bool auto_delete = true, int initial_capacity=127>
class CShVector : public CShVectorRO<T, auto_delete>  //  shrinkable vector: realloc-based shrinkable vector
{//  T MUST be free of copy constructors
  private:
    long  capacity;

    void Resize(long new_capacity)
    {
      capacity = new_capacity;

      T* new_start = (T*)realloc(data, capacity*sizeof(T));
      if (new_start)
        data = new_start;
      else // if realloc failed
        Free();
    }
    unsigned long CheckCapacity(unsigned long new_items_count)
    {
      unsigned long old_size = Count(), new_size = old_size + new_items_count;
      if (new_size > capacity)
        Resize(1 + capacity + (capacity? capacity:initial_capacity));
      return old_size;
    }

  public:
    CShVector() { capacity=0; }
    void Add(const T& obj)
    {
      long old_len = CheckCapacity(1);
      data[old_len] = obj;
      dataEnd = data + ++old_len;
    }
    void Add(const T* array_in, unsigned long count)
    {
      long old_len = CheckCapacity(count);
      T* copy_dest = data+old_len;
      for (const T* array_cell = array_in, *src_end = array_in+count ; array_cell != src_end ; array_cell++)
          *copy_dest++ = *array_cell;
      dataEnd = data + old_len + count;
    }
    void Shrink()
    {
      if (long size=Count())
        Resize(size);
      else
        Free();
    }
    virtual void Free() { capacity = 0; CShVectorRO<T, auto_delete>::Free(); }
};
//===========================================================================
template<class LengthType, class IndexType>
class CStringLookup // used for fast case-INSENSITIVE string lookups
{
  private:
    LengthType member_lengths[256]; // 0 = nonexistent, positive = str.length (if only one string), negative = number of members which start with the same letter (e.g. 3 for {'age', 'art', 'ape'})
    IndexType  array_start_index[256];
    const char ** members;

    void UpdateHash(long bucket_index, LengthType len, long str_index)
    {
      LengthType& hashCell = member_lengths[bucket_index];
      if (!hashCell)
      {
        hashCell = len;
        array_start_index[bucket_index] = str_index;
      }
      else
      {
        if (hashCell < 0)
          hashCell--;
        else
          hashCell = -2; // two members in this bucket
      }
    }

  public:
    CStringLookup(const char * ordered_list[])
    { // ordered_list must be alphabetically ordered and must end with NULL
      members = ordered_list;
      memset(member_lengths, 0, 256 * sizeof(LengthType));
      long member_index = 0;
      for (const char *member  ; member = *ordered_list, member; ordered_list++, member_index++)
      {
        LengthType memLen = strlen(member);
        UpdateHash(*member, memLen, member_index);
        UpdateHash(*member + (*member>'_'? -32:32), memLen, member_index);
      }
    }
    bool Contains(const char* str, long str_len) const
    {
      if (long memberLen = member_lengths[*str])
      {
        const char *candidate = members[array_start_index[*str]];
        if (memberLen>0) // only a single member with the same starting character exists
          return str_len == memberLen && (strncmp(str, candidate, memberLen)==0 || strncmpi(str, candidate, memberLen)==0);    // strcmp is faster than strcmpi
        else
          for (int mLen, i=array_start_index[*str], lastIndex=i-memberLen ; i < lastIndex ; i++)
            if ((mLen=strlen(members[i]), strncmpi(str, members[i], mLen))==0)
              return true;
      }
      return false;
    }
};
//===========================================================================
template<class T>
class CLinkedLoop   // circular linked list
{
  private:
    long link_count;
    class CLink
    {
      friend class CLinkedLoop;
      private:
        T data;
        CLink * next;
      public:
        CLink(const T &d, CLink * nextNode) {data = d; next = nextNode;}
        CLink * Next() const  {return next;}
        T& Data()             {return data;}
        ~CLink()
        {
          if (next)
          {
            CLink* restOfList = next;
            next = NULL;
            if (restOfList->next)
              delete restOfList;
          }
        }
    }* tail;

  public:
    CLinkedLoop()       { tail = NULL; link_count = 0;}
    ~CLinkedLoop()      { delete tail;  }
    long Length() const { return link_count;}
    CLink* Tail() const { return tail;}
    virtual void Clear(){ delete tail; tail = NULL; link_count = 0; }
    //.......................................................................
    void Add(const T &newData)
    { // tail->next points to the first node
      link_count++;
      if (tail)
        tail = tail->next = new CLink(newData, tail->next);
      else
      {
        tail = new CLink(newData, NULL);
        tail->next = tail;
      }
    }
    //.......................................................................
    void MoveTo(CLinkedLoop &destination_list)
    {
      if (!tail)
        return;
      if (destination_list.tail) // if dest is not empty  ooooo-->ooo
      {
        CLink* destStart = destination_list.tail->next;
        destination_list.tail->next = tail->next;
        tail->next = destStart;
      }
      destination_list.tail = tail; // destination_list is empty
      destination_list.link_count += link_count;
      tail = NULL;
      link_count = 0;
    }
    //.......................................................................
    T* ToArray()
    {
      if (Length() <= 0)
        return NULL;
      T* arr = new T[Length()];
      CLink* link = tail->next;
      for (long i=0 ; i < link_count ; i++, link = link->next)
        arr[i] = link->data;
      return arr;
    }
    //.......................................................................
    T& operator[](long data_index)
    {
      CLink* link = tail->next;
      for (long i=0 ; i < data_index ; i++, link = link->next);
      return link->data;
    }
};    
#endif

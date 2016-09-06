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

		Mohammd Mottaghi Dastjerdi (mamad[a.t]cs.duke.edu)
		Sep. 1, 2016

***********************************************************************************/

#ifndef HtmlElementH
#define HtmlElementH
//---------------------------------------------------------------------------
#include<map>

#include "jStrScan.h"
#include "jStrReg.h"
#include "JerdyContainers.h"
#include "tagID_enum.h"

typedef const char  c_char;
typedef const char* TRegStr;     // registered string (only deleted by the StringRegistry Class)
typedef const char* TStrPos;     // pointer to a position inside another (existing) string (memory doesn't need to be released)
typedef long  TAHandle; // Attribute Handle (used for registered strings in lue of TRegStr to enforce a pointer-value comparison instead of a full string comparison)


struct SNullableString  // its value can be either be NULL, "", or any string longer than 0 chars
{ // this struct is used when the NULL value is accepted and distinguished from empty string (e.g. for HTML Tag Attribute)
  c_char* str; // The empty value for this struct (i.e. "") must be static, hence no delete required). NULL means not initialized

  SNullableString(c_char* s) {str = s;}
  SNullableString() {}
  void  Free()    {if (str && !IsEmpty()) delete[] str;} // The empty value for this struct (i.e. "") must be static, hence no delete required).
  bool  IsEmpty() const {return *str == 0;} // must make sure !IsNull() before calling IsEmpty(), to avoid access violations
  bool  IsNull()  const {return str == NULL;}
  c_char* CStr(c_char* val_if_null="")  const {return IsNull()? val_if_null : str;}
  operator c_char*() const    {return str;}
  void operator=(c_char* nval) {str = nval;}
  bool operator<(const SNullableString& s2) const { return strcmp(str, s2.str)<0; }
};
//==============================================================================
struct SAttribute  // must be deleted explicitly by calling Free()
{
  TRegStr         name;   // must not be deleted by this class (since it's a registered string)
  SNullableString value;

  typedef CIterator<const SAttribute> It; // iterator

  TAHandle  Handle()   const {return (long)name;} // name is a registered string
  c_char*   AsString() const {return value.CStr();}
  void      Free()           {value.Free();}
};
//==============================================================================
class CTagAttributes
{
  private:
    class CAttribAccessor
    {// this class is used as the first element of the 'attribs' array
        friend class CTagAttributes;
        unsigned short count             :14;  // number of attributes
        unsigned short only_has_id_class :1;   // attribs 'id' and 'class'
        unsigned short has_id            :1;   // true if the 'ID' attribute is present
        unsigned short class_index;  // index (starting from 1 and NOT 0) of the element which contains the 'class' value
        c_char* id;     // shortcut to the element which contains the 'id' value

        void  AdjustShortcuts(unsigned long attrib_count, TRegStr ID_handle, TRegStr class_handle);
      public:
        short Count()  const {return count; }
        c_char* ID()   const {return id;}
        c_char* Class(c_char* replace_null) const {return class_index? ((SAttribute*)this)[class_index].value.CStr(replace_null) : NULL;}
        bool  HasOnlyClID() const {return only_has_id_class;}
        bool  HasID()       const {return has_id;}
        bool  HasClass()    const {return class_index;}
        long  ClassIndex()  const {return class_index;}
        const SAttribute* Items() const {return (SAttribute*)this+1;} // returns a pointer to the array of attributes
    };

  protected:
    c_char* tag_name;
    CAttribAccessor* attribs; // 'attribs' is an array of 'SAttribute' in which the first element is of 'CAttribAccessor' type, keeping the length and also shortcuts to ID and ClassName

    static const CCharRange validTagNameChars;
    static const CCharRange validTagAttrChars;
    static const CCharRange validAttrNameChars;

    TRegStr ExtractIDClassHandles(const SAttribute* attribs, unsigned long attr_count, TRegStr &extracted_class_handle/*out*/);

  public:
    CTagAttributes()  { tag_name = NULL; attribs = NULL;}
    void ClearAttributes();
    virtual void Free()       { ClearAttributes(); if (tag_name) { delete []tag_name; tag_name=NULL; } }
    c_char* TagName() const {return tag_name;}
    c_char* ID(c_char* if_null=NULL)    const {return HasID()? attribs->ID()? attribs->ID(): if_null : NULL;}
    c_char* Class(c_char* if_null=NULL) const {return attribs? attribs->Class(if_null) : NULL;}
    c_char* GetAttrib (c_char* attrib_name, c_char* replace_null_value=NULL) const;
    c_char* GetAttrib (TAHandle attrib_handle, c_char* replace_null_value=NULL) const;
    short AttribCount()    const {return attribs? attribs->Count() : 0;}
    bool  HasNoAtribute()  const {return attribs==NULL;}
    bool  HasOnlyClassID() const {return attribs && attribs->HasOnlyClID();}
    bool  HasClass() const {return attribs && attribs->HasClass();}
    bool  HasID()    const {return attribs && attribs->HasID();}
    SAttribute::It   Attribs()    const {return attribs? SAttribute::It(attribs->Items(), (long)attribs->Count()): SAttribute::It(NULL, (long)0);}
    CAttribAccessor* Attributes() const {return attribs;}
    void AssignAttribs(const SAttribute* atr_array, unsigned long atr_count);
    void AssignAttribs(const SAttribute* atr_array, unsigned long atr_count, TRegStr ID_handle, TRegStr class_handle);
    void AppendAttribs(SAttribute* atr_array, unsigned long atr_count);
    void AppendAttribs(SAttribute* atr_array, unsigned long atr_count, TRegStr ID_handle, TRegStr class_handle);
};
//===========================================================================
typedef unsigned long TTagIdentities; // bitwise-or of potentially multiple 'enTagIdentity' values
struct STagMetaData
{
  c_char* name;
  enTagIdentifier id;
  TTagIdentities  identities;
};
//===========================================================================
class CjHtmlElement : public CTagAttributes
{
  friend class CJerdyParser;

  private:
    const STagMetaData* meta;
    TStrPos openning_tag_start;
    TStrPos content_start;
    TStrPos closing_tag_start;
    TStrPos after_tag;
    long children_count;
    CjHtmlElement** children;
    struct {
      CjHtmlElement* parent;
      CjHtmlElement* previous_sibling;
      CjHtmlElement* next_sibling;
      CjHtmlElement* firstChild;
      CjHtmlElement* lastChild;
    } links;
    char content_closing_byte;

    void FreeChildren();
    void SetLinks(CjHtmlElement* par, CjHtmlElement* p_sibling, CjHtmlElement* n_sibling);
    void SetClosingPos(TStrPos clsoing_start, TStrPos after)
    {
      closing_tag_start = clsoing_start;
      after_tag = after;
      if (openning_tag_start == closing_tag_start)
        content_start = NULL;
    }
    void AssignConst(const char*target, char val) const { *(char*)target = val; }

  public:
    void* data; // this is not used by this class, and can be used by other classes to link any relevant data

    typedef CIterator<CjHtmlElement*> It;
    typedef CShVector<CjHtmlElement*, true, 255> TNodeArray;
    typedef bool (*TCallBack)(const CjHtmlElement *matching_node, void *data);

    CjHtmlElement(const STagMetaData* meta_data, TStrPos open_tag_start);
    ~CjHtmlElement(){ Free(); }

    virtual void Free() { if (tag_name==meta->name/*no mem allocated*/) tag_name=NULL; FreeChildren(); CTagAttributes::Free(); }
    TStrPos Begin()   const {return openning_tag_start;}
    TStrPos Content() const {return content_start;}
    TStrPos End()     const {return closing_tag_start;}
    TStrPos AfterEndTag() const {return after_tag;}
    bool  HasChildren()   const {return children_count>0;}
    long  ChildrenCount() const {return children_count;}
    const STagMetaData* Meta()  const {return meta;}
    CjHtmlElement** Children()  const {return children;}

    long Level()       const {return !Parent()? 0: 1+Parent()->Level();}
    long Depth()       const;
    long Descendants() const;

    CjHtmlElement* Child(long child_index) const {return children[child_index];}
    void AssignChildren(const CjHtmlElement* pointers_to_children[], long count);
    CjHtmlElement* PreviousSibling() const {return links.previous_sibling;}
    CjHtmlElement* NextSibling()     const {return links.next_sibling;}
    CjHtmlElement* Parent()          const {return links.parent;}
    CjHtmlElement* FirstChild()      const {return links.firstChild;}
    CjHtmlElement* LastChild()       const {return links.lastChild;}
    CjHtmlElement* First(c_char* target_tag_name, CjHtmlElement* _default=NULL) const;
    CjHtmlElement* Last(c_char* target_tag_name, CjHtmlElement* _default=NULL) const;
    void FindTags(c_char* desired_tag_name, TNodeArray *result_container) const;
    bool FindTags(c_char* desired_tag_name, TCallBack on_match_found, void *data_to_match_processor) const;

    void IsolateContent() const { AssignConst(&content_closing_byte, *closing_tag_start); AssignConst(closing_tag_start, 0); }
    void UndoContentIsolation() const { AssignConst(closing_tag_start, content_closing_byte);}
};
//---------------------------------------------------------------------------
class CjHtmlDocument
{
    typedef void (CjHtmlDocument::*TCallBackDFS)(CjHtmlElement* el, void* dat); // callback for depth-first search

  private:
    CjHtmlElement*    dom_root; // root of the Document Object Model (DOM)
    CjStringRegistry* attribute_registry;
    std::map<SNullableString, CjHtmlElement*>  id_to_node_map; // used for fast ID lookup
    CShVector<CjHtmlElement::TNodeArray*, true, 63> query_results;

    void DFS(CjHtmlElement* node, TCallBackDFS ProcessNode, void* data=NULL); // DFS = depth-first search
    void AddNodeIdToMap(CjHtmlElement* node, void *);
  public:
    bool auto_delete_source_buffer; // when true, the HTML-source buffer (given to CJerdyParser::Parse) will be deleted on destruction

    CjHtmlDocument(CjHtmlElement* tree_root, CjStringRegistry* at_reg) {dom_root=tree_root; attribute_registry=at_reg; auto_delete_source_buffer=false;}
    ~CjHtmlDocument();
    CjHtmlElement* Root()  const {return dom_root;}
    c_char*   SourceHTML() const {return dom_root->Begin();}
    void ClearQueryResults();
    void UpdateIDMap() { id_to_node_map.clear(); DFS(dom_root, &CjHtmlDocument::AddNodeIdToMap); }
    CjHtmlElement* GetElementById(c_char* id) const;
    CjHtmlElement::It GetElementsByTagName(c_char* tag_name, CjHtmlElement* subtree_root = NULL);
};
#endif

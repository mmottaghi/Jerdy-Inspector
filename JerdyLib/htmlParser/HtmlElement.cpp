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
		Sep. 1,2016

***********************************************************************************/

#pragma hdrstop

#include "HtmlElement.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

#define _EachChild(ch)   (CjHtmlElement **ch = children, **last_child = children + children_count ; ch < last_child ; ch++)
#define _EachChild_R(ch) (CjHtmlElement **ch = children + children_count ; --ch >= children ; )
#define _TagNameMatches(tname, elem) (strcmpi(elem->TagName(), tname)==0)

const CCharRange CTagAttributes::validTagNameChars("-?!_a-zA-Z0-9");
const CCharRange CTagAttributes::validTagAttrChars("-_*a-zA-Z0-9");
const CCharRange CTagAttributes::validAttrNameChars("-_a-zA-Z0-9\"'");

c_char* CTagAttributes::GetAttrib(c_char* attrib_name, c_char* replace_null_value) const
{
  for (SAttribute::It attr = Attribs(); attr.MoreLeft() ; attr.Next())
    if (strcmpi(attr.Current()->name, attrib_name)==0)
      return replace_null_value && !attr.Current()->value? replace_null_value : attr.Current()->AsString();
  return NULL;
}
//------------------------------------------------------------------------------
c_char* CTagAttributes::GetAttrib(TAHandle attrib_handle, c_char* replace_null_value) const
{
  for (SAttribute::It attr = Attribs(); attr.MoreLeft() ; attr.Next())
    if (attrib_handle == attr.Current()->Handle()) // 'attr.Current()->name'  is a registered string, therefore no strcmp call is needed
      return replace_null_value && !attr.Current()->value? replace_null_value : attr.Current()->AsString();
  return NULL;
}
//------------------------------------------------------------------------------
void CTagAttributes::ClearAttributes()
{
  if (attribs)
  {
    SAttribute* attr = &((SAttribute*)attribs)[1];
    for (long i=attribs->Count() ; i > 0 ; i--, attr++)
      attr->Free();
    delete [](SAttribute*)attribs;
    attribs = NULL;
  }
}
//------------------------------------------------------------------------------
TRegStr CTagAttributes::ExtractIDClassHandles(const SAttribute* attribs, unsigned long attr_count, TRegStr &extracted_class_handle/*out*/)
{
  TRegStr id_handle = NULL;
  for (extracted_class_handle = NULL ; attr_count > 0 ; attr_count--, attribs++) // find 'id' and 'class' values (if any)
    if (strcmp(attribs->name, "id")==0)
      id_handle = attribs->name;
    else if (strcmp(attribs->name, "class")==0)
      extracted_class_handle = attribs->name;
  return id_handle;
}
//------------------------------------------------------------------------------
void CTagAttributes::AssignAttribs(const SAttribute* atr_array, unsigned long len)
{
  TRegStr handle_of_ID, handle_of_class;
  handle_of_ID = ExtractIDClassHandles(atr_array, len, handle_of_class);
  AssignAttribs(atr_array, len, handle_of_ID, handle_of_class);
}
//------------------------------------------------------------------------------
void CTagAttributes::AssignAttribs(const SAttribute* atr_array, unsigned long len, TRegStr ID_handle, TRegStr class_handle)
{
  ClearAttributes();
  if (len > 0)
  {
    attribs = (CAttribAccessor*)new SAttribute[len+1]; // the first one is reserved for CAttribAccessor
    memcpy((void*)(attribs+1), atr_array, len*sizeof(SAttribute));
    attribs->AdjustShortcuts(len, ID_handle, class_handle);
  }
}
//------------------------------------------------------------------------------
void CTagAttributes::AppendAttribs(SAttribute* atr_array, unsigned long len)
{
  TRegStr handle_of_ID, handle_of_class;
  handle_of_ID = ExtractIDClassHandles(atr_array, len, handle_of_class);
  AppendAttribs(atr_array, len, handle_of_ID, handle_of_class);
}
//------------------------------------------------------------------------------
void CTagAttributes::AppendAttribs(SAttribute* atr_array, unsigned long attr_count, TRegStr ID_handle, TRegStr class_handle)
{
  if (!attribs) // no attribute exists
  {
    AssignAttribs(atr_array, attr_count, ID_handle, class_handle);
    memset((void*)atr_array, 0, attr_count*sizeof(SAttribute)); // NULL all the attrib values to prevent them from being mem-released by the caller function (they are now used by 'this->attribs')
  }
  else
  {
    long new_attributes_count = 0, i=attr_count ;
    for (SAttribute* attr=atr_array ; i > 0 ; i--, attr++) // find 'id' and 'class' values (if any)
      if (GetAttrib(attr->Handle()))
        attr->name = 0; // ignore (already exists);  the allocated memory for attr->value must be released by the caller
      else
        new_attributes_count++;

    if (new_attributes_count)
    {
      long old_attrib_count = attribs->Count();
      SAttribute* merged_attr_buf = new SAttribute[old_attrib_count + new_attributes_count + 1]; // the first one is reserved for CAttribAccessor
      memcpy((void*)(merged_attr_buf+1), (SAttribute*)attribs+1, old_attrib_count*sizeof(SAttribute));
      delete [](SAttribute*)attribs;
      attribs = (CAttribAccessor*)merged_attr_buf;

      SAttribute* new_attr_pos = merged_attr_buf + 1 + old_attrib_count;
      // copy new attributes
      i=attr_count;
      for (SAttribute* attr=atr_array ; i > 0 ; i--, attr++) // find 'id' and 'class' values (if any)
        if (attr->name) // duplicate attribs were marked by 0 (zero)
        {
          *new_attr_pos= *attr;
          attr->value.str = NULL; // this prevents the memory allocted to attr->value from bein released when atr_array is destroyed in the caller function
          new_attr_pos ++;
        }

      attribs->AdjustShortcuts(old_attrib_count + new_attributes_count, ID_handle, class_handle);
    }
  }
}
//==============================================================================
void CTagAttributes::CAttribAccessor::AdjustShortcuts(unsigned long attrib_count, TRegStr ID_handle, TRegStr class_handle)
{ // sufficient memory has been alocated by the 'CTagAttributes' class, before this method is called
  count = attrib_count;
  only_has_id_class = 1; // len > 0 because this method has been called
  class_index = 0; // 0 means no 'class' attribute
  has_id = 0;
  id = NULL;

  long i = --attrib_count; /*adjusting for class index*/
  for (const SAttribute* attr = (SAttribute*)this+1 ; ; i--, attr++) // find 'id' and 'class' values (if any)
  {
    if (attr->name == ID_handle)
    {
      id = attr->value;
      has_id = 1;
    }
    else if (attr->name == class_handle)
      class_index = count - i; // MUST start from 1 and NOT from zero
    else
      only_has_id_class = 0;

    if (!i)
      break;
  }
}
//==============================================================================
CjHtmlElement::CjHtmlElement(const STagMetaData* meta_data, TStrPos open_tag_start)
{
  tag_name = content_start = closing_tag_start = after_tag = NULL;
  meta = meta_data;
  openning_tag_start = open_tag_start;
  data = NULL;
  children = NULL;
  children_count = 0;
}
//------------------------------------------------------------------------------
void CjHtmlElement::FreeChildren()
{
  if (children)
  {
    CjHtmlElement** child_ptr = children;
    for (long i=children_count ; i > 0 ; i--, child_ptr++)
      delete *child_ptr;
    delete []children;
    children = NULL;
  }
}
//------------------------------------------------------------------------------
void CjHtmlElement::AssignChildren(const CjHtmlElement* pointers_to_children[], long count)
{
  FreeChildren();
  if (count <= 0)
    count = 0;
  else
  {
    children = new CjHtmlElement*[count];
    memcpy(children, pointers_to_children, count * sizeof(CjHtmlElement*));
  }
  children_count = count;
}
//------------------------------------------------------------------------------
void CjHtmlElement::SetLinks(CjHtmlElement* par, CjHtmlElement* p_sibling, CjHtmlElement* n_sibling)
{
  links.parent = par;
  links.next_sibling = n_sibling;
  links.previous_sibling = p_sibling;
  links.firstChild = children? *children: NULL;
  links.lastChild = children? children[children_count-1]: NULL;

  for (CjHtmlElement** child = children, **ch_end = children+children_count, *prev=NULL, **next=child+1
        ; child < ch_end ; prev= *child, child=next++)
    (*child)->SetLinks(this, prev, next<ch_end? *next:NULL);
}
//------------------------------------------------------------------------------
long CjHtmlElement::Descendants() const
{
  long sum = 0;
  for _EachChild(c)
    sum += (*c)->Descendants();
  return sum + children_count;
}
//------------------------------------------------------------------------------
long CjHtmlElement::Depth() const
{
  long cheight, max_height = -1;
  for _EachChild(c)
    if (max_height < (cheight = (*c)->Depth()))
      max_height = cheight;
  return max_height+1;
}
//------------------------------------------------------------------------------
CjHtmlElement* CjHtmlElement::First(c_char* target_tag_name, CjHtmlElement* _default) const
{// returns first descendant with the given tag name
  for _EachChild(cp)
  {
    CjHtmlElement *target, *child = *cp;
    if _TagNameMatches(target_tag_name, child)
      return child;
    else if (child->children && (target=child->First(target_tag_name))!=NULL)
      return target;
  }
  return _default; // if found nothing
}
//------------------------------------------------------------------------------
CjHtmlElement* CjHtmlElement::Last(c_char* target_tag_name, CjHtmlElement* _default) const
{// returns last descendant with the given tag name
  for _EachChild_R(rcp) // reverse order: last -> first
  {
    CjHtmlElement *target, *child = *rcp;
    if (child->children && (target=child->Last(target_tag_name))!=NULL)
      return target;
    else if _TagNameMatches(target_tag_name, child)
      return child;
  }
  return _default; // if found nothing
}
//------------------------------------------------------------------------------
void CjHtmlElement::FindTags(c_char* desired_tag_name, TNodeArray *result_container) const
{ // returns all elements with tag name 'desired_tag_name'; e.g. FindTag("div"...) returns all the divs in the DOM tree
  if _TagNameMatches(desired_tag_name, this)
    result_container->Add((CjHtmlElement*)this);
  for _EachChild(cp)
    (*cp)->FindTags(desired_tag_name, result_container);
  return ;
}
//------------------------------------------------------------------------------
bool CjHtmlElement::FindTags(c_char* desired_tag_name, TCallBack on_match_found, void *data_to_match_processor) const
{ // returns false to stop the search
  if _TagNameMatches(desired_tag_name, this)
    if (!on_match_found(this, data_to_match_processor))
      return false;
  for _EachChild(cp)
    if (!(*cp)->FindTags(desired_tag_name, on_match_found, data_to_match_processor))
      return false;
  return true;
}




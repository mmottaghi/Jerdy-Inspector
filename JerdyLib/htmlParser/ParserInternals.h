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

#ifndef ParserInternalsH
#define ParserInternalsH

#include<vector>

//---------------------------------------------------------------------------
struct CJerdyParser::SOpenTag
{
  typedef std::vector<SOpenTag>::iterator It;
  typedef std::vector<SOpenTag>::const_iterator CIt;

  const char* name; // must not be mem-released (because it points to dom_node->tag_name
  enTagIdentifier tag_id;
  SNestingRules   nesting;
  CjHtmlElement*   dom_node; // Document Object Model
  const SjHtmlTag* descriptor;
  std::vector<const CjHtmlElement*>   children;

  SOpenTag(unsigned long provisioned_children_count) { children.reserve(provisioned_children_count); }
  void TentativelyClose(TStrPos closing_tag_start_, TStrPos after_tag_, bool switch_nesting_ruleset);
  inline void Close(TStrPos closing_tag_start_, TStrPos after_tag_)
  {
    dom_node->AssignChildren((const CjHtmlElement**)children.begin(), children.size());
    CJerdyParser::SetElementClosingPos(dom_node, closing_tag_start_, after_tag_);
  }
};
//---------------------------------------------------------------------------
class CJerdyParser::CNestingStack
{
  private:
    SOpenTag::It     top; // points to the last tag on the stack (it should be a valid and currently-open tag)
    vector<SOpenTag> tag_stack;         // stack of open tags

    void IncreaseProvisionedNestingDepth(long increase_amount, long reserved_children_per_element);
  public:
    CNestingStack(CjHtmlElement* root, unsigned long initial_nesting_depth, unsigned long initial_children_per_element);
    void CloseTags(SOpenTag::CIt chain_start, TStrPos start_of_closing_tag, TStrPos after_closing_tag);
    void CloseTagsImplicitly(SOpenTag::CIt chain_start, TStrPos implicit_closing_tag_pos) {CloseTags(chain_start, implicit_closing_tag_pos, implicit_closing_tag_pos);}
    SOpenTag::It  FindTopMostOpenTag(TStrPos tag_name, long name_length) const;
    SOpenTag::It  Top()    const {return top;}
    SOpenTag::CIt Bottom() const {return tag_stack.begin();}
    void AssignParent(const CjHtmlElement* child);
    void Add(CjHtmlElement* new_element, bool use_exceptional_nesting_rules);
    CjHtmlElement* HtmlNode()  const {return top->descriptor->meta.id == tiRoot? NULL : tag_stack[1].dom_node;}
    bool IsHeadNodeAtTop()    const {return top->descriptor->meta.id == tiHead;}
};
//---------------------------------------------------------------------------
#endif

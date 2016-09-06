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

#ifndef JerdyParserH
#define JerdyParserH
//---------------------------------------------------------------------------
#include<ctype>
#include "HtmlElement.h"
//---------------------------------------------------------------------------
#define MAX_ATTR_COUNT 2000  // assuming no HTML tag has more than 2000 attributes (change it according to  your application)
#define DEFAULT_NEST_DEPTH 30
#define DEFAULT_NEST_BREADTH 100
//---------------------------------------------------------------------------

using namespace std;

struct SNestabilityClass;
struct SjHtmlTag
{ // Do NOT reorder the members of this structure
  STagMetaData meta;
  const SNestabilityClass* nestability;

  typedef CIterator<const SjHtmlTag> CIt; // const iterator
};
//---------------------------------------------------------------------------
class CJerdyParser
{
  private:
    struct SAttribBookmark
    { // this struct is used for parsing attrib names and values: no memory is allocated, so the results can be safely discarded (e.g. when tag is ignored by the HtmlParser)
      SStringClip name;   // keeps references to the start of the 'name' in the tag string (does not allocate separate memory to it)
      SStringClip value;
      const SAttribute* ConvertToAttributeArray(long length, CjStringRegistry* attrib_regsitry);
    };
    struct SSingletonNodes // Contains pointers to nodes that appear exactly one time in the DOM (e.g. HTML, BODY, ...)
    { // this struct is used by Parse(...) to pass pointers to AddTagToDomTree(...)
      CjHtmlElement* dummy_element; // used to append new attribs to <html> and <body> in case they reappear in the source HTML
      CjHtmlElement* html_node;
      CjHtmlElement* body_node;
      SSingletonNodes(CjHtmlElement* h, CjHtmlElement* b, CjHtmlElement* d){html_node=h; body_node=b; dummy_element=d;}
    };

    class CNestingStack;
    struct SOpenTag;
    struct Nesting;
    friend class SOpenTag;

    CjStrScan  html_scanner;
    CNestingStack*    nesting_stack;
    CjStringRegistry* attribute_registry;
    TRegStr ID_attr_handle;
    TRegStr class_attr_handle;

    static c_char* EMPTY_STR;
    static c_char* standard_html_tags_serialized;
    static const CjStringBank  standard_html_tag_names;
    static const SjHtmlTag     tag_descriptors[];
    static map<SStringClip, const SjHtmlTag*> tagname_to_descriptor_map; // tag-name to metadata mapping

    void AddTerminalElement(const SjHtmlTag* terminal_tag_type);
    void AddTagToDomTree(bool is_injected_tag, bool tag_injection_allowed, const SjHtmlTag* new_tag, const SSingletonNodes * precreated);
    void ReadAttributes(CjHtmlElement* tag_associated_with_attribs);
    void AddCommentElement();
    void ParseAllTagsOfDocumentHead();
    bool ParseTagForDocumentHead(const SjHtmlTag* new_tag);
    bool CommentTagHasStarted()   const {return html_scanner.Peek("!?");}
    bool ValidTagNameHasStarted() const {return isalpha(html_scanner.Peek());}
    CjHtmlElement* CreateNewHtmlElement(const SjHtmlTag* meta, bool is_injected);
    inline const SjHtmlTag* ScanTagNameAndGetItsDescriptor();
    inline void AddImplicitTagToDOM(enTagIdentifier tag_id, bool ignore_scanned_chars);
    inline void AddNonTerminalElement(const SjHtmlTag* tag_descriptor, bool is_implicit_tag, bool default_rule_set_inapplicable);
    inline void AugmentElementAttribs(CjHtmlElement* revisited_element, CjHtmlElement* dummy_element);
    static void SetElementClosingPos(CjHtmlElement* el, TStrPos closing_tag_pos, TStrPos after_tag) {el->SetClosingPos(closing_tag_pos, after_tag);} // this is solves the accesibility issue with forward subclass declaration as friend in C++

  public:
    static c_char* attribNameStopper;
    static c_char* tagNameStopper;

    CJerdyParser();

    static char*  LoadAllFileText(c_char* file_path, long *file_size_out = NULL);
    static CjStringRegistry*   CreateAttributeRegistry(bool shared = false) {return new CjStringRegistry(&standard_html_tag_names, shared);}
    static SjHtmlTag::CIt HtmlTags();
    CjHtmlDocument* Parse(c_char* html, CjStringRegistry* attr_reg=NULL, long provisioned_nesting_depth=DEFAULT_NEST_DEPTH, long provisioned_nesting_breadth=DEFAULT_NEST_BREADTH);
    CjHtmlDocument* ParseFile(c_char* html_file_path, CjStringRegistry* attr_reg=NULL, long provisioned_nesting_depth=DEFAULT_NEST_DEPTH, long provisioned_nesting_breadth=DEFAULT_NEST_BREADTH);
};
//---------------------------------------------------------------------------
struct SNestingAction
{
  long close   : 1;  // new tag will close the current tag: e.g., <p><li> will be treated as <p></p><li>
  long ignore  : 1;  // new tag will be ignored: e.g., <select><div><option> will be treated as <select><option>
  long block   : 1;  // tags that are rule-checking-blocked by this tag (e.g., <ul> blocks <li>)
  //long contain : 1; (implicit) // new tag will be a child to the current tag: e.g., <ul> <li>(new tag = li)
  long unused  : 29;

  SNestingAction(long cl, long ig, long bl) {close = cl!=0; ignore = ig!=0; block = bl!=0; unused=0;}
  long All() {return *(long*)this;}
};
//---------------------------------------------------------------------------
struct SNestingRules
{ // Do NOT reorder the members
  TTagIdentities  closers;     // tags that close this tag (e.g., <option> and <optgroup> close <option>)
  TTagIdentities  blockees;    // tags that are rule-checking-blocked by this tag (e.g., <ul> blocks <li>)
  TTagIdentities  tunnellers;  // tags that will tunnel through this tag and attach to its parent (e.g., in <div><table><p>, <p> will attach to <div> and not <table>; because <table> is transparent to <p>)
  TTagIdentities  ignorees;    // tags that must be ignored; e.g., <select><div><option> will be treated as <select><option>
  bool leads_tunnellers;     // when true, the tunneller element will be inserted before 'this tag' in the children-list of the parent

  SNestingAction Action(TTagIdentities child_tag_identity) const
  {
    return SNestingAction(closers & child_tag_identity, ignorees & child_tag_identity, blockees & child_tag_identity);
  }
};
//---------------------------------------------------------------------------
struct SNestabilityClass
{  // Do NOT reorder the members
  const SNestingRules*  rule_sets; // nesting rule sets
  enTagIdentifier injector;  // if 'this (tag)' hits the 'injector', another tag (injectee) will be injected before it; e.g. <table>(injector) causes <td> to inject <tr> before itself in this example: <table><td>
  enTagIdentifier injectee;  // e.g. <tr> is injected before <td> in the following context:  <table><tbody><td>  ---will be converted to--->  <table><tbody><tr><td>
  enTagIdentifier exception; // specifies which ancestor tag (if present) causes rule_sets[1] to be used instead of the default rule-set (i.e., rule_sets[0])
};
//---------------------------------------------------------------------------
#endif

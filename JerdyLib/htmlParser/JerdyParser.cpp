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

#include<stdio.h>
#include "JerdyParser.h"
#include "ParserInternals.h"
#include "jStrScan.h"
#include "TagDescriptions.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

c_char* CJerdyParser::EMPTY_STR = "";
c_char* CJerdyParser::attribNameStopper("= >\t\r\n/");
c_char* CJerdyParser::tagNameStopper("> \t\r\n/");
map<SStringClip, const SjHtmlTag*> CJerdyParser::tagname_to_descriptor_map;
const CjStringBank CJerdyParser::standard_html_tag_names(standard_html_tags_serialized, true);

//static string set: each string must be preceded by its length (one byte) and followed by zero
c_char* CJerdyParser::standard_html_tags_serialized = "\4abbr\0\6accept\0\Eaccept-charset\0\9accesskey\0\6action\0\5align\0\
\3alt\0\5async\0\Cautocomplete\0\9autofocus\0\8autoplay\0\8autosave\0\7bgcolor\0\6border\0\8buffered\0\9challenge\0\7charset\0\
\7checked\0\4cite\0\5class\0\4code\0\8codebase\0\5color\0\4cols\0\7colspan\0\7content\0\Fcontenteditable\0\Bcontextmenu\0\8controls\0\
\6coords\0\Bcrossorigin\0\4data\0\8datetime\0\7default\0\5defer\0\3dir\0\7dirname\0\8disabled\0\8download\0\9draggable\0\8dropzone\0\
\7enctype\0\3for\0\4form\0\Aformaction\0\Bformenctype\0\Aformmethod\0\Eformnovalidate\0\Aformtarget\0\7headers\0\6height\0\6hidden\0\
\4high\0\4href\0\8hreflang\0\Ahttp-equiv\0\4icon\0\2id\0\5ismap\0\8itemprop\0\7keytype\0\4kind\0\5label\0\4lang\0\8language\0\4list\0\
\4loop\0\3low\0\8manifest\0\3max\0\9maxlength\0\5media\0\Amediagroup\0\6method\0\3min\0\9minlength\0\8multiple\0\5muted\0\4name\0\
\Anovalidate\0\4open\0\7optimum\0\7pattern\0\4ping\0\Bplaceholder\0\6poster\0\7preload\0\Aradiogroup\0\8readonly\0\3rel\0\8required\0\
\8reversed\0\4rows\0\7rowspan\0\7sandbox\0\Aspellcheck\0\5scope\0\6scoped\0\8seamless\0\8selected\0\5shape\0\4size\0\5sizes\0\4span\0\
\Aspellcheck\0\3src\0\6srcdoc\0\7srclang\0\6srcset\0\5start\0\4step\0\5style\0\7summary\0\8tabindex\0\6target\0\5title\0\9translate\0\
\4type\0\Dtypemustmatch\0\6usemap\0\5value\0\5width\0\4wrap\0\7onabort\0\Conafterprint\0\Donbeforeprint\0\Eonbeforeunload\0\6onblur\0\
\8oncancel\0\9oncanplay\0\10oncanplaythrough\0\8onchange\0\7onclick\0\Boncuechange\0\Aondblclick\0\10ondurationchange\0\9onemptied\0\
\7onended\0\7onerror\0\7onfocus\0\Conhashchange\0\7oninput\0\9oninvalid\0\9onkeydown\0\Aonkeypress\0\7onkeyup\0\6onload\0\Conloadeddata\0\
\10onloadedmetadata\0\Bonloadstart\0\9onmessage\0\Bonmousedown\0\Conmouseenter\0\Conmouseleave\0\Bonmousemove\0\Aonmouseout\0\
\Bonmouseover\0\9onmouseup\0\Conmousewheel\0\9onoffline\0\8ononline\0\Aonpagehide\0\Aonpageshow\0\7onpause\0\6onplay\0\9onplaying\0\
\Aonpopstate\0\Aonprogress\0\Conratechange\0\7onreset\0\8onresize\0\8onscroll\0\8onseeked\0\9onseeking\0\8onselect\0\6onshow\0\
\9onstalled\0\9onstorage\0\8onsubmit\0\9onsuspend\0\Contimeupdate\0\8ontoggle\0\8onunload\0\Eonvolumechange\0\9onwaiting\0";

//..............................................................................
void CjHtmlDocument::ClearQueryResults()
{
  for (CjHtmlElement::TNodeArray* const * ar = query_results.Begin() ; ar != query_results.End() ; ar++)
    delete *ar;
  query_results.Free();
}
//------------------------------------------------------------------------------
CjHtmlDocument::~CjHtmlDocument()
{
  ClearQueryResults();
  if (auto_delete_source_buffer)
    delete []dom_root->Begin(); // delete the memory buffer which containts the HTML source
  delete dom_root;
  if (!attribute_registry->IsShared())
    delete attribute_registry;
}
//------------------------------------------------------------------------------
void CjHtmlDocument::DFS(CjHtmlElement* node, TCallBackDFS ProcessNode, void* data)
{ // Depth-First Search
  (this->*ProcessNode)(node, data);
  for (CjHtmlElement** child = node->Children(), **last_child=child+node->ChildrenCount() ; child < last_child ; child++)
    DFS(*child, ProcessNode, data);
}
//------------------------------------------------------------------------------
void CjHtmlDocument::AddNodeIdToMap(CjHtmlElement* node, void *)
{
  if (c_char* elem_id = node->ID(NULL))
    id_to_node_map[elem_id] = node;
}
//------------------------------------------------------------------------------
CjHtmlElement* CjHtmlDocument::GetElementById(c_char* id) const
{
  map<SNullableString, CjHtmlElement*>::const_iterator id_pos = id_to_node_map.find(SNullableString(id));
  return id_pos==id_to_node_map.end()? NULL : id_pos->second;
}
//---------------------------------------------------------------------------
CjHtmlElement::It CjHtmlDocument::GetElementsByTagName(c_char* tag_name, CjHtmlElement* subtree_root)
{
  CjHtmlElement::TNodeArray *matching_elements = new CjHtmlElement::TNodeArray();
  query_results.Add(matching_elements);

  if (!subtree_root)
    subtree_root = Root(); // DOM root

  if (subtree_root)
    subtree_root->FindTags(tag_name, matching_elements);
  return matching_elements->Iterator();
}
//==============================================================================
void CJerdyParser::SOpenTag::TentativelyClose(TStrPos closing_tag_start_, TStrPos after_tag_, bool switch_nesting_ruleset)
{
  if (switch_nesting_ruleset)
    nesting = descriptor->nestability->rule_sets[1];
  CJerdyParser::SetElementClosingPos(dom_node, closing_tag_start_, after_tag_);
}
//==============================================================================
CJerdyParser::CNestingStack::CNestingStack(CjHtmlElement* root, unsigned long initial_nesting_depth, unsigned long initial_children_per_element)
{
  if (initial_nesting_depth < 10)
    initial_nesting_depth = 10;
  if (initial_children_per_element < 20)
    initial_children_per_element = 20;
  IncreaseProvisionedNestingDepth(initial_nesting_depth, initial_children_per_element);
  top = tag_stack.begin()-1;
  Add(root, false);
}
//---------------------------------------------------------------------------
void CJerdyParser::CNestingStack::Add(CjHtmlElement* new_element, bool use_exceptional_nesting_rules)
{
  if (++top >= tag_stack.end())  /*TODO: test this line in a unit test*/
    IncreaseProvisionedNestingDepth(tag_stack.capacity(), DEFAULT_NEST_BREADTH);

  top->dom_node = new_element;
  top->descriptor = (const SjHtmlTag*)new_element->Meta();
  top->tag_id   = top->descriptor->meta.id;
  top->nesting  = top->descriptor->nestability->rule_sets[use_exceptional_nesting_rules]; // 0 is the default rule set
  top->name = new_element->TagName();
  top->children.clear();
}
//---------------------------------------------------------------------------
void CJerdyParser::CNestingStack::AssignParent(const CjHtmlElement* child)
{
  if (top->nesting.tunnellers & child->Meta()->identities) // tunnel through 'top' and attach the child to the parent of 'top'
  {
    SOpenTag::It ancestor;
    for (ancestor = top-1 ; ancestor->nesting.tunnellers & child->Meta()->identities ; ancestor--);
    vector<const CjHtmlElement*>* par_child_list = &ancestor->children;
    if ((ancestor+1)->nesting.leads_tunnellers)
      par_child_list->insert(par_child_list->end()-1, child); // e.g. <table><div> is treated as <div><table> (<div> is placed before <table>)
    else
      par_child_list->push_back(child);
  }
  else
    top->children.push_back(child);
}
//---------------------------------------------------------------------------
CJerdyParser::SOpenTag::It CJerdyParser::CNestingStack::FindTopMostOpenTag(TStrPos tag_name, long name_length) const
{
  for (SOpenTag::It candidate = top ; candidate >= tag_stack.begin() ; candidate--)
    if (strncmpi(candidate->name, tag_name, name_length)==0)
        return candidate;
  return NULL;
}
//---------------------------------------------------------------------------
void CJerdyParser::CNestingStack::IncreaseProvisionedNestingDepth(long increase_amount, long reserved_children_per_element)
{
  long top_index = top - tag_stack.begin();

  tag_stack.reserve(tag_stack.capacity() + increase_amount);
  tag_stack.insert(tag_stack.end(), increase_amount, SOpenTag(reserved_children_per_element));

  // recalculates local pointers since memory reallocation (by ::insert) renders them invalid
  top = tag_stack.begin() + top_index;
}
//---------------------------------------------------------------------------
void CJerdyParser::CNestingStack::CloseTags(SOpenTag::CIt closing_chain_start, TStrPos start_of_closing_tag, TStrPos after_closing_tag)
{ // chain of tags to be closed (all the tags after 'closing_chain_start' up to the 'top' will be closed)
  for (SOpenTag::It cur_tag = top ; cur_tag > closing_chain_start ; cur_tag--, top--)
    cur_tag->Close(start_of_closing_tag, start_of_closing_tag);
  (top--)->Close(start_of_closing_tag, after_closing_tag);
}
//===========================================================================
const SAttribute* CJerdyParser::SAttribBookmark::ConvertToAttributeArray(long length, CjStringRegistry* attrib_regsitry)
{ // 'this' is assumed to be the start of an array of SAttribBookmark which should be converted (overwritten) to an array of 'SAttribute'
  SAttribBookmark *bookmark_attr = this;
  SAttribute *target_attr = (SAttribute *)this; // output of the function: no memory is allocated
  for ( ; length>0 ; length--, bookmark_attr++, target_attr++)
  {
    target_attr->name = attrib_regsitry->Register(bookmark_attr->name.start, bookmark_attr->name.length);
    c_char* val_st = bookmark_attr->value.start;
    target_attr->value = (val_st == NULL || !*val_st? val_st : bookmark_attr->value.Clone());
  }
  return (SAttribute *)this;
}
//===========================================================================
CJerdyParser::CJerdyParser()
{
  if (tagname_to_descriptor_map.size()==0) // semi thread-safe (not thread-safe, but in case of inexclusive initialization, nothing catastrophic happens :)
  {
    SStringClip key;
    for (const SjHtmlTag* y = tag_descriptors ; y->meta.name ; y++)
      tagname_to_descriptor_map[(key<<y->meta.name, key)] = y;
  }
  attribute_registry = NULL;
}
//---------------------------------------------------------------------------
const SjHtmlTag* CJerdyParser::ScanTagNameAndGetItsDescriptor()
{
  html_scanner.SkipNot(tagNameStopper); // scan tag name
  map<SStringClip, const SjHtmlTag*>::iterator desc = tagname_to_descriptor_map.find(html_scanner.Clip());
  return desc==tagname_to_descriptor_map.end()? &tag_descriptors[tiCustomTag] : desc->second;
}
//---------------------------------------------------------------------------
void CJerdyParser::AddTerminalElement(const SjHtmlTag* tag_descriptor)
{// adds elements that don't take any child-elements; such as <img>, <link>, <meta>, <script>, ...
  CjHtmlElement* element = CreateNewHtmlElement(tag_descriptor, false);

  TStrPos start_of_closing_tag;
  if (element->meta->identities & icVoid) // e.g. <img>, <br>, <input>, ...
  {
    start_of_closing_tag = html_scanner.Cursor()-1;
    element->content_start = NULL;
  }
  else if (element->meta->identities & icPseudoVoid)
  { // before proceeding, look only for the closing tag; e.g. </script>, </style> ...
    start_of_closing_tag = (TStrPos)-1;
    while(html_scanner.JumpTo("</", (bool*)-1).Skip(1).Peek() != 0)
    {
      html_scanner.Skip(1).SkipNot(tagNameStopper); // scan tag name
      if (strncmpi(html_scanner.Str(), element->meta->name, html_scanner.StrLength())==0)
      {
        start_of_closing_tag = html_scanner.Str()-2;
        ReadAttributes(NULL); // ignores all the attributes (if any)
        break;
      }
    }
  }
  element->SetClosingPos(start_of_closing_tag, html_scanner.Cursor());
}
//---------------------------------------------------------------------------
void CJerdyParser::AddNonTerminalElement(const SjHtmlTag* tag_descriptor, bool is_implicit_tag, bool default_rule_set_inapplicable)
{
  nesting_stack->Add(CreateNewHtmlElement(tag_descriptor, is_implicit_tag), default_rule_set_inapplicable);
}
//---------------------------------------------------------------------------
void CJerdyParser::AddImplicitTagToDOM(enTagIdentifier tag_id, bool ignore_scanned_chars)
{
  if (ignore_scanned_chars)
    html_scanner.StartStr(1); // 1 compensates for -1 in the 'CreateNewHtmlElement' method
  nesting_stack->Add(CreateNewHtmlElement(&tag_descriptors[tag_id], true), false);
}
//---------------------------------------------------------------------------
void CJerdyParser::AddCommentElement()
{// this function is ONLY called when the parser has already scanned <  and has seen <? or <! or </
  c_char* start_of_tag = html_scanner.Cursor();
  html_scanner.Skip(1).SkipNot(tagNameStopper); // scan tag name

  CjHtmlElement* comment_element = NULL;
  if (strncmpi(start_of_tag, "!doctype", 8)==0) // <!doctype
  {
    if (nesting_stack->Top()->descriptor->meta.id == tiRoot) // <!doctype> only attaches to the root
    {
      comment_element = CreateNewHtmlElement(&tag_descriptors[tiDocType], true /* treated as an injected tag */); // 'content_start' is set to NULL by the constructor
      html_scanner.JumpTo('>', (bool*)-1);
    }
    else
    {
      html_scanner.JumpTo('>', (bool*)-1).Skip(1); // ignore this tag
      return;
    }
  }
  else
  {
    comment_element = CreateNewHtmlElement(&tag_descriptors[tiComment], true /* treated as an injected tag */); // 'content_start' is set to NULL by the constructor
    if (strncmp(start_of_tag, "!--", 3)==0) // HTML comment  <!--  -->
    {
      html_scanner.Seek(start_of_tag);
      html_scanner.JumpTo("-->", (bool*)-1, 2).Skip(2);
    }
    else
      html_scanner.JumpTo('>', (bool*)-1);
  }
  comment_element->SetClosingPos(html_scanner.Cursor(), html_scanner.Cursor()+1);
  html_scanner.Skip(1);
}
//---------------------------------------------------------------------------
bool CJerdyParser::ParseTagForDocumentHead(const SjHtmlTag* new_tag)
{ // returns true if the current tag is a body-tag (e.g., <div>, <p> and NOT <meta>, <link>, ...
  enTagIdentifier new_tag_id = new_tag->meta.id;
  if (new_tag_id == tiHtml)  // <html>
  {
    if (CjHtmlElement* html_node = nesting_stack->HtmlNode()) // Html-node already is created (may or may not be virtual)
    {
      CjHtmlElement dummy= CjHtmlElement(&tag_descriptors[tiComment].meta, NULL); // used to append new attribs to <html> in case it reappears in the source HTML
      AugmentElementAttribs(html_node, &dummy); // merge any (possibly) new attributes with existing attributes of the <html> node
    }
    else  // an actual (not a virtual one) <html> node starts here
      AddNonTerminalElement(new_tag, false, false);
  }
  else
  {
    if (!nesting_stack->HtmlNode()) // Html-node has not been created yet
      AddImplicitTagToDOM(tiHtml, false);

    if (new_tag_id == tiHead)  // <head>
    {
      if (nesting_stack->IsHeadNodeAtTop())
        ReadAttributes(NULL); // ignore <head> since it has already been seen
      else
        AddNonTerminalElement(new_tag, false, false);
    }
    else
    {
      if (!nesting_stack->IsHeadNodeAtTop()) // <head> not seen yet
        AddImplicitTagToDOM(tiHead, false);

      if (new_tag->meta.identities & icHeadTags) // head tags: <meta>, <link>, <title>, ...
        AddTerminalElement(new_tag);
      else // non-header tags: e.g., <p> <div> <table> <h1> ...
        return true;
    }
  }
  return false;
}
//---------------------------------------------------------------------------
void CJerdyParser::ParseAllTagsOfDocumentHead()
{
  TStrPos scan_undo_pos;
  const SjHtmlTag* encountered_tag = &tag_descriptors[tiNil];
  for (html_scanner.SkipSpaces() ; scan_undo_pos=html_scanner.Cursor(), !html_scanner.IsAtEnd() ; html_scanner.SkipSpaces())
    if (html_scanner.Peek('<')) // potentially an HTML tag follows
    {
      html_scanner.Skip(1); // skips '<'
      if (ValidTagNameHasStarted())
      {
        encountered_tag = ScanTagNameAndGetItsDescriptor(); // scans tag name here
        if (encountered_tag->meta.id == tiBody || ParseTagForDocumentHead(encountered_tag))
          break; // document body has implicitly started: done with document-head parsing
      }
      else if (CommentTagHasStarted()) // <! or <?
        AddCommentElement();
      else if (html_scanner.Peek('/'))
      {
        html_scanner.Skip(1);
        if (ValidTagNameHasStarted())
        {
          c_char* cur_tag_start = html_scanner.Cursor()-1;
          html_scanner.SkipNot(tagNameStopper); // scan tag name

          ReadAttributes(NULL);

          bool quit_document_head = false;
          c_char* sensitive_closing_tags[] = {"/html", "/head", "/body", NULL}; /* all these three tags are 5 chars in length*/
          for (c_char** hot_tag = sensitive_closing_tags ; *hot_tag ; hot_tag++)
            if (strncmpi(cur_tag_start, *hot_tag, 5)==0)
            {
              if ((*hot_tag)[2]=='e') //  i.e.,   /head
                nesting_stack->Top()->TentativelyClose(cur_tag_start - 1/* -1 for '<' */, html_scanner.Cursor(), true); // </head>: tentatively closes <head>
              else
                quit_document_head = true; // <head> must end here
              break;
            }
          if (quit_document_head) // either </body> or </html> has been read
            break;
        }
        else  // treat as comment; e.g., </ ~comment> or </1div> or </ table>
        {
          html_scanner.Skip(-1);
          AddCommentElement();
        }
      }
      else // text has started
        break;
    }
    else // text has started
      break;

  html_scanner.Seek(scan_undo_pos); // undoes last scan; e.g. 'tag name scanning', ...

  bool body_tag_encountered = encountered_tag->meta.id == tiBody;
  if (!nesting_stack->HtmlNode()) // Html-node has not been created yet
    AddImplicitTagToDOM(tiHtml, !body_tag_encountered);
  if (!nesting_stack->IsHeadNodeAtTop()) // <head> not seen yet
    AddImplicitTagToDOM(tiHead, !body_tag_encountered);

  CjHtmlElement* head_node = (nesting_stack->Bottom()+2)->dom_node; // <root><html><head>
  TStrPos head_tentative_close = head_node->closing_tag_start;
  TStrPos head_tentative_after = head_node->after_tag;

  nesting_stack->CloseTagsImplicitly(nesting_stack->Top(), html_scanner.Cursor());

  // correct the closing position of <head> (if it has been closed by </head> and no other header-tag has appeared ever since)
  if (head_tentative_close && (!head_node->HasChildren() || head_node->Child(head_node->ChildrenCount()-1)->after_tag <= head_tentative_close))
    head_node->SetClosingPos(head_tentative_close, head_tentative_after);

  if (body_tag_encountered)
  {
    html_scanner.StartStr(1).Skip(5); // 5 is the length of '<body' (adjustment needed for read-attributes)
    AddNonTerminalElement(encountered_tag, false, false);
  }
  else
    AddImplicitTagToDOM(tiBody, true);
}
//---------------------------------------------------------------------------
void CJerdyParser::AddTagToDomTree(bool is_injected_tag, bool tag_injection_allowed, const SjHtmlTag* new_tag, const SSingletonNodes * precreated)
{
  bool nestability_exception_found = false;
  enTagIdentifier  missing_container_id = tiNil; // the 'tag-id' of the missing container tag; e.g. <tbody> is missing in: <table><tr>  and will be injected automatically
  SOpenTag::CIt    top_most_tag_to_be_closed = NULL;
  TTagIdentities     new_tag_identity = new_tag->meta.identities;
  for (SOpenTag::CIt stack_bottom = nesting_stack->Bottom(), ancestor = nesting_stack->Top() ; ancestor > stack_bottom ; ancestor--)
  { // find out the impact of currently open tags on the nesting of the new tag: Close, Ignore, ...
    enTagIdentifier ancestor_tag_id = ancestor->tag_id;
    if (ancestor_tag_id == new_tag->nestability->injector)
      missing_container_id = new_tag->nestability->injectee;

    if (ancestor_tag_id == new_tag->nestability->exception) /*TODO: test this line in a unit test*/
      nestability_exception_found = true;

    SNestingAction action = ancestor->nesting.Action(new_tag_identity);
    if (action.All()==0) /* means 'no action'; because .All() returns a bitmap in which each bit corresponds to an action */
      continue;
    if (action.ignore)
    {
      if (action.close) // ignore and close: e.g. <select><select>
        nesting_stack->CloseTagsImplicitly(ancestor, html_scanner.Str()-1);

      enTagIdentifier new_tag_id = new_tag->meta.id;
      if (new_tag_id == tiHtml || new_tag_id == tiBody && ancestor_tag_id != tiSelect) // <select> ignores <body>  in <select><body>
        AugmentElementAttribs(new_tag_id==tiHtml? precreated->html_node : precreated->body_node, precreated->dummy_element); // <body> and <html> have already been added to DOM: just append 'new' attributes
      else if (!is_injected_tag) // ignore the new tag
          ReadAttributes(NULL);
      return;
    }
    else if (action.close)
      top_most_tag_to_be_closed = ancestor;

    if (action.block)
      break;
  }

  if (missing_container_id != tiNil && tag_injection_allowed)
  {
    AddTagToDomTree(true,  true,  &tag_descriptors[missing_container_id], precreated); // inject the missing container tag; e.g. in <table><thead><td>, <tr> is injected before <td>
    AddTagToDomTree(is_injected_tag, false, new_tag, precreated); // no injection allowed this time
    return;
  }

  if (top_most_tag_to_be_closed)
    nesting_stack->CloseTagsImplicitly(top_most_tag_to_be_closed, html_scanner.Str()-1); // an implicit closing tag is assumed to exist right before the current tag starts

  if (new_tag_identity & igTerminal) // void or pseudo-void; e.g., <img>, <script>, ...
    AddTerminalElement(new_tag);
  else
    AddNonTerminalElement(new_tag, is_injected_tag, nestability_exception_found);
}
//---------------------------------------------------------------------------
void CJerdyParser::AugmentElementAttribs(CjHtmlElement* revisited_element, CjHtmlElement* dummy_element)
{
  ReadAttributes(dummy_element);
  if (dummy_element->AttribCount() > 0)
  {
    revisited_element->AppendAttribs((SAttribute*) dummy_element->Attributes()->Items(), dummy_element->AttribCount(), ID_attr_handle, class_attr_handle);
    dummy_element->ClearAttributes();
  }
}
//---------------------------------------------------------------------------
CjHtmlElement* CJerdyParser::CreateNewHtmlElement(const SjHtmlTag* tag_data, bool is_injected)
{
  CjHtmlElement* new_element = new CjHtmlElement(&tag_data->meta, -1 + html_scanner.Str()/*tag-name starts here e.g. <div ...>*/);

  if (is_injected)
  {
    new_element->tag_name = tag_data->meta.name;
    new_element->content_start = new_element->openning_tag_start;
  }
  else
  {
    new_element->tag_name = html_scanner.Extract();
    ReadAttributes(new_element);
    new_element->content_start = html_scanner.Cursor();
  }

  nesting_stack->AssignParent(new_element);
  return new_element;
}
//---------------------------------------------------------------------------
void CJerdyParser::ReadAttributes(CjHtmlElement* tag_associated_with_attribs)
{ // this function does not allocate any memory for the read attributes, instead it only creates bookmarks (start and end positions)
  long attrib_count = 0;
  SAttribBookmark attrib_list_buffer[MAX_ATTR_COUNT], *newAttr = attrib_list_buffer;
  for (char nc ; (nc=html_scanner.SkipSpaces().Peek())!=0 ; newAttr++, attrib_count++)  /* assuming MAX_ATTR_COUNT won't be reached */
    if (nc=='>')
    {
      html_scanner.Skip(1);
      break;
    }
    else if (nc=='/')
      html_scanner.Skip(1), --newAttr, --attrib_count;
    else
    {
      newAttr->name = html_scanner.SkipNot(attribNameStopper).Clip();

      if (html_scanner.SkipSpaces().Peek() != '=')
        newAttr->value.Set(NULL, 0); // uninitialized attribute
      else // read attribute value (e.g. id='tag1')
      { // attrib value might be wrapped by " or '
        char stringWrapper = html_scanner.Skip(1).SkipSpaces().Peek();
        if (stringWrapper == '"' || stringWrapper == '\'')
          html_scanner.StartStr(1).Skip(1).JumpTo(stringWrapper, (bool*)-1).ExtractClip(newAttr->value).Skip(1);
        else
          newAttr->value = html_scanner.SkipNot(tagNameStopper).Clip();

        if (newAttr->value.length==0)
          newAttr->value.start = EMPTY_STR;  // empty string
      }
    }

  if (attrib_count && tag_associated_with_attribs)
  {
    attrib_list_buffer->ConvertToAttributeArray(attrib_count, attribute_registry); // 'attrib_list_buffer' will be converted to an array of 'SAttribute': memory will be allocated for attrib values
    tag_associated_with_attribs->AssignAttribs((SAttribute*)attrib_list_buffer, attrib_count, ID_attr_handle, class_attr_handle); // safe to be called even when 'attrib_count' == 0
  }
}
//---------------------------------------------------------------------------
CjHtmlDocument* CJerdyParser::Parse(c_char* html, CjStringRegistry* attr_reg, long provisioned_nesting_depth, long provisioned_nesting_breadth)
{
  html_scanner.Seek(html);

  attribute_registry = attr_reg? attr_reg : CreateAttributeRegistry(false);
  ID_attr_handle = attribute_registry->Register("id", 2);
  class_attr_handle = attribute_registry->Register("class", 5);

  CjHtmlElement* root = new CjHtmlElement(&tag_descriptors[tiRoot].meta, html);
  root->tag_name = tag_descriptors[tiRoot].meta.name;
  CNestingStack open_tags(root, provisioned_nesting_depth, provisioned_nesting_breadth);
  nesting_stack = &open_tags;

  ParseAllTagsOfDocumentHead(); // this method adds <html>, <head> and <body> to the DOM tree

  CjHtmlElement* html_el = open_tags.HtmlNode();
  CjHtmlElement* body_el = (open_tags.Bottom()+2)->dom_node;

  CjHtmlElement dummy= CjHtmlElement(&tag_descriptors[tiComment].meta, NULL); // used to append new attribs to <html> and <body> in case they reappear in the source HTML
  for(SSingletonNodes singletons(html_el, body_el, &dummy) ; !html_scanner.JumpTo('<', (bool*)-1).Skip(1).IsAtEnd() ; )
    if (html_scanner.Peek('/')) // </
    {
      html_scanner.Skip(1);
      if (ValidTagNameHasStarted())
      {
        c_char* closing_tag_start = html_scanner.Cursor() - 2 /* -2 for '</' */;
        html_scanner.SkipNot(tagNameStopper); // scan tag name
        if (SOpenTag::It correpsonding_open_tag = open_tags.FindTopMostOpenTag(html_scanner.Str(), html_scanner.StrLength()))
        {
          ReadAttributes(NULL); // NULL means: ignore all attributes (if any)
          if (correpsonding_open_tag->tag_id == tiBody || correpsonding_open_tag->tag_id == tiHtml)
            correpsonding_open_tag->TentativelyClose(closing_tag_start, html_scanner.Cursor(), false);
          else
            open_tags.CloseTags(correpsonding_open_tag, closing_tag_start, html_scanner.Cursor());
        }
        else // if corresponding open-tag, not found
          ReadAttributes(NULL); // NULL means: ignore all attributes (if any)
      }
      else  // treat as comment; e.g., </ ~comment> or </1div> or </ table>
      {
        html_scanner.Skip(-1);
        AddCommentElement();
      }
    }
    else if (ValidTagNameHasStarted()) // HTML tag: the next char after < MUST be alphabetic
      AddTagToDomTree(false, true, ScanTagNameAndGetItsDescriptor()/* scans tag name */, &singletons);
    else if (CommentTagHasStarted()) // <! or <?
      AddCommentElement();

  nesting_stack = NULL;

  // save closing info of <html> and <body> for later corrections
  TStrPos html_tentative_end   = html_el->closing_tag_start;
  TStrPos body_tentative_end   = body_el->closing_tag_start;
  TStrPos html_tentative_after = html_el->after_tag;
  TStrPos body_tentative_after = body_el->after_tag;

  open_tags.CloseTagsImplicitly(open_tags.Bottom(), html_scanner.Cursor()); // wrap up: close all the remaining open tags

  if (html_tentative_after && html_scanner.Seek(html_tentative_after).SkipSpaces().IsAtEnd()) //  correct the closing position of <html>
  {
    html_el->SetClosingPos(html_tentative_end, html_tentative_after);
    if (!body_tentative_after) // if <body> has not been explicitly closed
      body_el->SetClosingPos(html_tentative_end, html_tentative_end);
  }
  if (body_tentative_after && (html_scanner.Seek(body_tentative_after).SkipSpaces().IsAtEnd() || html_scanner.Cursor()==html_tentative_end))  //  correct the closing position of <body>
    body_el->SetClosingPos(body_tentative_end, body_tentative_after);

  CjHtmlDocument* dom = new CjHtmlDocument(open_tags.Bottom()->dom_node, attribute_registry);
  dom->Root()->SetLinks(NULL, NULL, NULL);
  dom->UpdateIDMap();
  return dom;
}
//---------------------------------------------------------------------------
char* CJerdyParser::LoadAllFileText(c_char* file_path, long *file_size_out)
{
  char* file_content = NULL;
  long file_size = -1;

  FILE * txt_file = fopen(file_path, "rb");
  if (txt_file) // load the entire file content to memory
  {
    fseek(txt_file, 0, 2); // jump to the end of the file
    file_size = ftell(txt_file);
    file_content = new char[file_size+1];

    fseek(txt_file, 0, 0); // jump to the start of the file
    fread(file_content, sizeof(char), file_size, txt_file);
    fclose(txt_file);
    file_content[file_size]=0;
  }

  if (file_size_out)
    *file_size_out = file_size;

  return file_content;
}
//---------------------------------------------------------------------------
CjHtmlDocument* CJerdyParser::ParseFile(c_char* html_file_path, CjStringRegistry* attr_reg, long provisioned_nesting_depth, long provisioned_nesting_breadth)
{
  char* html = LoadAllFileText(html_file_path);
  if (!html)
    return NULL;
  CjHtmlDocument* dom = Parse(html, attr_reg, provisioned_nesting_depth, provisioned_nesting_breadth);
  dom->auto_delete_source_buffer = true;
  return dom;
}
//---------------------------------------------------------------------------
SjHtmlTag::CIt CJerdyParser::HtmlTags()
{
  long number_of_tags = 0;
  for (const SjHtmlTag* y = tag_descriptors ; y->meta.name ; y++)
    number_of_tags++;

  return SjHtmlTag::CIt(tag_descriptors, number_of_tags);
}


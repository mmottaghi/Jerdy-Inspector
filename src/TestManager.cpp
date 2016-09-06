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

#include<stdio.h>
#include<time.h>

#pragma hdrstop

#include "TestManager.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

vector<TRoStr>* STestArgs::GetArray(TRoStr array_name)
{
  for (vector<pair<TRoStr, vector<TRoStr> > >::iterator arr = arrays.begin() ; arr != arrays.end() ; arr++)
    if (strcmpi(arr->first, array_name)==0)
      return &arr->second;
  return NULL;
}
//---------------------------------------------------------------------------
TRoStr  STestArgs::Get(TRoStr arg_name, TRoStr value_if_null)
{
  for (vector<pair<TRoStr, TRoStr> >::iterator arg = non_arrays.begin() ; arg != non_arrays.end() ; arg++)
    if (strcmpi(arg->first, arg_name)==0)
      return arg->second;
  return value_if_null;
}
//---------------------------------------------------------------------------
pair<TRoStr, TRoStr>* STestArgs::FindKeyValue(TRoStr key)
{
  for (vector<pair<TRoStr, TRoStr> >::iterator arg = non_arrays.begin() ; arg != non_arrays.end() ; arg++)
    if (strcmpi(arg->first, key)==0)
      return arg;
  return NULL;
}
//===========================================================================
bool CTestManager::ReadTestArguments(FILE *config_file, const char* test_start_word, STestArgs *args)
{// extracts test options in the form of:  @param-name=arg  e.g., @ignoreComments = true, or @arr = {val1, val2, ...}
  long start_word_len = strlen(test_start_word);
  for (char separator ; !feof(config_file) ;)
  {
    separator = -1;
    fscanf(config_file, "%*[\r\n\t ]%c", &separator); // read the start of the line
    if (separator == test_start_word[0])
    {
      temp_buf[0] = test_start_word[0];
      fread(temp_buf+1, 1, start_word_len, config_file);
      temp_buf[start_word_len]=0;
      if (strncmp(temp_buf, test_start_word, start_word_len) == 0) // test-start found (e.g. '>root') found: start of the expected tree
        return true;
    }
    else //read text parameters; e.g. @param=394
      for(char *arg_name, val_start=0 ; separator == '@' ; val_start=0)
      {
        separator = -1;
        arg_name = args->buffer.End();
        arg_name[0]='@';
        arg_name[1]=0;
        fscanf(config_file, "%[a-zA-Z_0-9]%*[\t ]%c%*[\t ]%c", arg_name+1, &separator, &val_start);
        if (arg_name[1])
        {
          if (separator=='=' && val_start=='{') // array-arguemnt has been given:  @tags = {div, td, p, table, title}
          {
            args->arrays.push_back();
            args->arrays.back().first = arg_name; // array name
            args->buffer.NewString();

            // read array elements
            vector<TRoStr> *array = &args->arrays.back().second;
            for (char next_char=0, *element ; element = args->buffer.End(), next_char != '}' ; args->buffer.NewString())
            {
              element[0]=0;
              fscanf(config_file, "%*[,\t ]%[^\t\r\n ,}]%c", element, &next_char);
              if (element[0])
                array->push_back(element);
              if (args->buffer.Capacity() < 20) // chain is near full
              {
                fscanf(config_file, "%*[^}]}"); // jump to }
                break;
              }
            }
            continue;
          }
          fseek(config_file, -1, 1); // undo reading val_start

          args->non_arrays.push_back();
          args->non_arrays.back().first = arg_name; // parameter name
          args->buffer.NewString();
          if (separator=='=')
          {
            char *arg_value = args->buffer.End();
            arg_value[0]=0;
            fscanf(config_file, "%*[\t ]%c", &separator);
            if (separator == '\'' || separator == '"')
            {
              char wrapped_str_format[100];
              sprintf(wrapped_str_format, "%%[^%c\r\n]%c", separator, separator);
              fscanf(config_file, wrapped_str_format, arg_value);
            }
            else if (!(separator == '#' || separator < '!')) // neither comment nor control char: ignore those
            {
              arg_value[0] = separator;
              fscanf(config_file, "%[-a-zA-Z_0-9+.<>]", arg_value+1);
            }

            args->non_arrays.back().second = arg_value;// argument (param-value)
            args->buffer.NewString();
          }
        }
      }
    fscanf(config_file, "%*[^\r\n]");
  }
  return false;
}
//----------------------------------------------------------------------------
string CTestManager::SerializeDOM(CjHtmlElement* subtree, long level, const char* main_buf_start, bool include_attributes)
{
  char *line=temp_buf;
  if (level)
  {
    *line++ ='\r';
    *line++ ='\n';
  }
  memset(line, '\t', level);
  line += level;
  *(line+1) = 0;

  long child_cnt = subtree->ChildrenCount();
  long start = subtree->Begin() - main_buf_start;
  long end = subtree->End() - main_buf_start;
  long len = !subtree->Content()? 0 : subtree->End() - subtree->Content();
  sprintf(line, "%s (%d, %ld:%ld, %ld)", subtree->TagName(), subtree->AttribCount(), start, end, len);
  level++;

  string s;
  s = temp_buf;
  if (include_attributes && subtree->AttribCount() > 0)
  {
    s.append(" <^");
    for (SAttribute::It atr = subtree->Attribs() ; atr.MoreLeft() ; atr.Next())
    {
      s.append(atr.Current()->name);
      if (!atr.Current()->value.IsNull())
      {
        s.append("=");
        s.append(atr.Current()->value.CStr());
        s.append(" ^");
      }
    }
    s.append(">");
  }

  if (child_cnt)
    for (CjHtmlElement*child = subtree->Child(0) ; child ; child = child->NextSibling())
      s = s + SerializeDOM(child, level, main_buf_start, include_attributes);
  return s;
}
//----------------------------------------------------------------------------
AnsiString CTestManager::RunCustomTest(const char* test_config_file_path, const char* html_file, bool detailed_report)
{
  AnsiString test_result;
  FILE *config_file = fopen(test_config_file_path, "rb");
  if (!config_file)
  {
    test_result = (AnsiString)"Failed to open the test-configuration file: "+test_config_file_path;
    return test_result;
  }

  char *expected_tree = "<No Expected Tree Found !>";
  STestArgs options;
  if (ReadTestArguments(config_file, ">root", &options)) // '>root' found: start of the expected tree
  {
    long expected_start = ftell(config_file)-6;
    long end_of_file_pos = ftell((fseek(config_file, 0, 2), config_file));
    long exp_tree_len = end_of_file_pos - expected_start;
    expected_tree = new char[exp_tree_len+1];
    fseek(config_file, expected_start, 0);
    fread(expected_tree, sizeof(char), exp_tree_len, config_file);
    for (char * right = expected_tree+exp_tree_len-1 ; right > expected_tree ; right--)
      if (*right > 32)
      {
        *++right=0;
        break;
      }
  }
  fclose(config_file);

  struct
  {
    bool ignore_attributes;
  } test_options;
  test_options.ignore_attributes = true;

  if (pair<TRoStr, TRoStr> *kv = options.FindKeyValue("@ignore_attribs"))
  {
    test_options.ignore_attributes = strcmpi(kv->second, "true")==0;
    *(char*)kv->first = '#';
  }

  CjHtmlDocument* dom_of_test = parser.ParseFile(html_file);
  if (dom_of_test)
  {
    string test_tree = SerializeDOM(dom_of_test->Root(), 0, dom_of_test->Root()->Begin(), !test_options.ignore_attributes);
    if (strcmp(test_tree.c_str(), expected_tree)==0)
      test_result = "Passed. ";
    else
    {
      test_result = "Failed ! ";
      if (detailed_report)
        test_result.cat_printf("\r\nGot:\r\n%s\r\nExpected:\r\n%s\r\n", test_tree.c_str(), expected_tree);
    }
    delete dom_of_test;

    for (int i=0, cnt=options.non_arrays.size() ; i < cnt ;i++) // listing parameters
      test_result.cat_printf("%s = %s, ", options.non_arrays[i].first, options.non_arrays[i].second);
  }
  else
    test_result = (AnsiString)"Failed to open HTML file: ";

  if (expected_tree[0] != '<')
    delete []expected_tree;

  test_result.cat_printf("%s", html_file);
  return test_result;
}
//----------------------------------------------------------------------------
char NestingRelation(CjHtmlElement* first, CjHtmlElement* second)
{
  char relation;
  if (first && second)
  {
    if (first->PreviousSibling()==second  && second->NextSibling()==first) // lead; e.g. <table><p>
      relation = 'L';
    else if (first->NextSibling()==second && second->PreviousSibling()==first) // close; e.g. <p><p>)
      relation = 'C';
    else if (first->FirstChild()==second  && second->Parent()==first) // e.g. <ul><li>
      relation = '.';
    else if (first->Parent()==second) // e.g. <div><html>
      relation = 'R';
    else
    {// check if the second node can be found at deeper levels of the firstNode's children
      long distance_to_first = 0;
      for (CjHtmlElement* ancestor=second ; ; distance_to_first++, ancestor=ancestor->Parent())
        if (ancestor==first)
        {
          relation = '0' + distance_to_first;
          break;
        }
        else if (!ancestor)
        {
          relation = '^'; // first and second are (distant) cousins
          break;
        }
    }
  }
  else if (first) // this implies second==NULL
    relation = 'i';  // 'i' ignore; e.g. <select><div>  (div must be ignored) second==NULL
  else
    relation = '-'; // '-' first el missing; e.g. <td><div> (<table> is missing before <td>, so <td> is ignored)
  return relation;
}
//---------------------------------------------------------------------------
char CTestManager::FindNestingForPair(SInMemHTML *inmem, TRoStr parent, TRoStr child, TRoStr prefix1, TRoStr prefix2)
{
  inmem->CreateTagPair(parent, child, prefix1, prefix2);

  CjHtmlDocument* document  = parser.Parse(inmem->html, attrib_registry);
  CjHtmlElement* first_el  = document->GetElementById(inmem->pid);
  CjHtmlElement* second_el = document->GetElementById(inmem->cid);

  char nesting_code = NestingRelation(first_el, second_el);
  delete document;
  return nesting_code;
}
//---------------------------------------------------------------------------
bool CTestManager::ValidateBlocking(TRoStr prefix_for_all, TRoStr parent_prefix, TRoStr blocker, TRoStr blockee, char xo /*expected_outcome*/, bool report_failures_only)
{
  SInMemHTML inmem;
  char real_outcome = '.';
  if (FindNestingForPair(&inmem, blockee, blockee, prefix_for_all, parent_prefix) == 'C')
  {
    inmem.CreateTagPair(blockee, blockee, prefix_for_all, parent_prefix, blocker);

    CjHtmlDocument* document  = parser.Parse(inmem.html, attrib_registry);
    CjHtmlElement* first_el  = document->GetElementById(inmem.pid);
    CjHtmlElement* second_el = document->GetElementById(inmem.cid);

    if (first_el && first_el->Children() && second_el && second_el->Parent() && strcmpi(second_el->TagName(), blockee)==0 && strcmpi(first_el->TagName(), blockee)==0)
    {
      if (second_el->Parent()->Parent()==first_el && strcmpi(second_el->Parent()->TagName(), blocker)==0) // immediate: e.g. <option><p><option>
        real_outcome = 'B';
      else
        for (CjHtmlElement* ancestor = second_el->Parent() ; ancestor ; ancestor = ancestor->Parent())
          if (ancestor == first_el)
            for (CjHtmlElement* middle_tag = second_el->Parent() ; middle_tag && middle_tag != first_el ; middle_tag = middle_tag->Parent())
              if (strcmpi(middle_tag->TagName(), blocker)==0)
                real_outcome = 'b';
    }
  }

  bool passed = xo==real_outcome;

  AnsiString result_msg = passed? "Passed":"Failed";
  if (!passed)
    result_msg.cat_printf("(Expected %c, Got %c)", xo, real_outcome);
  result_msg.cat_printf(": %s", inmem.html);
  if (!report_failures_only || !passed)
    report->Lines->Add(result_msg);

  return passed;
}
//---------------------------------------------------------------------------
bool CTestManager::ValidateNestability(TRoStr prefix_for_all, TRoStr parent_prefix, TRoStr parent, TRoStr child, char xo /*expected_outcome*/, bool report_failures_only)
{ // returns true if successful (validated)
  SInMemHTML inmem;
  char nesting_relation = FindNestingForPair(&inmem, parent, child, prefix_for_all, parent_prefix);

  bool passed = xo==nesting_relation;

  AnsiString result_msg = passed? "Passed":"Failed";
  if (!passed)
    result_msg.cat_printf("(Expected %c, Got %c)", xo, nesting_relation);
  result_msg.cat_printf(": %s", inmem.html);
  if (!report_failures_only || !passed)
    report->Lines->Add(result_msg);

  return passed;
}
//---------------------------------------------------------------------------
void CTestManager::TestTagPairs(FILE *test_descr_file, STestArgs *args, TRoStr valid_outcomes, TAssertFunction Assert)
{
  randomize();
  vector<TRoStr> *child_tags = args->GetArray("@childTags");
  if (!child_tags)
  {
    report->Lines->Add((AnsiString)"ERROR! Missing array: @childTags");
    return;
  }
  unsigned long child_tags_count = child_tags->size();
  if (!child_tags_count)
  {
    report->Lines->Add((AnsiString)"ERROR! @childTags is empty.");
    return;
  }

  struct {
    long run_sets;
    long skipped_sets;
    long run_tests;
    long failures;
    long successes;
  } stats;
  memset(&stats, 0, sizeof(stats)); // set all stats to zero

  CjStringRegistry* attr_reg = parser.CreateAttributeRegistry(true);
  TRoStr global_prefix = args->Get("@globalPrefix", "");
  bool report_fails_only = strcmpi(args->Get("@onlyReportFailures", "false"), "true")==0;

  for (CStringChain<2000> test_buf ; !feof(test_descr_file) ; )
  {
    char after_tagname, *parent_tag = test_buf.End(), *prefix;

    parent_tag[0]=0;
    fscanf(test_descr_file, "%*[ \t\r\n]%[^ \t\r\n]%*[ \t]%c", parent_tag, &after_tagname);
    if (parent_tag[0]==0 || strcmpi(parent_tag, ">stop")==0)
      break;
    if (parent_tag[0]=='#') // commented out
    {
      fscanf(test_descr_file, "%*[ \t\r\n]%*[^ \t\r\n]");
      continue;
    }

    (prefix = test_buf.NewString())[0]=0;
    if (after_tagname=='{')
    {
      fscanf(test_descr_file, "%[^}\r\n]}", prefix);
      test_buf.NewString();
    }
    else // undo 'after_tagname': put the scanned character back
      fseek(test_descr_file, -1, 1);

    char *expected_outcomes = test_buf.NewString();
    expected_outcomes[0] = 0;
    fscanf(test_descr_file, "%*[ \t]%[^ \t\r\n]", expected_outcomes);
    if (strlen(expected_outcomes) != child_tags_count)
    {
      AnsiString err_msg;
      err_msg.sprintf("ERROR! The outcome string must have exactly %ld letters (%s {%s}).", child_tags_count, parent_tag, prefix);
      report->Lines->Add(err_msg);
      stats.skipped_sets++;
      continue;
    }


    stats.run_sets++;
    for (unsigned long t=0 ; t < child_tags_count ; t++)
      if (expected_outcomes[t]!='x')
      {
        stats.run_tests++;
        if (!strchr(valid_outcomes, expected_outcomes[t]))
        {
          SInMemHTML pair;
          pair.CreateTagPair(parent_tag, child_tags->at(t), global_prefix, prefix);
          report->Lines->Add((AnsiString)"ERROR! Unrecognized outcome code: " + expected_outcomes[t] + " given for " + pair.html);
          stats.failures++;
        }
        else if ((this->*Assert)(global_prefix, prefix, parent_tag, child_tags->at(t), expected_outcomes[t], report_fails_only))
          stats.successes++;
        else
          stats.failures++;
      }
    fscanf(test_descr_file, "%*[^\r\n]");
  }
  delete attr_reg;

  report->Lines->Add(">> Results Summary");
  report->Lines->Add((AnsiString)" Run test sets: " + stats.run_sets);
  report->Lines->Add((AnsiString)" Total tests: " + stats.run_tests);
  report->Lines->Add((AnsiString)" Successes: " + stats.successes);
  report->Lines->Add((AnsiString)" Failures: " + stats.failures);
  report->Lines->Add((AnsiString)" Skipped test sets: " + stats.skipped_sets);
}
//---------------------------------------------------------------------------
void CTestManager::RunAutoGeneratedTest(TRoStr test_description_file_path)
{
  AnsiString test_result;
  FILE *test_description_file = fopen(test_description_file_path, "rb");
  if (!test_description_file)
  {
    report->Lines->Add((AnsiString)"ERROR! Failed to open the test description file: "+test_description_file_path);
    return;
  }

  STestArgs options;
  if (!ReadTestArguments(test_description_file, ">start", &options)) // '>start' found: start of the expected tree
    report->Lines->Add((AnsiString)"ERROR! Could not find '>start'.");
  else
  {
    report->Lines->Add((AnsiString)"Synopsis: "+options.Get("@Synopsis", ""));

    TRoStr test_type = options.Get("@testType", "");
    report->Lines->Add((AnsiString)"Test Type = "+test_type+ ", Started @ "+Now());

    long start_time = GetTickCount();
    if (strcmpi(test_type, "nestability")==0)
      TestTagPairs(test_description_file, &options, ".CLiR^123456789-", &CTestManager::ValidateNestability);
    else if (strcmpi(test_type, "blocking")==0)
      TestTagPairs(test_description_file, &options, ".Bb", &CTestManager::ValidateBlocking);
    else
      report->Lines->Add((AnsiString)"ERROR! Unrecognized value (or no value) specified for @testType: "+test_type);
    report->Lines->Add((AnsiString)"Test Finished @ "+Now());
    report->Lines->Add((AnsiString)"Total Latency(ms): "+(GetTickCount()-start_time));
  }
  report->Lines->Add("");
  fclose(test_description_file);
}
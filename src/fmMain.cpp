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

#include <vcl.h>
#include <stdio.h>
#include <io.h>
#pragma hdrstop

#include "fmMain.h"
#include "fmStats.h"
#include "fmAbout.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FZoomableTree"
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------

AnsiString __fastcall GroupNumber(long num)
{
  char grpnum[30], *src;
  AnsiString strnum = num<0? -num:num;
  src = strnum.c_str();

  int l=0,r;
  if (num<0)
    grpnum[l++]='-';
  for ( r=strnum.Length()-1 ; r>=3 ; )
  {
    for (int i=0 ; i < 3 ; i++)
      grpnum[l++] = src[r--];
    grpnum[l++] = ',';
  }
  while (r >= 0)
    grpnum[l++] = src[r--];
  grpnum[l]=0;
  strrev(grpnum);
  return (AnsiString)grpnum;
}
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
  : TForm(Owner)
{
  dom_tree = NULL;
  html_content = NULL;
  test_manager.report = htmlCodeView;

  browse_directory = "sample HTML/";
  ListFilesAsMenuItems("*.htm*", mnuBrowse, BrowseItemClicked);
  ListFilesAsMenuItems("*.tdf", mnuAutoGenTests, RunAutoGenTest);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuExitClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
TTreeNode *TfrmMain::AddTreeNode(TTreeNode * parent, long icon_index, const CjHtmlElement* html_element, const char*caption)
{
  TTreeNode *newNode = DomViewer->Tree->Items->AddChild(parent, caption);
  newNode->ImageIndex = icon_index;
  newNode->SelectedIndex = icon_index;
  newNode->Data = (void*)html_element;
  return newNode;
}
//---------------------------------------------------------------------------
long TfrmMain::IconIndex(enTagIdentifier tag_id)
{
  long index = -1;
  if ((long)tag_id > 6)
    switch(tag_id)
    {
      case tiDiv:     index = 14; break;
      case tiSpan:    index = 16; break;
      case tiA:       index = 19; break;
      case tiTable:   index = 17; break;
      case tiBr:      index = 18; break;
      case tiLink:    index = 8;  break;
      case tiStyle:   index = 9; break;
      case tiScript:  index = 10; break;
      case tiP:       index = 11; break;
      case tiButton:  index = 12; break;
      case tiImg:     index = 13; break;
      case tiUl:      index = 21; break;
      case tiOl:      index = 20; break;
      case tiLi:      index = 22; break;
      default:      index = 7;  break;
    }
  return index < 0? /* identity mapping */(long)tag_id : index;
}
//---------------------------------------------------------------------------
void TfrmMain::LoadHtmlElement(const CjHtmlElement* elem, TTreeNode *parentNode)
{
  char* node_caption = formatting_buffer;
  node_caption += sprintf(node_caption, "`%s ", elem->TagName());
  if (elem->HasID())
    node_caption += sprintf(node_caption, " #%s", elem->ID(""));
  if (elem->HasClass())
    node_caption += sprintf(node_caption, " .%s", elem->Class(""));

  TTreeNode *elemTreeNode= AddTreeNode(parentNode, IconIndex(elem->Meta()->id), elem, formatting_buffer);
  for (long i=0, cnt=elem->ChildrenCount() ; i < cnt ; i++)
    LoadHtmlElement(elem->Child(i), elemTreeNode);
}
//---------------------------------------------------------------------------
void TfrmMain::LoadHtmlTree(const CjHtmlDocument* dom_tree)
{
  DomViewer->Tree->Items->Clear();
  LoadHtmlElement(dom_tree->Root(), NULL);
  DomViewer->Tree->Items->Item[0]->Selected = true;
  DomViewer->Tree->Items->Item[0]->Expand(true);
  DomViewer->Tree->Items->Item[0]->MakeVisible();
  DomViewer->Tree->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormResize(TObject *Sender)
{
  if (Width - DomViewer->Width < 80)
    DomViewer->Width = (3 * Width)/10;
  if (Width - DomViewer->Width - grpAtrribs->Width < 60)
    grpAtrribs->Width = (5*(Width - DomViewer->Width))/10;
  if (htmlCodeView->Height < 70 || Height - htmlCodeView->Height < 70)
    htmlCodeView->Height = (53*Height)/100;
  if (Height - propList->Height < 80)
    htmlCodeView->Height = (53*Height)/100;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ListFilesAsMenuItems(const char* file_filter, TMenuItem * container_menu, void __fastcall (__closure* ClickHandler)(TObject *))
{
  container_menu->Clear();

  void *search_handle;
  WIN32_FIND_DATA found_file;

  search_handle = FindFirstFile((browse_directory+file_filter).c_str(), &found_file);
  if ((long)search_handle != -1)
    do
    {
      if (found_file.cFileName[0]=='.')
        continue;
      TMenuItem *m_item = new TMenuItem(container_menu);
      m_item->Caption = found_file.cFileName;
      m_item->OnClick = ClickHandler;
      container_menu->Add(m_item);
    }
    while(FindNextFileA(search_handle, &found_file));
  FindClose(search_handle);

  container_menu->Enabled = container_menu->Count > 0;
  loaded_browse_item_index = -1;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuOpenClick(TObject *Sender)
{
  if (!OpenDialog->Execute())
    return;
  browse_directory = ExtractFilePath(OpenDialog->FileName);
  ListFilesAsMenuItems("*.htm*", mnuBrowse, BrowseItemClicked);
  ListFilesAsMenuItems("*.tdf", mnuAutoGenTests, RunAutoGenTest);
  OpenHtmlFile(OpenDialog->FileName.c_str());
}
//---------------------------------------------------------------------------
char* __fastcall TfrmMain::PrepNodeProp(const char* prop_value)
{
  if (prop_value)
    sprintf(formatting_buffer, "%d: %.100s", (long)prop_value - (long)html_content, prop_value);
  else
    strcpy(formatting_buffer, "(null)");
  return formatting_buffer;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::DomViewerTreeChange(TObject *Sender,
      TTreeNode *Node)
{
  if (const CjHtmlElement* html_el = (const CjHtmlElement*)Node->Data)
  {
    propList->Cells[1][0] = html_el->TagName();
    propList->Cells[1][1] = html_el->TagName()==html_el->Meta()->name? "Yes":"No";
    propList->Cells[1][2] = PrepNodeProp(html_el->Begin());
    propList->Cells[1][4] = GroupNumber(!html_el->Content()? 0 : (long)html_el->End() - (long)html_el->Content());
    propList->Cells[1][5] = PrepNodeProp(html_el->End());
    propList->Cells[1][6] = PrepNodeProp(html_el->AfterEndTag());
    propList->Cells[1][7] = html_el->ChildrenCount();
    propList->Cells[1][8] = html_el->AttribCount();
    propList->Cells[1][9] = html_el->Level();
    propList->Cells[1][10] = html_el->Depth();

    html_el->IsolateContent();
    propList->Cells[1][3] = PrepNodeProp(html_el->Content());
    html_el->UndoContentIsolation();

    attribList->Strings->Clear();
    for (SAttribute::It attr = html_el->Attribs() ; attr.MoreLeft() ; attr.Next())
    {
      attribList->Strings->Add(attr.Current()->value.CStr("(null)"));
      attribList->Cells[0][attribList->Strings->Count] = attr.Current()->name;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuTreeZoomToolbarClick(TObject *Sender)
{
  DomViewer->Toolbar->Visible = mnuTreeZoomToolbar->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::DomViewerbtnHideButtonsClick(TObject *Sender)
{
  DomViewer->btnHideButtonsClick(Sender);
  mnuTreeZoomToolbar->Checked = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::BrowseItemClicked(TObject *Sender)
{ // opens a file from the browse directory
  for(long i=0, cnt= mnuBrowse->Count ; i < cnt ; i++)
    if (mnuBrowse->Items[i] == Sender)
    {
      loaded_browse_item_index = i;
      OpenFromBrowseSet();
      break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::OpenFromBrowseSet()
{
  if (loaded_browse_item_index >= 0)
    OpenHtmlFile((browse_directory + mnuBrowse->Items[loaded_browse_item_index]->Caption).c_str());
}
//---------------------------------------------------------------------------
void TfrmMain::OpenHtmlFile(const char* new_path)
{
  if (html_content)
  {
    delete []html_content;
    html_content = NULL;
  }

  long flen;
  CJerdyParser parser;
  html_file_path = new_path;
  html_content = parser.LoadAllFileText(html_file_path.c_str(), &flen);
  if (!html_content)
  {
    CreateMessageDialog((AnsiString)"HTML file not found !\r\n"+html_file_path.c_str(), mtError, TMsgDlgButtons()<<mbOK)->ShowModal();
    return ;
  }

  if (flen<=0)
    StatusBar->Panels->Items[1]->Text = "Empty";
  else
    StatusBar->Panels->Items[1]->Text = GroupNumber(flen);

  StatusBar->Panels->Items[9]->Text = "?";
  StatusBar->Panels->Items[7]->Text = "?";
  StatusBar->Panels->Items[5]->Text = "?";
  StatusBar->Panels->Items[3]->Text = "?";
  Application->ProcessMessages();

  Caption = "Jerdy Dom Viewer - ";
  Caption = Caption + html_file_path;
  actViewSourceExecute(NULL);

  if (dom_tree)
  {
    delete dom_tree;
    dom_tree = NULL; // in case of exceptions, it won't be deleted multiple times
  }

  unsigned long start_time  = GetTickCount();
  dom_tree = parser.Parse(html_content);
  StatusBar->Panels->Items[7]->Text = GetTickCount()==start_time? (AnsiString)"< 1":GroupNumber(GetTickCount() - start_time);
  StatusBar->Panels->Items[3]->Text = GroupNumber(dom_tree->Root()->Descendants());

  start_time  = GetTickCount();
  LoadHtmlTree(dom_tree);
  StatusBar->Panels->Items[9]->Text = GroupNumber(GetTickCount() - start_time);
  StatusBar->Panels->Items[5]->Text = dom_tree->Root()->Depth();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuOpenPreviousClick(TObject *Sender)
{
  if (loaded_browse_item_index == 0)
    return;
  if (--loaded_browse_item_index < 0)
    loaded_browse_item_index = 0;
  OpenFromBrowseSet();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuOpenNextClick(TObject *Sender)
{
  if (loaded_browse_item_index >= mnuBrowse->Count-1)
    return;
  if (loaded_browse_item_index < 0)
    loaded_browse_item_index = mnuBrowse->Count-1;
  else if (++loaded_browse_item_index >= mnuBrowse->Count)
    loaded_browse_item_index = mnuBrowse->Count-1;
  OpenFromBrowseSet();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuSerializeClick(TObject *Sender)
{
  if (!dom_tree)
  {
    CreateMessageDialog("The DOM is empty.", mtError, TMsgDlgButtons()<<mbOK)->ShowModal();
    return;
  }

  if (mnuAutoClearMsgBrd->Checked)
    htmlCodeView->Lines->Clear();
  htmlCodeView->Lines->Add(test_manager.SerializeDOM(dom_tree->Root(), 0, dom_tree->Root()->Begin(), !mnuNoAttribsInDOMSerialize->Checked).c_str());
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actClearMsgBoardExecute(TObject *Sender)
{
  htmlCodeView->Lines->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actViewSourceExecute(TObject *Sender)
{
  if (!FileExists(html_file_path))
  {
    CreateMessageDialog((AnsiString)"HTML file not found !\r\n"+html_file_path.c_str(), mtError, TMsgDlgButtons()<<mbOK)->ShowModal();
    return ;
  }
  htmlCodeView->Lines->LoadFromFile(html_file_path);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actRunTestExecute(TObject *Sender)
{
  if (html_file_path.Trim().Length()<=0)
  {
    CreateMessageDialog("No HTML is open !", mtError, TMsgDlgButtons()<<mbOK)->ShowModal();
    return ;
  }

  AnsiString test_file_associated_to_current_html = ChangeFileExt(html_file_path, ".txt");
  if (!FileExists(test_file_associated_to_current_html))
  {
    CreateMessageDialog((AnsiString)"Test-configuration file not found !\r\nMissing file:\r\n"+test_file_associated_to_current_html.c_str(), mtError, TMsgDlgButtons()<<mbOK)->ShowModal();
    return ;
  }
  CJerdyParser parser;
  htmlCodeView->Lines->Add(test_manager.RunCustomTest(test_file_associated_to_current_html.c_str(), html_file_path.c_str(), true));
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actTreeWindow25Execute(TObject *Sender)
{
  DomViewer->Width = (((TAction*)Sender)->Tag * Width)/100;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuWordWrapClick(TObject *Sender)
{
  mnuWordWrap->Enabled = false;
  htmlCodeView->WordWrap = mnuWordWrap->Checked;
  htmlCodeView->ScrollBars = htmlCodeView->WordWrap? ssVertical : ssBoth;
  mnuWordWrap->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actShowInnerHTMLExecute(TObject *Sender)
{
  if (!DomViewer->Tree->Selected)
    return;

  const CjHtmlElement* sel_element = (const CjHtmlElement*)DomViewer->Tree->Selected->Data;
  htmlCodeView->Lines->Clear();
  sel_element->IsolateContent();
  htmlCodeView->Lines->Add(sel_element->Content());
  sel_element->UndoContentIsolation();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actShowOuterHTMLExecute(TObject *Sender)
{
  if (!DomViewer->Tree->Selected)
    return;

  const CjHtmlElement* sel_element = (const CjHtmlElement*)DomViewer->Tree->Selected->Data;
  htmlCodeView->Lines->Clear();

  char after_char, * after = (char*)sel_element->AfterEndTag();
  after_char = *after;
  *after = 0;
  htmlCodeView->Lines->Add(sel_element->Begin());
  *after = after_char;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actListAttribsExecute(TObject *Sender)
{
  if (!DomViewer->Tree->Selected)
    return;

  const CjHtmlElement* sel_element = (const CjHtmlElement*)DomViewer->Tree->Selected->Data;
  if (mnuAutoClearMsgBrd->Checked)
    htmlCodeView->Lines->Clear();

  AnsiString attrib_line;
  for (SAttribute::It attr = sel_element->Attribs() ; attr.MoreLeft() ; attr.Next())
  {
    attrib_line.printf("[%s=%s]", attr.Current()->name, attr.Current()->value.CStr("(null)"));
    htmlCodeView->Lines->Add(attrib_line);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuRunAllTestsClick(TObject *Sender)
{
  if (mnuAutoClearMsgBrd->Checked)
    htmlCodeView->Lines->Clear();

  abort_running_tests = false;
  mnuStopUnitTests->Visible = true;
  mnuRunAllTests->Enabled = false;

  htmlCodeView->Lines->Add(((AnsiString)"Started at ")+Now());
  CJerdyParser parser;
  AnsiString config_file, html_file;
  long tests_found = 0;
  for (long i=0 ; i < mnuBrowse->Count ; i++)
  {
    Application->ProcessMessages();
    html_file = browse_directory + mnuBrowse->Items[i]->Caption;
    config_file = ChangeFileExt(html_file, ".txt");
    if (FileExists(config_file))
    {
      htmlCodeView->Lines->Add(test_manager.RunCustomTest(config_file.c_str(), html_file.c_str(), false));
      tests_found ++;
    }
    if (abort_running_tests)
    {
      htmlCodeView->Lines->Add("  *** Test-Running Aborted ! ");
      break;
    }
  }
  htmlCodeView->Lines->Add(((AnsiString)"Finished at ")+Now() + ", Ran " + tests_found + " tests.");

  mnuStopUnitTests->Visible = false;
  mnuRunAllTests->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuStopUnitTestsClick(TObject *Sender)
{
  abort_running_tests = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuRunAllAGTestsClick(TObject *Sender)
{
  for(long i=0, cnt= mnuAutoGenTests->Count ; i < cnt ; i++)
    RunAutoGenTest(mnuAutoGenTests->Items[i]);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::RunAutoGenTest(TObject *Sender)
{
  test_manager.RunAutoGeneratedTest((browse_directory + ((TMenuItem*) Sender)->Caption).c_str());
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuStatisticsClick(TObject *Sender)
{
  if (!dom_tree)
  {
    CreateMessageDialog("No HTML has been loaded!", mtWarning, TMsgDlgButtons()<<mbOK)->ShowModal();
    return ;
  }

  frmStats->subtree_root = dom_tree->Root();
  frmStats->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actTagStatsExecute(TObject *Sender)
{
  if (!DomViewer->Tree->Selected)
    return;

  frmStats->subtree_root = (const CjHtmlElement*)DomViewer->Tree->Selected->Data;
  frmStats->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuAboutClick(TObject *Sender)
{
  frmAbout->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuTestabilityClick(TObject *Sender)
{
  mnugTests->Visible = mnuTestability->Checked;
}
//---------------------------------------------------------------------------



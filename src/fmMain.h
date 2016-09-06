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

#ifndef fmMainH
#define fmMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <Dialogs.hpp>
#include <ActnList.hpp>
#include "FZoomableTree.h"
#include "TestManager.h"
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
  TStatusBar *StatusBar;
  TMainMenu *MainMenu;
  TMenuItem *mnugFile;
  TMenuItem *mnuExit;
  TMenuItem *mnuOpen;
  TMenuItem *mnugView;
  TMenuItem *mnuTreeZoomToolbar;
  TMenuItem *mnuSepLine1;
  TMenuItem *mnuSepLine2;
  TMenuItem *mnuBrowse;
  TMenuItem *mnuOpenNext;
  TMenuItem *mnuOpenPrevious;
  TMenuItem *mnugTests;
  TMenuItem *mnuSerialize;
  TMenuItem *InnerHTML1;
  TMenuItem *OuterHTML1;
  TMenuItem *mnugDOM;
  TMenuItem *mnuInnerHTML;
  TMenuItem *mnuOuterHTML;
  TMenuItem *ListAttributes1;
  TMenuItem *mnuListAttributes;
  TMenuItem *mnuStopUnitTests;
  TMenuItem *mnuRunAllTests;
  TMenuItem *mnuNoAttribsInDOMSerialize;
  TMenuItem *N3;
  TMenuItem *mnuClearMsgBrd;
  TMenuItem *mnuViewSource;
  TMenuItem *mnuAutoClearMsgBrd;
  TMenuItem *mnuRunTest;
  TMenuItem *mnuWidth15;
  TMenuItem *mnuWidth25;
  TMenuItem *mnuWidth50;
  TMenuItem *mnuWidth70;
  TMenuItem *mnuWordWrap;
  TMenuItem *N4;
  TMenuItem *mnuRunAllAGTests;
  TMenuItem *mnuAutoGenTests;
  TMenuItem *mnuStatistics;
  TMenuItem *ContainedTagFrequencies1;
  TZoomableTree *DomViewer;
  TImageList *TagIcons;
  TSplitter *splitTree;
  TPanel *pnlRightSide;
  TMemo *htmlCodeView;
  TGroupBox *grpProperties;
  TSplitter *splitAttribs;
  TGroupBox *grpAtrribs;
  TValueListEditor *propList;
  TSplitter *splitMsgBoard;
  TValueListEditor *attribList;
  TOpenDialog *OpenDialog;
  TActionList *ActionList;
  TAction *actClearMsgBoard;
  TAction *actViewSource;
  TAction *actRunTest;
  TAction *actTreeWindow25;
  TAction *actTreeWindow50;
  TAction *actTreeWindow70;
  TAction *actTreeWindow15;
  TAction *actShowInnerHTML;
  TAction *actShowOuterHTML;
  TPopupMenu *popMnuDom;
  TAction *actListAttribs;
  TAction *actTagStats;
  TMenuItem *mnugHelp;
  TMenuItem *mnuAbout;
  TMenuItem *N1;
  TMenuItem *mnuTestability;
  TMenuItem *ContainedTagFrequencies2;
  void __fastcall mnuExitClick(TObject *Sender);
  void __fastcall FormResize(TObject *Sender);
  void __fastcall mnuOpenClick(TObject *Sender);
  void __fastcall DomViewerTreeChange(TObject *Sender, TTreeNode *Node);
  void __fastcall mnuTreeZoomToolbarClick(TObject *Sender);
  void __fastcall DomViewerbtnHideButtonsClick(TObject *Sender);
  void __fastcall BrowseItemClicked(TObject *Sender);
  void __fastcall OpenFromBrowseSet();
  void __fastcall mnuOpenNextClick(TObject *Sender);
  void __fastcall mnuOpenPreviousClick(TObject *Sender);
  void __fastcall mnuSerializeClick(TObject *Sender);
  void __fastcall actClearMsgBoardExecute(TObject *Sender);
  void __fastcall actViewSourceExecute(TObject *Sender);
  void __fastcall actRunTestExecute(TObject *Sender);
  void __fastcall actTreeWindow25Execute(TObject *Sender);
  void __fastcall actShowInnerHTMLExecute(TObject *Sender);
  void __fastcall mnuWordWrapClick(TObject *Sender);
  void __fastcall actShowOuterHTMLExecute(TObject *Sender);
  void __fastcall actListAttribsExecute(TObject *Sender);
  void __fastcall mnuRunAllTestsClick(TObject *Sender);
  void __fastcall mnuStopUnitTestsClick(TObject *Sender);
  void __fastcall mnuRunAllAGTestsClick(TObject *Sender);
  void __fastcall mnuStatisticsClick(TObject *Sender);
  void __fastcall actTagStatsExecute(TObject *Sender);
  void __fastcall mnuAboutClick(TObject *Sender);
  void __fastcall mnuTestabilityClick(TObject *Sender);

private:	// User declarations
  long loaded_browse_item_index;
  char formatting_buffer[2000];
  char* html_content;
  bool abort_running_tests;
  CjHtmlDocument* dom_tree;
  CTestManager test_manager;
  AnsiString browse_directory;
  AnsiString html_file_path;

  TTreeNode *AddTreeNode(TTreeNode * parent, long icon_index, const CjHtmlElement* html_element, const char *caption);
  long IconIndex(enTagIdentifier tag_id);
  void LoadHtmlElement(const CjHtmlElement* elem, TTreeNode *parentNode);
  void LoadHtmlTree(const CjHtmlDocument* dom_tree);
  void OpenHtmlFile(const char* file_path);
  void __fastcall ListFilesAsMenuItems(const char* file_filter, TMenuItem * container_menu, void __fastcall (__closure* ClickHandler)(TObject *));
  char* __fastcall PrepNodeProp(const char* prop_value);
  void __fastcall RunAutoGenTest(TObject *Sender);

public:		// User declarations
  __fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif

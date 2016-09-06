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

#include <vcl.h>
#pragma hdrstop

#include "fmStats.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmStats *frmStats;
//---------------------------------------------------------------------------
__fastcall TfrmStats::TfrmStats(TComponent* Owner)
  : TForm(Owner), total_html_tags(CJerdyParser::HtmlTags().CountLeft()), tag_descriptors(CJerdyParser::HtmlTags().Current())
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmStats::CountTags(const CjHtmlElement* node)
{
  tag_frequency[node->Meta()->id]++;
  for (long i=0, cnt = node->ChildrenCount() ; i < cnt ; i++)
    CountTags(node->Child(i));
}
//---------------------------------------------------------------------------
void __fastcall TfrmStats::FormShow(TObject *Sender)
{
  AnsiString title;
  title.cat_printf("Frequency Of Tags Under '%s' At Level %ld", subtree_root->TagName(), subtree_root->Level());
  Caption = title;
  freqChart->Title->Text->Clear();
  freqChart->Title->Text->Add(title);
  Series1->Clear();
  
  tag_frequency = new long[total_html_tags+1];
  memset(tag_frequency, 0, (total_html_tags+1) * sizeof(long));
  CountTags(subtree_root);

  for (long index_max, max, i=0 ; i < total_html_tags ; i++)
  {
    index_max=tiNil;
    max=-1;
    for (long j=0 ; j < total_html_tags ; j++)
      if (max < tag_frequency[j])
      {
        max = tag_frequency[j];
        index_max = j;
      }

    if (index_max > tiRoot && tag_frequency[index_max]>0)
      Series1->Add(tag_frequency[index_max], tag_descriptors[index_max].meta.name);
    tag_frequency[index_max] = -1;
  }
  delete []tag_frequency;
}
//---------------------------------------------------------------------------
void __fastcall TfrmStats::FormKeyPress(TObject *Sender, char &Key)
{
  if (Key==27)
    Close();
}
//---------------------------------------------------------------------------

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

#ifndef fmAboutH
#define fmAboutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TfrmAbout : public TForm
{
__published:	// IDE-managed Components
  TImage *Image1;
  TStaticText *StaticText2;
  TStaticText *StaticText1;
  void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
public:		// User declarations
  __fastcall TfrmAbout(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmAbout *frmAbout;
//---------------------------------------------------------------------------
#endif

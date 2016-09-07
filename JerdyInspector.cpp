//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("src\FZoomableTree.cpp", ZoomableTree); /* TFrame: File Type */
USEFORM("src\fmAbout.cpp", frmAbout);
USEFORM("src\fmStats.cpp", frmStats);
USEFORM("src\fmMain.cpp", frmMain);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  try
  {
     Application->Initialize();
     Application->CreateForm(__classid(TfrmMain), &frmMain);
     Application->CreateForm(__classid(TfrmAbout), &frmAbout);
     Application->CreateForm(__classid(TfrmStats), &frmStats);
     Application->Run();
  }
  catch (Exception &exception)
  {
     Application->ShowException(&exception);
  }
  catch (...)
  {
     try
     {
       throw Exception("");
     }
     catch (Exception &exception)
     {
       Application->ShowException(&exception);
     }
  }
  return 0;
}
//---------------------------------------------------------------------------

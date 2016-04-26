#ifndef CERRORPROC_H
#define CERRORPROC_H
#include "Afx.h"
#include "Structure.h"
#include "State.h"
using namespace std;
void EmitError(string szErr);// emit error message
void WriteProcErr(vector<ErrorInfo> ErrorProcess,ofstream &fout);
void PrintError(); // output error message to the file recording the the error in the compiling process
void EmitError(string szErr,CToken token);//emit syntax  error message

#endif // CERRORPROC_H

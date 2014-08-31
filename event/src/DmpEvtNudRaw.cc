/*
 *  $Id: DmpEvtNudRaw.cc, 2014-08-20 17:49:42 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#include <iostream>

#include "DmpEvtNudRaw.h"
#include "DmpEFeeFlags.h"

//-------------------------------------------------------------------
DmpEvtNudRaw::DmpEvtNudRaw(){
}

//-------------------------------------------------------------------
DmpEvtNudRaw::~DmpEvtNudRaw(){
}

//-------------------------------------------------------------------
void DmpEvtNudRaw::Reset(){
  fGlobalID.clear();
  fADC.clear();
}

//-------------------------------------------------------------------
void DmpEvtNudRaw::AppendSignal(const short &b,const short &v){
// *
// *  TODO: 
// *
  short n= fGlobalID.size();
  for(size_t i=0;i<n;++i){
    if(b == fGlobalID[i]){
      std::cout<<"Error: exist this block id"<<std::endl;
      return;
    }
  }
  fGlobalID.push_back(b);
  fADC.push_back(v);
}

//-------------------------------------------------------------------
void DmpEvtNudRaw::GenerateStatus(){
//-------------------------------------------------------------------
  short lastTrigger = fFeeNavig.Trigger;
  if(-1 != lastTrigger && -1 != fFeeNavig.Trigger){
    short t = lastTrigger-fFeeNavig.Trigger;
    if(t != -1 || t != 0xff){ // TODO: check ??
      //fIsGood = false;        // trigger skip, but event not wrong
// *
// *  TODO: cout warning
// *
      //DmpLogError<<" trigger not continuous:\t"<<t<<DmpLogEndl;
    }
  }
}

//-------------------------------------------------------------------
short DmpEvtNudRaw::GetSignal(const short &b)const{
  short n= fGlobalID.size();
  for(size_t i=0;i<n;++i){
    if(b == fGlobalID[i]){
      return fADC[i];
    }
  }
  return -999;
}


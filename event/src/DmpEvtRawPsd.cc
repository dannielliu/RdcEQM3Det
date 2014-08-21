/*
 *  $Id: DmpEvtRawPsd.cc, 2014-08-20 17:49:42 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#include <iostream>

#include "DmpEvtRawPsd.h"
#include "DmpEFeeFlags.h"

//-------------------------------------------------------------------
DmpEvtRawPsd::DmpEvtRawPsd(){
}

//-------------------------------------------------------------------
DmpEvtRawPsd::~DmpEvtRawPsd(){
}

//-------------------------------------------------------------------
void DmpEvtRawPsd::Reset(){
  fGlobalID.clear();
  fADC.clear();
}

//-------------------------------------------------------------------
void DmpEvtRawPsd::AppendSignal(const short &gid,const short &v){
  short n= fGlobalID.size();
  for(size_t i=0;i<n;++i){
    if(gid == fGlobalID[i]){
      std::cout<<"Error: exist this gid"<<std::endl;
      return;
    }
  }
  fGlobalID.push_back(gid);
  fADC.push_back(v);
}

//-------------------------------------------------------------------
void DmpEvtRawPsd::GenerateStatus(){
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
short DmpEvtRawPsd::GetSignal(const short &gid)const{
  short n= fGlobalID.size();
  for(size_t i=0;i<n;++i){
    if(gid == fGlobalID[i]){
      return fADC[i];
    }
  }
  return -999;
}


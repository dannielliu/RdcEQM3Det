/*
 *  $Id: DmpEvtRawBgo.cc, 2014-08-20 19:36:43 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#include <iostream>

#include "DmpEvtRawBgo.h"
#include "DmpEFeeFlags.h"

//-------------------------------------------------------------------
DmpEvtRawBgo::DmpEvtRawBgo()
 :fTrigger(-1),fRunMode(-1),fIsGood(true)
{
}

//-------------------------------------------------------------------
DmpEvtRawBgo::~DmpEvtRawBgo(){
}

//-------------------------------------------------------------------
void DmpEvtRawBgo::Reset(){
  fFeeNavig.clear();
  fGlobalID.clear();
  fADC.clear();
  fIsGood = true;
}

//-------------------------------------------------------------------
void DmpEvtRawBgo::AppendSignal(const short &gid,const short &v){
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
void DmpEvtRawBgo::GenerateStatus(){
  for(size_t i=0;i<fFeeNavig.size();++i){
    if(false ==  fFeeNavig[i].CRCFlag){
      fIsGood = false;
      break;
    }
    if(DmpETriggerFlag::kCheckWrong == fFeeNavig[i].TriggerFlag){
      fIsGood = false;
      break;
    }
    if(DmpEPackageFlag::kGood != fFeeNavig[i].PackageFlag){
      fIsGood = false;
      break;
    }
  }
//-------------------------------------------------------------------
  fTrigger = fFeeNavig[0].Trigger;
  for(short i=1;i<fFeeNavig.size();++i){
    if(fTrigger != fFeeNavig[i].Trigger){
      fTrigger = -1;
      fIsGood = false;
    }
  }
//-------------------------------------------------------------------
  short lastTrigger = fTrigger;
  if(-1 != lastTrigger && -1 != fTrigger){
    short t = lastTrigger-fTrigger;
    if(t != -1 || t != 0xff){ // TODO: check ??
      //fIsGood = false;        // trigger skip, but event not wrong
// *
// *  TODO: cout warning
// *
      //DmpLogError<<" trigger not continuous:\t"<<t<<DmpLogEndl;
    }
  }
//-------------------------------------------------------------------
  fRunMode = fFeeNavig[0].RunMode;
  for(size_t i=1;i<fFeeNavig.size();++i){
    if(fRunMode != fFeeNavig[i].RunMode){
      fRunMode = -1;
      //fIsGood = false;  // yes! Some good event run mode not match, too
    }
  }
}

//-------------------------------------------------------------------
short DmpEvtRawBgo::GetSignal(const short &gid)const{
  short n= fGlobalID.size();
  for(size_t i=0;i<n;++i){
    if(gid == fGlobalID[i]){
      return fADC[i];
    }
  }
  return -999;
}


/*
 *  $Id: DmpRdcAlgNud.cc, 2014-08-20 15:18:00 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
 *    Yifeng WEI (weiyf@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpEvtRawNud.h"
#include "DmpDataBuffer.h"
#include "DmpRdcAlgEQM.h"
#include "DmpParameterNud.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpRdcAlgEQM::InitializeNud(){
  return true;
}

//-------------------------------------------------------------------
bool DmpRdcAlgEQM::ProcessThisEventNud(const long &id){
  if(fNudBuf.find(id) == fNudBuf.end()){
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<") not find "<<id<<std::endl;
    return false;
  }
  //fEvtNud->Reset();
  return true;
}



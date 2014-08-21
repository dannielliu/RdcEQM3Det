/*
 *  $Id: DmpRdcAlgNud.cc, 2014-08-20 15:18:00 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
 *    Yifeng WEI (weiyf@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpEvtRawNud.h"
#include "DmpDataBuffer.h"
#include "DmpRdcAlgEQM.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpRdcAlgEQM::InitializeNud(){
  return true;
}

//-------------------------------------------------------------------
bool DmpRdcAlgEQM::ProcessThisEventNud(const long &id){
  if(fNudBuf.find(id) == fNudBuf.end()){
    return false;
  }
  //fEvtNud->Reset();
  fNudBuf.erase(fNudBuf.begin());
  return true;
}



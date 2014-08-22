/*
 *  $Id: DmpRdcAlgPsd.cc, 2014-08-20 15:18:00 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
 *    Yifeng WEI (weiyf@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpEvtRawPsd.h"
#include "DmpDataBuffer.h"
#include "DmpRdcAlgEQM.h"
#include "DmpParameterPsd.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpRdcAlgEQM::InitializePsd(){
  return true;
}

//-------------------------------------------------------------------
bool DmpRdcAlgEQM::ProcessThisEventPsd(const long &id){
  if(fPsdBuf.find(id) == fPsdBuf.end()){
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<") not find "<<id<<std::endl;
    return false;
  }
  //fEvtPsd->Reset();
  return true;
}



/*
 *  $Id: DmpRdcAlgBgo.cc, 2014-08-20 21:20:24 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
 *    Yifeng WEI (weiyf@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpEvtRawBgo.h"
#include "DmpDataBuffer.h"
#include "DmpRdcAlgEQM.h"
#include "DmpBgoBase.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpRdcAlgEQM::InitializeBgo(){
  if(fCNCTPathBgo == "NO"){
    DmpLogInfo<<"No set connector:\tBgo"<<DmpLogEndl;
    return true;
  }else{
    DmpLogInfo<<"Setting connector:\tBgo"<<DmpLogEndl;
  }
  // setup connector
  short feeID=0, channelNo=0, channelID=0, layerID=0, barID=0, sideID=0, dyID=0;
  boost::filesystem::directory_iterator end_iter;
  for(boost::filesystem::directory_iterator iter(fCNCTPathBgo);iter!=end_iter;++iter){
    if(iter->path().extension() != ".cnct") continue;
    ifstream cnctFile(iter->path().string().c_str());
    if (not cnctFile.good()){
      DmpLogError<<"\t\treading cnct file ("<<iter->path().string()<<") failed"<<DmpLogEndl;
      cnctFile.close();
      return false;
    }else{
      std::cout<<"\t\treading cnct file: "<<iter->path().string();
    }
    cnctFile>>feeID;
    cnctFile>>channelNo;
    for(short s=0;s<channelNo;++s){
      cnctFile>>channelID;
      cnctFile>>layerID;
      cnctFile>>barID;
      cnctFile>>sideID;
      cnctFile>>dyID;
      fMapBgo.insert(std::make_pair(feeID*1000+channelID,DmpBgoBase::ConstructGlobalDynodeID(layerID,barID,sideID,dyID)));
    }
    cnctFile.close();
    std::cout<<" Done. ID = "<<feeID<<"\tN_channel = "<<channelNo<<std::endl;
  }
  //-------------------------------------------------------------------
  fEvtBgo = new DmpEvtRawBgo();
  if(not gDataBuffer->RegisterObject("Event/Rdc/Bgo",fEvtBgo,"DmpEvtRawBgo")){
    return false;
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpRdcAlgEQM::ProcessThisEventBgo(const long &id){
  if(fBgoBuf.find(id) == fBgoBuf.end()){
    return false;
  }
  fEvtBgo->Reset();
  short nFee = fBgoBuf[0].size();
  for(size_t i=0;i<nFee;++i){
    fEvtBgo->SetFeeNavigator(fBgoBuf[0][i]->Navigator);
    short nChannel = 0;
    if(DmpERunMode::kCompress == fBgoBuf[0][i]->Navigator.RunMode){
      nChannel = fBgoBuf[0][i]->Signal.size();
    }else if(DmpERunMode::k0Compress == fBgoBuf[0][i]->Navigator.RunMode || DmpERunMode::kCalDAC == fBgoBuf[0][i]->Navigator.RunMode){
    }
  }
  //fEvtBgo->GenerateStatus();
  return true;
}



/*
 *  $Id: ReadDataIntoDataBuffer.cc, 2014-08-21 17:09:15 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 08/08/2014
*/

#include "DmpRdcAlgEQM.h"
#include "DmpEDetectorID.h"
#include "DmpCore.h"

bool DmpRdcAlgEQM::ReadDataIntoDataBuffer(){
  static short s_LastPkgID = -1, s_LastFeeTrg = -1, s_CurrentFeeTrg = 0;
  long currentEventID = gCore->GetCurrentEventID();
  unsigned int scientificHeader = 0;         // 4 bytes 0xe225 0813
  fFile.read((char*)(&scientificHeader),4);
  while(0xe2250813 != htobe32(scientificHeader)){
    fFile.seekg((int)fFile.tellg()-3,std::ios::beg);
    fFile.read((char*)(&scientificHeader),4);
  }
  int endOfLastHeader = fFile.tellg();
  _HeaderNavig newEvt;
  unsigned short packetID = 0;
  fFile.read((char*)(&packetID),2);
  packetID = htobe16(packetID) & 0x3fff;    // only bits 0~13
  if((s_LastPkgID != -1) && (((packetID-1)&s_LastPkgID) != s_LastPkgID)){
    DmpLogWarning<<"Scientific data package count not continuous...\tLast/Current: "<<s_LastPkgID<<"/"<<packetID<<DmpLogEndl;
  }
  s_LastPkgID = packetID;
//std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::hex<<packetID<<std::dec<<std::endl;
  fFile.read((char*)(&newEvt.DataLength),2);
  fFile.read(newEvt.Time,8);
  //-------------------------------------------------------------------
  {
  std::cout<<"  Time:";
  for(size_t i=2;i<8;++i){
    std::cout<<std::hex<<"  "<<(short)(unsigned char)newEvt.Time[i];
  }
  std::cout<<std::dec<<std::endl;
  }
  //-------------------------------------------------------------------
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::endl;
  fHeaderBuf.insert(std::make_pair(currentEventID,newEvt));
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::endl;
  {
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<newEvt.DataLength<<"\t"<<fHeaderBuf.end()->second.DataLength<<std::endl;
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<currentEventID<<"\t"<<fHeaderBuf.end()->first<<"\t"<<fHeaderBuf.size()<<std::endl;
  for(size_t i=2;i<8;++i){
    std::cout<<std::hex<<"  "<<(short)(unsigned char)fHeaderBuf.end()->second.Time[i];
  }
  std::cout<<std::dec<<std::endl;
  }
  //DmpLogDebug<<std::dec<<"\tpackageID = "<<packetID<<"\t length = "<<htobe16(newEvt.DataLength)<<DmpLogEndl;
  //-------------------------------------------------------------------
  if(CheckE2250813DataLength(htobe16(newEvt.DataLength))){    // will find next 0xe2250813 as expected
    for(short i=0;i<fTotalFeeNo;++i){
      unsigned short feeHeader = 0;
      fFile.read((char*)(&feeHeader),2);
      if(0xeb90 == htobe16(feeHeader)){
        char pkgFlag;
        fFile.read(&pkgFlag,1);
        char runMode_feeID;
        fFile.read(&runMode_feeID,1);   // bit 7,6: mode;   bit 5,4: detector id;   bit 3~0: fee id
        short runMode = runMode_feeID>>6;
        short detectorID = (runMode_feeID>>4)&0x03;
        short feeID = runMode_feeID&0x000f;   // &0000 1111
        unsigned short dataLength;        // 2 bytes
        fFile.read((char*)(&dataLength),2);
        dataLength= htobe16(dataLength);
        fFile.seekg((int)fFile.tellg()-2,std::ios::beg);
        if(CheckEb90DataLength(dataLength)){
          char data[dataLength];  // NOTE: include 2 bytes of data length
          fFile.read(data,dataLength);
          _FeeData newFee(data,dataLength,feeID,runMode,-1,-1,pkgFlag,true);
          if(i==0){
            s_CurrentFeeTrg = newFee.Navigator.Trigger;
            if((s_LastFeeTrg != -1) && ((s_CurrentFeeTrg&(s_LastFeeTrg+1)) != s_CurrentFeeTrg)){
              DmpLogWarning<<"Fee trigger not continuous..."<<DmpLogEndl;
            }
          }else{
            if(newFee.Navigator.Trigger == s_CurrentFeeTrg){    // trigger match
              if(newFee.Navigator.CRCFlag){   // CRC right
                if(DmpEDetectorID::kBgo == detectorID){
                  DmpLogInfo<<" Fee type [Bgo] "<<std::hex<<feeID<<" mode "<<runMode<<std::dec<<DmpLogEndl;
                  fBgoBuf[currentEventID].push_back(newFee);
                }else if(DmpEDetectorID::kPsd == detectorID){
                  DmpLogInfo<<" Fee type [Psd] "<<std::hex<<feeID<<" mode "<<runMode<<std::dec<<DmpLogEndl;
                  fPsdBuf[currentEventID].push_back(newFee);
                }else if(DmpEDetectorID::kNud == detectorID){
                  DmpLogInfo<<" Fee type [Nud] "<<std::hex<<feeID<<" mode "<<runMode<<std::dec<<DmpLogEndl;
                  fNudBuf[currentEventID] = newFee;
                }else if(DmpEDetectorID::kStk == detectorID){
                  DmpLogInfo<<" Fee type [Stk] "<<std::hex<<feeID<<" mode "<<runMode<<std::dec<<DmpLogEndl;
                  // *
                  // *  TODO: 
                  // *
                }else{
                  Exception(endOfLastHeader,"Fee type error");
                  return false;
                }
              }else{
                Exception(endOfLastHeader,"CRC error");
                return false;
              }
            }else{
              Exception(endOfLastHeader,"Trigger not match");
              return false;
            }
          }
        }else{
          Exception(endOfLastHeader,"Data length error [0xeb90]");
          return false;
        }
//-------------------------------------------------------------------
      }else{
        Exception(endOfLastHeader,"Not find 0xeb90");
        return false;
      }
    }
    //fOneEventReady = true;
  }else{
    Exception(endOfLastHeader,"Data length error [0xe2250813]");
    return false;
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpRdcAlgEQM::CheckE2250813DataLength(const int &n){
  int skipPoint = fFile.tellg();
  fFile.seekg(skipPoint+n+1-8,std::ios::beg);   // time: 8 bytes. need 1
  unsigned int scientificHeader = 0;            // 4 bytes 0xe225 0813
  fFile.read((char*)(&scientificHeader),4);
  if(0xe2250813 != htobe32(scientificHeader)){
    return false;
  }
  fFile.seekg(skipPoint,std::ios::beg);
  return true;
}

//-------------------------------------------------------------------
bool DmpRdcAlgEQM::CheckEb90DataLength(const int &n){
  int skipPoint = fFile.tellg();
  fFile.seekg(skipPoint+n,std::ios::beg);   // NOTE:  n include 2 bytes which is data length
  unsigned short header = 0;         // 4 bytes 0xe225 0813
  fFile.read((char*)(&header),2);
  fFile.seekg(skipPoint,std::ios::beg);
  if(0xeb90 != htobe16(header) && 0xe225 != htobe16(header)){   // the last Fee will read 0xe225
    return false;
  }
  return true;
}

//-------------------------------------------------------------------
void DmpRdcAlgEQM::Exception(const int &endOfLastE2250813,const std::string &e){
  DmpLogError<<e; PrintTime();
  fFile.seekg(endOfLastE2250813,std::ios::beg);
  unsigned int scientificHeader = 0;         // 4 bytes 0xe225 0813
  fFile.read((char*)(&scientificHeader),4);
  while(0xe2250813 != htobe32(scientificHeader)){
    fFile.seekg((int)fFile.tellg()-3,std::ios::beg);
    fFile.read((char*)(&scientificHeader),4);
  }
  int nBytes = (int)fFile.tellg() - endOfLastE2250813;
  char *errorData = new char[nBytes];
  fFile.seekg(endOfLastE2250813-4,std::ios::beg);
  fFile.read(errorData,nBytes);
  fOutError.write(errorData,nBytes);
  delete[] errorData;
}


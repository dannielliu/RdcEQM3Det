/*
 *  $Id: ReadDataIntoDataBuffer.cc, 2014-08-17 21:10:58 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 08/08/2014
*/

#include "DmpRdcAlgEQM.h"
#include "DmpEDetectorID.h"
#include "DmpCore.h"

bool DmpRdcAlgEQM::ReadDataIntoDataBuffer(){
  if(fFile.eof()){
    DmpLogInfo<<"Reach the end of "<<fInDataName<<DmpLogEndl;
    return false;
  }
  unsigned int scientificHeader = 0;         // 4 bytes 0xe225 0813
  fFile.read((char*)(&scientificHeader),4);
  while(0xe2250813 != htobe32(scientificHeader)){
    fFile.seekg((int)fFile.tellg()-3,std::ios::beg);
    fFile.read((char*)(&scientificHeader),4);
  }
  ++fCurrentPkgID;
  if(fCurrentPkgID == fMaxPkgNo){
    gCore->TerminateRun();
  }
  int endOfLastHeader = fFile.tellg();
  _HeaderNavig newEvt;
  fFile.read(newEvt.PacketID,2);
//std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::hex<<(short)newEvt.PacketID[1]<<std::dec<<std::endl;
  fFile.read((char*)(&newEvt.DataLength),2);
  fFile.read(newEvt.Time,8);
  //-------------------------------------------------------------------
  std::cout<<"  Time:";
  for(size_t i=2;i<8;++i){
    std::cout<<std::hex<<"  "<<(short)(unsigned char)newEvt.Time[i];
  }
  std::cout<<std::dec<<std::endl;
  //-------------------------------------------------------------------
  fHeaderBuf.insert(std::make_pair((short)(unsigned char)newEvt.PacketID[1],newEvt));
  //DmpLogDebug<<std::dec<<"\tpackageID = "<<(short)(unsigned char)newEvt.PacketID[1]<<"\t"<<fCurrentPkgID<<"\t length = "<<htobe16(newEvt.DataLength)<<DmpLogEndl;
  //-------------------------------------------------------------------
  if(CheckE2250813DataLength(htobe16(newEvt.DataLength))){    // will find next 0xe2250813 as expected
    for(short i=0;i<fTotalFeeNo;++i){
      unsigned short feeHeader = 0;
      fFile.read((char*)(&feeHeader),2);
      if(0xeb90 == htobe16(feeHeader)){
        char feeTrigger;
        fFile.read(&feeTrigger,1);
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::hex<<(short)(unsigned char)feeTrigger<<std::dec<<std::endl;
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
//std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::endl;
          _FeeData newFee(data,dataLength,feeID,runMode,(short)(unsigned char)feeTrigger);
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::endl;
          if(newFee.Navigator.Trigger == (short)(unsigned char)newEvt.PacketID[1]){ // TODO: right?
            if(newFee.Navigator.CRCFlag){   // CRC right
              if(DmpEDetectorID::kBgo == detectorID){
                DmpLogInfo<<" Fee type [Bgo] "<<std::hex<<feeID<<" mode "<<runMode<<std::dec<<DmpLogEndl;
                fBgoBuf[newFee.Navigator.Trigger].push_back(newFee);
              }else if(DmpEDetectorID::kPsd == detectorID){
                DmpLogInfo<<" Fee type [Psd] "<<std::hex<<feeID<<" mode "<<runMode<<std::dec<<DmpLogEndl;
                fPsdBuf[newFee.Navigator.Trigger] = newFee;
              }else if(DmpEDetectorID::kNud == detectorID){
                DmpLogInfo<<" Fee type [Nud] "<<std::hex<<feeID<<" mode "<<runMode<<std::dec<<DmpLogEndl;
                fPsdBuf[newFee.Navigator.Trigger] = newFee;
              }else if(DmpEDetectorID::kStk == detectorID){
                DmpLogInfo<<" Fee type [Stk] "<<std::hex<<feeID<<" mode "<<runMode<<std::dec<<DmpLogEndl;
                // *
                // *  TODO: 
                // *
              }else{
                DmpLogWarning<<" Fee type error.. "<<(short)(runMode_feeID&0x30)<<DmpLogEndl;
              }
            }else{
              Exception(endOfLastHeader,"CRC error");
              return true;
            }
          }else{
            Exception(endOfLastHeader,"Trigger not match");
            return true;
          }
        }else{
          Exception(endOfLastHeader,"Data length error [0xeb90]");
          return true;
        }
//-------------------------------------------------------------------
      }else{
        Exception(endOfLastHeader,"Not find 0xeb90");
        return true;
      }
    }
    fOneEventReady = true;
  }else{
    Exception(endOfLastHeader,"Data length error [0xe2250813]");
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
  DmpLogWarning<<e; PrintTime();
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


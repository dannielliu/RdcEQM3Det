/*
 *  $Id: ReadDataIntoDataBuffer.cc, 2014-08-21 17:09:15 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 08/08/2014
*/

#include "DmpRdcAlgEQM.h"
#include "DmpEDetectorID.h"
#include "DmpCore.h"
#include "DmpParameterBgo.h"
#include "DmpParameterPsd.h"
#include "DmpParameterNud.h"

bool DmpRdcAlgEQM::ReadDataIntoDataBuffer(){
  static short s_LastPkgID = -1, s_LastFeeTrg = -1, s_CurrentFeeTrg = 0;
  static short s_TotalFeeNo = DmpParameterBgo::kFeeNo+DmpParameterNud::kFeeNo+DmpParameterPsd::kFeeNo;
  unsigned int scientificHeader = 0;         // 4 bytes 0xe225 0813
  fFile.read((char*)(&scientificHeader),4);
  while(0xe2250813 != htobe32(scientificHeader)){
    fFile.seekg((int)fFile.tellg()-3,std::ios::beg);
    fFile.read((char*)(&scientificHeader),4);
  }
  int endOfLastHeader = fFile.tellg();
  unsigned short packetID = 0;
  fFile.read((char*)(&packetID),2);
  packetID = htobe16(packetID) & 0x3fff;    // only bits 0~13
  if((s_LastPkgID != -1) && (((packetID-1)&s_LastPkgID) != s_LastPkgID)){
    DmpLogWarning<<"Scientific data package count not continuous...\tLast/Current: "<<s_LastPkgID<<"/"<<packetID<<DmpLogEndl;
  }
  s_LastPkgID = packetID;
//std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::hex<<packetID<<std::dec<<std::endl;
  unsigned short dataLength = 0;
  fFile.read((char*)(&dataLength),2);
  dataLength = htobe16(dataLength);
  char time[8];
  fFile.read(time,8);
  _HeaderNavig *newEvt = new _HeaderNavig(dataLength,&time[2]);
  fHeaderBuf.insert(std::make_pair(fGoodRawEventID,newEvt));
  if(CheckE2250813DataLength(dataLength)){    // will find next 0xe2250813 as expected
    for(short i=0;i<s_TotalFeeNo;++i){
      unsigned short feeHeader = 0;
      fFile.read((char*)(&feeHeader),2);
      if(0xeb90 == htobe16(feeHeader)){
        fFile.seekg((int)fFile.tellg()+2,std::ios::beg);    // NOTE: skip 1 byte pkgFlag, 1 byte runMode_feeID
        fFile.read((char*)(&dataLength),2);
        dataLength= htobe16(dataLength);
        if(CheckEb90DataLength(dataLength)){
          fFile.seekg((int)fFile.tellg()-4,std::ios::beg);  // 4 = 2 bytes data length + skipped 2 bytes
          char data[dataLength];    // NOTE: include 1 pkgFlag, include 1 byte runMode_feeID, include 2 bytes of data length, include all scientific data, exclude 2 bytes of CRC
          fFile.read(data,dataLength);
          unsigned short crc = 0;
          fFile.read((char*)(&crc),2);
          crc= htobe16(crc);
          _FeeData *newFee = new _FeeData(data,dataLength,crc);
          //DmpLogInfo<<"Fee ID 0x"<<std::hex<<newFee->Navigator.FeeID<<", Mode "<<newFee->Navigator.RunMode<<std::dec<<DmpLogEndl;
          if(i==0){ // trigger check
            s_CurrentFeeTrg = newFee->Navigator.Trigger;
            if((s_LastFeeTrg != -1) && ((s_CurrentFeeTrg&(s_LastFeeTrg+1)) != s_CurrentFeeTrg)){    // trigger continuous
              DmpLogWarning<<"Fee trigger not continuous..."<<DmpLogEndl;
            }
            s_LastFeeTrg = s_CurrentFeeTrg;
          }else{
            if(newFee->Navigator.Trigger != s_CurrentFeeTrg){    // trigger match
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\t"<<fEventInBuf.size()<<std::endl;
              Exception(endOfLastHeader,"Trigger not match");
              return false;
            }
          }
          short detectorID = (newFee->Navigator.FeeID>>4)&0x03;
          if(newFee->Navigator.CRCFlag){   // CRC check
            if(DmpEDetectorID::kBgo == detectorID){
              fBgoBuf[fGoodRawEventID].push_back(newFee);
            }else if(DmpEDetectorID::kPsd == detectorID){
              fPsdBuf[fGoodRawEventID].push_back(newFee);
            }else if(DmpEDetectorID::kNud == detectorID){
              fNudBuf[fGoodRawEventID] = newFee;
            }else if(DmpEDetectorID::kStk == detectorID){
              // *
              // *  TODO: 
              // *
            }else{
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\t"<<fEventInBuf.size()<<std::endl;
              Exception(endOfLastHeader,"Fee type error");
              return false;
            }
          }else{
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\t"<<fEventInBuf.size()<<std::endl;
            Exception(endOfLastHeader,"CRC error");
            return false;
          }
        }else{
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\t"<<fEventInBuf.size()<<std::endl;
          Exception(endOfLastHeader,"Data length error [0xeb90]");
          return false;
        }
      }else{
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\t"<<fEventInBuf.size()<<std::endl;
        Exception(endOfLastHeader,"Not find 0xeb90");
        return false;
      }
    }
  }else{
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\t"<<fEventInBuf.size()<<std::endl;
    Exception(endOfLastHeader,"Data length error [0xe2250813]");
    return false;
  }
  // check buffer, has current event? TODO: for Stk?
  fEventInBuf.push_back(fGoodRawEventID);
  ++fGoodRawEventID;
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
  fFile.seekg(skipPoint+n-2,std::ios::beg);   // NOTE:  n include 2 bytes which is data length
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
  EraseBuffer(fGoodRawEventID);
}



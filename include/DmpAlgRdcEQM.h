/*
 *  $Id: DmpAlgRdcEQM.h, 2014-08-31 22:46:15 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 27/05/2014
*/

#ifndef DmpAlgRdcEQM_H
#define DmpAlgRdcEQM_H

#include <boost/filesystem.hpp>     // path
#include <fstream>

#include "DmpFeeNavig.h"
#include "DmpVAlg.h"

//-------------------------------------------------------------------
struct _HeaderNavig{
  _HeaderNavig():DataLength(0xffff){
    for(size_t i=0;i<6;++i){
      Time[i] = 0x00;
    }
  }
  _HeaderNavig(const unsigned short &dl,char *t){
    DataLength = dl;
    for(size_t i=0;i<6;++i){
      Time[i] = t[i];
    }
  }
  unsigned short     DataLength;
  char      Time[6];
};

//-------------------------------------------------------------------
struct _FeeData{
  _FeeData(){}
  _FeeData(const _FeeData &r);
  _FeeData(char *data,const short &bytes,const unsigned short &crc);
  ~_FeeData(){}
  DmpFeeNavig  Navigator;
  std::vector<char> Signal;
};

//-------------------------------------------------------------------
class DmpEvtBgoRaw;
class DmpEvtPsdRaw;
class DmpEvtNudRaw;
class DmpEvtHeader;

class DmpAlgRdcEQM : public DmpVAlg{
/*
 *  DmpAlgRdcEQM
 *
 */
public:
  DmpAlgRdcEQM();
  ~DmpAlgRdcEQM();

  void Set(const std::string&,const std::string&);
  bool Initialize();
  bool ProcessThisEvent();
  bool Finalize();

public:
  std::string GetInputFileName()const{return fInDataName.filename().string();}
  std::string GetInputPath()const{return fInDataName.stem().string();}

private:    // for all
  bool ReadDataIntoDataBuffer();    // read one e2250813
  std::vector<long> fEventInBuf;    // Event ID: Event in Buffer
  /* 
   *    1. return true:
   *        1.1     read one 0xe2250813 into data buffer
   *        1.2     throw one 0xe2250813 into Exception()
   *    2. retrun false:
   *        reach the end of input file stream
   */
  bool CheckE2250813DataLength(const int &nBytes);
  bool CheckEb90DataLength(const int &nBytes);
  void Exception(const int &b,const std::string &e);     // throw whole data of e2250813 into fOutError
  void EraseBuffer(const long &id);

  boost::filesystem::path   fInDataName;    // input data name
  long              fGoodRawEventID;    // good event id, find from raw data(one e2250813 or many e2250813)
  std::ifstream     fFile;          // in data stream
  std::ofstream     fOutError;      // save error datas into Error_fInDataName
  std::map<long,_HeaderNavig*>      fHeaderBuf;
  std::map<long,std::vector<_FeeData*> >    fBgoBuf;
  std::map<long,std::vector<_FeeData*> >    fPsdBuf;
  std::map<long,_FeeData*>      fNudBuf;

private:
  DmpEvtHeader      *fEvtHeader;    // save me
  bool ProcessThisEventHeader(const long &id);    // convert event header
  void PrintTime()const;

private:    // Bgo
  std::string       fCNCTPathBgo;   // connector path
  std::map<short,short> fMapBgo;    // map of Bgo connector

  DmpEvtBgoRaw      *fEvtBgo;       // Bgo outdata
  bool InitializeBgo();
  bool ProcessThisEventBgo(const long &id);

private:    // Psd
  std::string       fCNCTPathPsd;   // connector path
  std::map<short,short> fMapPsd;    // map of Psd connector

  DmpEvtPsdRaw      *fEvtPsd;       // Psd outdata
  bool InitializePsd();
  bool ProcessThisEventPsd(const long &id);

private:    // Nud
  std::string       fCNCTPathNud;   // connector path
  std::map<short,short> fMapNud;    // map of Nud connector

  DmpEvtNudRaw      *fEvtNud;       // Nud outdata
  bool InitializeNud();
  bool ProcessThisEventNud(const long &id);
};

#endif



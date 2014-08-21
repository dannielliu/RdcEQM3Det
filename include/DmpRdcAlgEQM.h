/*
 *  $Id: DmpRdcAlgEQM.h, 2014-08-17 20:53:59 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 27/05/2014
*/

#ifndef DmpRdcAlgEQM_H
#define DmpRdcAlgEQM_H

#include <boost/filesystem.hpp>     // path
#include <fstream>

#include "DmpFeeNavig.h"
#include "DmpVAlg.h"

//-------------------------------------------------------------------
struct _HeaderNavig{
  _HeaderNavig():DataLength(0xffff){
    for(size_t i=0;i<8;++i){
      Time[i] = 0x00;
    }
  }
  _HeaderNavig(const _HeaderNavig &r){
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::endl;
    DataLength = r.DataLength;
    for(size_t i=0;i<8;++i){
      Time[i] = r.Time[i];
    }
  }
  unsigned short     DataLength;
  char      Time[8];
};

//-------------------------------------------------------------------
struct _FeeData{
  _FeeData(){}
  _FeeData(const _FeeData &r);
  _FeeData(char *data,const short &bytes,const short &feeID,const short &runMode,const short &trigger,const short &trgFlag,const char &pkgFlag,const bool &crc);
  ~_FeeData(){}
  DmpFeeNavig  Navigator;
  std::vector<char> Signal;
};

//-------------------------------------------------------------------
class DmpEvtRawBgo;
class DmpEvtRawPsd;
class DmpEvtRawNud;
class DmpEvtHeader;

class DmpRdcAlgEQM : public DmpVAlg{
/*
 *  DmpRdcAlgEQM
 *
 */
public:
  DmpRdcAlgEQM();
  ~DmpRdcAlgEQM();

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

  boost::filesystem::path   fInDataName;    // input data name
  short             fTotalFeeNo;    //
  std::ifstream     fFile;          // in data stream
  std::ofstream     fOutError;      // save error datas into Error_fInDataName
  std::map<long,_HeaderNavig>  fHeaderBuf;
  std::map<long,std::vector<_FeeData> >    fBgoBuf;
  std::map<long,std::vector<_FeeData> >    fPsdBuf;
  std::map<long,_FeeData>      fNudBuf;

private:
  DmpEvtHeader      *fEvtHeader;    // save me
  bool ProcessThisEventHeader(const long &id);    // convert event header
  void PrintTime()const;

private:    // Bgo
  std::string       fCNCTPathBgo;   // connector path
  std::map<short,short> fMapBgo;    // map of Bgo connector

  DmpEvtRawBgo      *fEvtBgo;       // Bgo outdata
  bool InitializeBgo();
  bool ProcessThisEventBgo(const long &id);

private:    // Psd
  std::string       fCNCTPathPsd;   // connector path
  std::map<short,short> fMapPsd;    // map of Psd connector

  DmpEvtRawPsd      *fEvtPsd;       // Psd outdata
  bool InitializePsd();
  bool ProcessThisEventPsd(const long &id);

private:    // Nud
  std::string       fCNCTPathNud;   // connector path
  std::map<short,short> fMapNud;    // map of Nud connector

  DmpEvtRawNud      *fEvtNud;       // Nud outdata
  bool InitializeNud();
  bool ProcessThisEventNud(const long &id);
};

#endif



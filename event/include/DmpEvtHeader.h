/*
 *  $Id: DmpEvtHeader.h, 2014-08-08 21:59:44 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 13/12/2013
*/

#ifndef DmpEvtHeader_H
#define DmpEvtHeader_H

#include "TObject.h"

class DmpEvtHeader : public TObject{
/*
 * DmpEvtHeader
 *
*/
public:
  DmpEvtHeader(){Reset();}
  ~DmpEvtHeader(){}

  void  Reset(){fSecond=0;fMillisecond=-1;}                // invoke it at the begin of Rdc::ProcessThisEvent
  void  SetEventID(const long &v) {fEventID=v;}     // one good event
  void  SetTime(char *time){
    for(size_t i=0;i<4;++i){        // 4 bytes second
      fSecond = fSecond*256 + (short)(unsigned char)time[i];
    }
    fMillisecond = (short)(unsigned char)time[4]*256 + (short)(unsigned char)time[5];          // 2 bytes millisecond
  }
  short GetEventID()const {return fEventID;}
  int   GetSecond()const {return fSecond;}
  short GetMillisecond()const {return fMillisecond;}

private:
  short     fEventID;           // good event
  int       fSecond;            // time second
  short     fMillisecond;       // time millisecond

  ClassDef(DmpEvtHeader,1)
};

#endif


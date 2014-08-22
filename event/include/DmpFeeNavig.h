/*
 *  $Id: DmpFeeNavig.h, 2014-08-10 20:57:03 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 07/08/2014
*/

#ifndef DmpFeeNavig_H
#define DmpFeeNavig_H

#include "TObject.h"
#include "DmpEFeeFlags.h"

//-------------------------------------------------------------------
class DmpFeeNavig : public TObject{
/*
 *  navigator of Fee, no ADC counts in this class
 *
 */
public:
  DmpFeeNavig():FeeID(-1),RunMode(3),Trigger(-1),PackageFlag(0xff),TriggerFlag(-1),CRCFlag(true){}
  DmpFeeNavig(const DmpFeeNavig &r){
    FeeID = r.FeeID;
    RunMode = r.RunMode;
    Trigger = r.Trigger;
    TriggerFlag = r.TriggerFlag;
    PackageFlag = r.PackageFlag;
    CRCFlag = r.CRCFlag;
  }
  DmpFeeNavig& operator=(const DmpFeeNavig &r){
    FeeID = r.FeeID;
    RunMode = r.RunMode;
    Trigger = r.Trigger;
    TriggerFlag = r.TriggerFlag;
    PackageFlag = r.PackageFlag;
    CRCFlag = r.CRCFlag;
  }
  DmpFeeNavig(const short &feeID,const short &runMode,const short &trigger,const short &trgFlag,const char &pkgFlag,const bool &crc):FeeID(feeID),RunMode(runMode),Trigger(trigger),TriggerFlag(trgFlag),PackageFlag(pkgFlag),CRCFlag(crc){}

  short FeeID;
  short RunMode;
  short Trigger;
  short TriggerFlag;
  char  PackageFlag;
  bool  CRCFlag;

  ClassDef(DmpFeeNavig,1)
};

//typedef std::vector<DmpFeeNavig>    V_DmpFeeNavig;

#endif


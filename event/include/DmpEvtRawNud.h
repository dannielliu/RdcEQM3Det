/*
 *  $Id: DmpEvtRawNud.h, 2014-08-20 18:42:40 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#ifndef DmpEvtRawNud_H
#define DmpEvtRawNud_H

#include <vector>
#include "DmpFeeNavig.h"

//-------------------------------------------------------------------
class DmpEvtRawNud : public TObject{
/*
 *  DmpEvtRawNud
 *
 *      this class is used to save output of Raw for Nud
 *
 *   one object mean one Nud event
 *
 */
public:
  DmpEvtRawNud();
  ~DmpEvtRawNud();
  void  Reset();
  void  SetFeeNavigator(const DmpFeeNavig &r){fFeeNavig = r;}
  void  AppendSignal(const short &b,const short &v);
  void  GenerateStatus();

  bool  IsGoodEvent()const{return (fFeeNavig.CRCFlag && fFeeNavig.TriggerFlag && fFeeNavig.PackageFlag);}
  short GetTrigger()const{return fFeeNavig.Trigger;}
  short GetRunMode()const{return fFeeNavig.RunMode;}
  DmpFeeNavig GetFeeNavigator()const{return fFeeNavig;}
  short GetSignal(const short &b)const;

private:
  DmpFeeNavig   fFeeNavig;
  std::vector<short>    fGlobalID;  // block id (0~3)
  std::vector<short>    fADC;

  ClassDef(DmpEvtRawNud,1)
};

#endif



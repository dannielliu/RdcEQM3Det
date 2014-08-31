/*
 *  $Id: DmpBindingRdcEQM.cc, 2014-08-31 22:47:32 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 07/03/2014
*/

#include <boost/python.hpp>

#include "DmpAlgRdcEQM.h"


BOOST_PYTHON_MODULE(libDmpRdcEQM){
  using namespace boost::python;

  // default algorithm
  class_<DmpAlgRdcEQM,boost::noncopyable,bases<DmpVAlg> >("DmpAlgRdcEQM",init<>());
}



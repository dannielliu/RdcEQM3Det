/*
 *  $Id: DmpBindingRdcEQM.cc, 2014-08-20 14:20:02 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 07/03/2014
*/

#include <boost/python.hpp>

#include "DmpRdcAlgEQM.h"


BOOST_PYTHON_MODULE(libDmpRdcEQM){
  using namespace boost::python;

  // default algorithm
  class_<DmpRdcAlgEQM,boost::noncopyable,bases<DmpVAlg> >("DmpRdcAlgEQM",init<>());
}



#include "RootEventData/TEvtRecPi0.h"

ClassImp(TEvtRecPi0)

TEvtRecPi0::TEvtRecPi0() :
  m_unconMass(0),
  m_chisq(0),
  m_hiPx(0),
  m_hiPy(0),
  m_hiPz(0), 
  m_hiPe(0),
  m_loPx(0),
  m_loPy(0),
  m_loPz(0),
  m_loPe(0),
  m_hiEnGamma(0),
  m_loEnGamma(0)
{

}

TEvtRecPi0::~TEvtRecPi0() {
}

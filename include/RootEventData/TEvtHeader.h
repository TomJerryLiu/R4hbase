#ifndef ROOT_TEvtHeader_H
#define ROOT_TEvtHeader_H 1

#include "TObject.h"
#include "TObjArray.h"


class TEvtHeader : public TObject {
    
public:

    TEvtHeader();
    virtual ~TEvtHeader();

    void initialize( Int_t Id, Int_t runId, UInt_t evenTag);

    void Clear(Option_t *option="");
 
    void Print(Option_t *option="") const;


    /// Access the TEvtHeader number
    inline Int_t getEventId() { return m_eventId; };

    /// Access the run number
    inline Int_t getRunId() { return m_runId; };

    inline UInt_t time() const { return m_time; }

    inline void setTime(int value) { m_time = value; }

    inline UInt_t getEventTag() { return m_eventTag;}

    inline void setFlag1(int value) { m_flag1 = value; }
    inline void setFlag2(int value) { m_flag2 = value; }

    inline UInt_t getFlag1() { return m_flag1; }
    inline UInt_t getFlag2() { return m_flag2; }

private:

    /// Event Number 
    Int_t m_eventId;  
    
    /// Run number
    Int_t m_runId;
  
    UInt_t m_time;
    
    //eventTag
    UInt_t m_eventTag;

    // flags
    UInt_t m_flag1;
    UInt_t m_flag2;
    
    ClassDef(TEvtHeader,7)

};

#endif 



#pragma once

#include "flxCore.h"

class flxCoreValueEvtBase : public flxOperation
{
  protected:
    static constexpr uint kOpTypeNone = 0;
    static constexpr uint kOpTypeLessThan = 1;
    static constexpr uint kOpTypeGreaterThan = 2;
    static constexpr uint kOpTypeEqual = 3;
    static constexpr uint kOpTypeNotEqual = 4;
};

template <typename T> class flxCoreValueEvt : public flxCoreValueEvtBase
{

  public:
    flxCoreValueEvt() : _currentState{false}, _pDataSource{nullptr}, _pEvent{nullptr}
    {
        flxRegister(currentMode, "Current Mode", "The current mode of the value test");
        flxRegister(testValue, "Value", "The test value");
    }

    // Delcare a value property - based on template type and this object -- using *internal* template for props
    _flxPropertyTyped<flxCoreValueEvt, T> testValue;

    // Property to set/define the operand
    flxPropertyUint16<flxCoreValueEvt> currentMode = {kOpTypeNone,
                                                          {{"Disabled", kOpTypeNone},
                                                           {"Less Than", kOpTypeLessThan},
                                                           {"Greater Than", kOpTypeGreaterThan},
                                                           {"Equal", kOpTypeEqual},
                                                           {"Not Equal", kOpTypeNotEqual}}};

    void checkValue(void)
    {
    	if (!_pDataSource || !_pEvent || currentMode() == kOpTypeNone)
    		return;

    	T value;

    	bool new_state = logicOps[currentMode()](_pDataSource->get_value(value), testValue());

    	if ( new_state != _currentState )
    	{
    		_currentState = new_state;
    		_pEvent->emit(_currentState);
    	} 
    }

    void setParameter(flxParameterOutScalar &param)
    {
    	_pDataSource = param.accessor();
    }
    void setEvent(flxSignalBool &theEvent)
    {
    	_pEvent = &theEvent;
    }

  private:

  	bool _currentState;

    // Define an array of oprand closures
    typedef bool (*logicOpFn_t)(T, T);
    static constexpr logicOpFn_t logicOps[5] = {// noop
                                            [](T p1, T p2) -> bool { return false; },
                                            // less than
                                            [](T p1, T p2) -> bool { return p1 < p2; },
                                            // greater than
                                            [](T p1, T p2) -> bool { return p1 > p2; },
                                            // equal
                                            [](T p1, T p2) -> bool { return p1 == p2; },
                                            // not equal
                                            [](T p1, T p2) -> bool { return p1 != p2; }};

    // our data source
    flxParameterOutScalar *_pDataSource;
    
    // The event that is triggered
    flxSignalBool * _pEvent;


};
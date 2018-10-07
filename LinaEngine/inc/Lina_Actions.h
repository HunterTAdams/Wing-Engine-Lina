/*
Author: Inan Evin
www.inanevin.com

BSD 2-Clause License
Lina Engine Copyright (c) 2018, Inan Evin All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO
-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
-- BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
-- GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
-- OF SUCH DAMAGE.

4.0.30319.42000
10/6/2018 11:26:54 PM

*/

#pragma once

#ifndef Lina_Actions_H
#define Lina_Actions_H
#include <list>
#include "pch.h"
#include <variant>
#include <type_traits>

enum ActionType
{
	ActionType1,
	ActionType2
};



class Lina_ActionBase
{

public:

	Lina_ActionBase() {};
	virtual void* GetData() { return 0; }
	inline ActionType GetActionType() { return m_ActionType; }

private:
	Lina_ActionBase(const Lina_ActionBase& rhs) = delete;
	ActionType m_ActionType;

};

template<typename T>
class Lina_Action : public Lina_ActionBase
{

public:

	Lina_Action() { }
	inline void SetData(T t) { m_Value = t; }
	virtual void* GetData() { return &m_Value; }

private:
	T m_Value;

};


class Lina_ActionHandlerBase
{
public:

	Lina_ActionHandlerBase() {}
	Lina_ActionHandlerBase(ActionType at) : m_ActionType(at) {};

	inline ActionType GetActionType() { return m_ActionType; }
	inline void SetActionType(ActionType t) { m_ActionType = t; }
	virtual void Control(Lina_ActionBase& action) { };
	virtual void Execute(Lina_ActionBase& action) {};


private:
	ActionType m_ActionType;
};


template<typename T = int>
class Lina_ActionHandler_ValCheck : public Lina_ActionHandlerBase
{

public:

	Lina_ActionHandler_ValCheck() {};
	Lina_ActionHandler_ValCheck(ActionType at) :
		Lina_ActionHandlerBase::Lina_ActionHandlerBase(at) {};

	inline void SetUseCondition(bool b) { m_UseCondition = b; }
	inline void SetUseBinding(bool b) { m_UseBinding = b; }
	inline void SetUseParamCallback(bool b) { m_UseParamCallback = b; }
	inline void SetUseNoParamCallback(bool b) { m_UseNoParamCallback = b; }
	inline void SetCondition(T t) { m_Condition = t; }

	inline bool GetUseBinding() { return m_UseBinding; }
	inline bool GetConditionCheck() { return m_UseCondition; }
	inline bool GetUseParamCallback() { return m_UseParamCallback; }
	inline bool GetUseNoParamCallback() { return m_UseNoParamCallback; }
	inline T GetCondition() { return m_Condition; }

	template<typename U>
	bool CompareData(U u)
	{
		if (std::is_same<U, T>::value)
			return LinaEngine::Internal::comparison_traits<T>::equal(m_Condition, u);

		return false;
	}

	virtual void Control(Lina_ActionBase& action) override
	{
		T* typePointer = static_cast<T*>(action.GetData()); // cast from void* to int*
		if (CompareData(*typePointer))
		{
			Execute(action);
		}
	}


private:
	bool m_UseParamCallback = false;
	bool m_UseNoParamCallback = false;
	bool m_UseBinding = false;
	bool m_UseCondition = false;
	T m_Condition = NULL;
};




template<typename T = int>
class Lina_ActionHandler_CallMethod : public Lina_ActionHandler_ValCheck<T>
{
public:

	Lina_ActionHandler_CallMethod() {}

	Lina_ActionHandler_CallMethod(ActionType at) :
		Lina_ActionHandler_ValCheck<T>::Lina_ActionHandler_ValCheck(at)
	{

	}

	virtual void Control(Lina_ActionBase& action) override
	{
		if (Lina_ActionHandler_ValCheck<T>::GetConditionCheck())
			Lina_ActionHandler_ValCheck<T>::Control(action);
		else
			Execute(action);
	}

	virtual void Execute(Lina_ActionBase& action) override
	{
		if (Lina_ActionHandler_ValCheck<T>::GetUseNoParamCallback())
			m_CallbackNoParam();

		if (Lina_ActionHandler_ValCheck<T>::GetUseParamCallback())
			m_CallbackParam(*static_cast<T*>(action.GetData()));

		if (Lina_ActionHandler_ValCheck<T>::GetUseBinding())
		{
			*m_Binding = *static_cast<T*>(action.GetData());
		}

	
	}

	void SetParamCallback(std::function<void(T)> && cbp) { m_CallbackParam = cbp; }
	void SetNoParamCallback(std::function<void()>&& cb) { m_CallbackNoParam = cb; }
	void SetBinding(T* binding) { m_Binding = binding; }

private:
	T* m_Binding;
	std::function<void()> m_CallbackNoParam;
	std::function<void(T)> m_CallbackParam;
};



class Lina_ActionDispatcher
{

public:

	Lina_ActionDispatcher()
	{

	}
	std::list<Lina_ActionHandlerBase*> m_TestListeners;
	std::list<std::weak_ptr<Lina_ActionHandlerBase>> m_ActionHandlers;

};



class TestClass
{
public:
	TestClass()
	{
		int toBind = 2;
		Lina_ActionDispatcher* disp = new Lina_ActionDispatcher;
		Lina_Action<int> action;
		action.SetData(25);
		//	action.id = "xd";

		auto f = []() {std::cout << "selam" << std::endl; };
		auto f2 = [](int c) {std::cout << "lambda: " << c << std::endl; };

		Lina_ActionHandler_CallMethod<int> b(ActionType::ActionType1);

		//b.SetParamCallback(f);
		//b.SetUseBinding(true);
		//b.SetBinding(&toBind);

		b.SetUseCondition(true);
		b.SetUseNoParamCallback(true);
		b.SetUseParamCallback(true);
		b.SetUseBinding(true);

		b.SetCondition(25);
		b.SetNoParamCallback(f);
		b.SetParamCallback(f2);
		b.SetBinding(&toBind);

		disp->m_TestListeners.push_back(&b);
		disp->m_TestListeners.front()->Control(action);
		std::cout << toBind;
	}

};


//static bool deleteAll(Foo * theElement) { delete theElement; return true; }

//foo_list.remove_if(deleteAll);

#endif
#pragma once
#include <cstdio>
#include <vector>

class Animator;
class AnimatorData{};

class I_AnimatorObserver
{
public:
	virtual ~I_AnimatorObserver(){};
	virtual void on_notify(const Animator& sender, AnimatorData event) = 0;
};

class ExampleObserver: public I_AnimatorObserver
{
public:
	//should not modify the list of observers
	void on_notify(const Animator& sender, AnimatorData event)
	{
		printf("recieved event");
	}
};

class ExampleSubject
{
public:
	void add_observer(I_AnimatorObserver* observer)
	{
		observers.push_back(observer);
	}
	
	void remove_observer(I_AnimatorObserver* observer)
	{
		for (std::vector<I_AnimatorObserver*>::iterator it = observers.begin(); it != observers.end();)
		{
			if (*it == observer){
				it = observers.erase(it);
			}
		}
		
	}
	
protected:
	void notify(const Animator& entity, AnimatorData data)
	{
		for (int i = 0; i < observers.size(); i++)
		{
			observers[i]->on_notify(entity, data);
		}
		
	}
	
private:
	std::vector<I_AnimatorObserver*> observers;
};


class ExampleClass
{
public:
	void foo()
	{
		auto obs = new ExampleObserver();
		auto subj = new ExampleSubject();
		subj->add_observer(obs);
	}	
};
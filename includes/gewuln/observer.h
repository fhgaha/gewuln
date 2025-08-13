#pragma once
#include <cstdio>
#include <vector>

class Entity;
class EventData
{
	
};

class Observer
{
public:
	virtual ~Observer() {};
	virtual void on_notify(const Entity& sender, EventData event) = 0;
};

class Achievements: public Observer
{
public:
	//should not modify the list of observers
	void on_notify(const Entity& sender, EventData event)
	{
		printf("recieved event");
		
	}
};

class Subject
{
public:
	void add_observer(Observer* observer)
	{
		observers.push_back(observer);
	}
	
	void remove_observer(Observer* observer)
	{
		for (std::vector<Observer*>::iterator it = observers.begin(); it != observers.end();)
		{
			if (*it == observer){
				it = observers.erase(it);
			}
		}
		
	}
	
protected:
	void notify(const Entity& entity, EventData event)
	{
		for (int i = 0; i < observers.size(); i++)
		{
			observers[i]->on_notify(entity, event);
		}
		
	}
	
private:
	std::vector<Observer*> observers;
};

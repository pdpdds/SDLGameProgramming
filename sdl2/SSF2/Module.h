#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"

struct Collider;
class Application;

class Module
{
public:

	Module(bool active = true) : active(active)
	{}

	virtual ~Module()
	{}

	bool IsEnabled() const
	{
		return active;
	}

	 bool Enable()
	{
		if(active == false)
			return active = Start();

		return true;
	}

	 bool Disable()
	 {
		 if(active == true)
			 return active = !CleanUp();

		 return true;
	 }

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual UpdateStatus PreUpdate()
	{
		return UpdateStatus_Continue;
	}

	virtual UpdateStatus Update()
	{
		return UpdateStatus_Continue;
	}

	virtual UpdateStatus PostUpdate()
	{
		return UpdateStatus_Continue;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void OnCollision(Collider* c1, Collider* c2)
	{}


private:
	bool active = true;
};

#endif // __MODULE_H__
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

class GameObject {
protected:
	bool ok = true;
public:
    virtual ~GameObject() {}
	virtual bool isOK();
};

#endif

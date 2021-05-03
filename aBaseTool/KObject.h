#ifndef __KOBJECT_H__
#define __KOBJECT_H__

namespace nsq{
class KObject{
public:
	KObject(){
		OnCreate();
	}
	virtual ~KObject(){
	    	OnDestroy();
	}
protected:
	virtual void OnCreate(){}
	virtual void Initialize(){}
	virtual void OnDestroy(){}
};
}

#endif

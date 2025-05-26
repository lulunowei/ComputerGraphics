#pragma once
class Uncopyable
{
protected:
	Uncopyable() {}
	~Uncopyable() {}
private:
	//阻止拷贝复制
	Uncopyable(const Uncopyable& );
	Uncopyable& operator=(const Uncopyable&);
};


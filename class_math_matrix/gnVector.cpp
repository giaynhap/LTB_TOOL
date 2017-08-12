#include "gnVector.h"


gnVector::gnVector(int size)
{
	this->size = size;
}

bool gnVector::operator=(gnVector vec)
{
	
	for (int i=0;i<vec.size;i++ )
	    data[i]= vec.data[i];
	return true;
}
bool gnVector::operator==(gnVector a)
{
	for (int i=0;i<size;i++)
		if (data[i]!=a.data[i]) return false;
	return true;
}
bool gnVector::operator!=(gnVector a)
{
	return  !(*this==a);
}
float gnVector::operator[](int a)
{
	return  data[a];
}

gnVector gnVector::operator+(gnVector vec) const
{
	gnVector rVector (vec.size);
	if (vec.size != size) return rVector;
	for (int i=0;i<size;i++)
		rVector.data[i]=data[i]+vec.data[i];
	return rVector;
}

gnVector gnVector::operator-(gnVector vec) const
{
	gnVector rVector (vec.size);
	if (vec.size != size) return rVector;
	for (int i=0;i<size;i++)
		rVector.data[i]=data[i]-vec.data[i];
	return rVector;
}

float gnVector::operator*(gnVector vec) const
{
	float resual=0.0f;
	if (vec.size != size) return 0.0f;
	for (int i=0;i<size;i++)
		resual += data[i]*vec.data[i];
	return resual;
}

gnVector& gnVector::operator+=(gnVector vec) 
{
	if (vec.size != size) return *this;
	for (int i=0;i<size;i++)
		data[i]+=vec.data[i];
	return *this;	
}

gnVector& gnVector::operator-=(gnVector vec) 
{
	gnVector rVector (vec.size);
	if (vec.size != size) return *this;
	for (int i=0;i<size;i++)
		data[i]-=vec.data[i];
	return *this;	
}
gnVector&  gnVector::operator*=(float a)
{
	for (int i=0;i<size;i++)
		data[i]*=a;
	return *this;
}
gnVector&  gnVector::operator/=(float a) 
{

	for (int i=0;i<size;i++)
		data[i]/=a;
	return *this;
}

gnVector  gnVector::operator*(float a) const
{
	gnVector rVector (size);
	for (int i=0;i<size;i++)
	rVector.data[i]=data[i]*a;
	return rVector;
}
gnVector  gnVector::operator/(float a) const
{
	gnVector rVector (size);
	for (int i=0;i<size;i++)
	rVector.data[i]=data[i]/a;
	return rVector;
}

float gnVector::Length()
{
	 float resual=0.0f;
	 for (int i=0;i<size;i++)
		 resual+=data[i]*data[i];
	 return sqrt(resual);
}


float gnVector::Normalize()
{
	float S = this->Length() ;
	if(S > 0.0f)
	{
		for (int i=0;i<size;i++)
			data[i]*= 1.0f/sqrtf(S);
		return S*  1.0f/sqrtf(S);
	}
	return 0.0f;
}

gnVector::gnVector()
{
	size =0;
}
gnVector::~gnVector()
{
	size =0;
}

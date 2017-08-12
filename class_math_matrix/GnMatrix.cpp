/*
    GIAYNHAP CODER!
 
*/
#include "gnMatrix.h"


/// Tinh Nghich Dao
gnMatrix gnMatrix::Inverse()
{
	gnMatrix rMatrix (size.x,size.y);
	
	if (size.x != size.y)  return rMatrix;

	float nDet =  this->det();
//		printf ("size %i   %i   %f \n",size.x,size.y,nDet);
	for (int i=0 ;i<size.x;i++)
		for (int j=0 ;j<size.y;j++)
		{
			rMatrix.data[j*MAXPT+i] = this->__getM(i,j).det()*pow(-1.0f,i+j+2);
				
		}
		
	rMatrix/=nDet;
	//printf("%f  %f  %f   %f\n%f  %f  %f   %f\n%f  %f  %f   %f\n%f  %f  %f   %f",rMatrix(0,0),rMatrix(0,1),rMatrix(0,2),rMatrix(0,3),rMatrix(1,0),rMatrix(1,1),rMatrix(1,2),rMatrix(1,3),rMatrix(2,0),rMatrix(2,1),rMatrix(2,2),rMatrix(2,3),rMatrix(3,0),rMatrix(3,1),rMatrix(3,2),rMatrix(3,3));

	return rMatrix;
}
/// Tinh Dinh Thuc
float gnMatrix::det()
{
	float mDet = 0.0;
		if (size.x != size.y)  return 0;
		if (size.x>2)
		{
			int j = 0 ;
			for (int i=0;i<size.x;i++)
				mDet+= this->__getM(i,j).det()*pow(-1.0f,i+j+2)*data[i*MAXPT+j];
		}
		else
		{
			mDet = data[0*MAXPT+0]*data[1*MAXPT+1]-data[1*MAXPT+0]*data[0*MAXPT+1];
		}
		return mDet;
}
gnMatrix gnMatrix::__getM(int x, int y)
	{
		gnMatrix rMatrix (size.x-1,size.y-1);

		if (size.x != size.y)  return rMatrix;
		
		int ix=0,iy=0;
		for (int i=0;i<size.x;i++)
		{	if (i!=x)
			{
				for (int j=0;j<size.y;j++)
				{
					if (y!=j)
					{
						rMatrix.data[ix*MAXPT+iy]=data[i*MAXPT+j];
						iy++;
					}
				}
				ix++;
			}
			iy=0;
		}
		return rMatrix;
	};
///// Gan
float& gnMatrix::operator()(int a,int b)
{
	return data[a*MAXPT+b];
}
float gnMatrix::operator()(int a,int b) const
{
	return  data[a*MAXPT+b];
}

gnMatrix& gnMatrix::operator=(gnMatrix& m)
{
	//this->clean_data();
	size.x = m.size.x;
	size.y = m.size.y;
	
	for (int i=0 ;i<size.x;i++)
	{
		
		for (int j=0 ;j<size.y;j++)
			data[i*MAXPT+j] = m.data[i*MAXPT+j];
	}
	return *this;
}



//// Nhan 2 ma tran
gnMatrix gnMatrix::operator*(gnMatrix a) const
{
	gnMatrix rMatrix (size.x,a.size.y);
	if (size.y != a.size.x) return rMatrix;
	for (int i=0 ;i<size.x;i++)
		for (int k=0 ;k<size.y;k++)
			for (int j=0 ;j<size.y;j++)
				rMatrix.data[i*MAXPT+k] += data[i*MAXPT+j]*a.data[j*MAXPT+k];
	  return rMatrix;
}
//             *=
bool gnMatrix::operator*=(gnMatrix a) 
{
	gnMatrix rMatrix(size.x,size.y);
	if (size.y != a.size.x) return false;
	for (int i=0 ;i<size.x;i++)
		for (int k=0 ;k<size.y;k++)
			for (int j=0 ;j<size.y;j++)
				rMatrix.data[i*MAXPT+k] += data[i*MAXPT+j]*a.data[j*MAXPT+k];
	*this =rMatrix ;
	//rMatrix.clean_data();
	  return true;
}

//// Cong ma tran
gnMatrix  gnMatrix::operator+(gnMatrix a) const
{
	gnMatrix rMatrix (size.x,size.y);
	if (size.x != a.size.x || size.y != a.size.y) return rMatrix;  
	
	for (int i=0 ;i<size.x;i++)
		for (int j=0 ;j<size.y;j++)
			rMatrix.data[i*MAXPT+j] = data[i*MAXPT+j]+a.data[i*MAXPT+j];
	return rMatrix;
}

bool  gnMatrix::operator+=(gnMatrix a)
{
	gnMatrix rMatrix (size.x,size.y);
	if (size.x != a.size.x || size.y != a.size.y) return false;  
	
	for (int i=0 ;i<size.x;i++)
		for (int j=0 ;j<size.y;j++)
			rMatrix.data[i*MAXPT+j] = data[i*MAXPT+j]+a.data[i*MAXPT+j];
	*this = rMatrix;
	rMatrix.clean_data();
	return true;
}
//// Tru ma tran
bool  gnMatrix::operator-=(gnMatrix a) 
{
	gnMatrix rMatrix (size.x,size.y);
	if (size.x != a.size.x || size.y != a.size.y) return false;  
	
	for (int i=0 ;i<size.x;i++)
		for (int j=0 ;j<size.y;j++)
			rMatrix.data[i*MAXPT+j] = data[i*MAXPT+j]-a.data[i*MAXPT+j];
	*this = rMatrix;
	//rMatrix.clean_data();
	return true;
}
gnMatrix  gnMatrix::operator-(gnMatrix a) const
{
	gnMatrix rMatrix (size.x,size.y);
	if (size.x != a.size.x || size.y != a.size.y) return rMatrix;  
	
	for (int i=0 ;i<size.x;i++)
		for (int j=0 ;j<size.y;j++)
			rMatrix.data[i*MAXPT +j] = data[i*MAXPT+j]-a.data[i*MAXPT+j];
	return rMatrix;
}
//// Mu ^^^^
gnMatrix gnMatrix::operator^( int a) 
{
	gnMatrix rMatrix (size.x,size.y);
	if (size.x != size.y ) return rMatrix;
	if (a>0)
	{
	rMatrix = *this;
	for (int i=1;i<a;i++)
		rMatrix= rMatrix*(*this);
	}else 
		if (a=-1) rMatrix = this->Inverse();
	return rMatrix;
}
//// Nhan voi mot so
gnMatrix gnMatrix::operator*(float a) const
{
	gnMatrix rMatrix (size.x,size.y);
	for (int i=0 ;i<size.x;i++)
		for (int j=0 ;j<size.y;j++)
			rMatrix.data[i*MAXPT+j] = data[i*MAXPT+j]*a;
	return rMatrix;
}
                 //~~ *=
gnMatrix& gnMatrix::operator*=(float a)
{
	for (int i=0 ;i<size.x;i++)
		for (int j=0 ;j<size.y;j++)
			data[i*MAXPT+j] *= a;
return *this;
}

//// Chia voi mot so
gnMatrix gnMatrix::operator/(float a) const
{
	gnMatrix rMatrix (size.x,size.y);
	for (int i=0 ;i<size.x;i++)
		for (int j=0 ;j<size.y;j++)
			rMatrix.data[i*MAXPT+j] = data[i*MAXPT+j]/a;
	return rMatrix;
}
                 //~~ /=
gnMatrix& gnMatrix::operator/=(float a)
{
	for (int i=0 ;i<size.x;i++)
		for (int j=0 ;j<size.y;j++)
		{
			 data[i*MAXPT+j]/= a;
		}
	return *this;
}
//// 

/// tao ma tran
gnMatrix::gnMatrix()
{
	size.x = 0;size.y = 0;
	
}

gnMatrix::~gnMatrix()
{
	size.x = 0;size.y = 0;
	

}
gnMatrix::gnMatrix(int x,int y)
{
	size.x = x;size.y = y;
	
	for (int i=0;i<x;i++)
	{
		
		for (int j=0;j<y;j++)
			data[i*MAXPT+j]=0;
	}
}

#include "gnLoadLtb.h"
 void gnLoadLtb::PostionSlerp( gnVector p,  gnVector  p2, float t,gnVector &presual)
 {
	presual.data[0]=p.data[0]*(1.0-t)+p2.data[0]*t;
	presual.data[1]=p.data[1]*(1.0-t)+p2.data[1]*t;
	presual.data[2]=p.data[2]*(1.0-t)+p2.data[2]*t;
 }
void gnLoadLtb::QuaternionSlerp( gnVector p,  gnVector  q, float t,gnVector &rqt)
{
	float qt[4];
	int i;
	float omega, cosom, sinom, sclp, sclq;
	float a = 0;
	float b = 0;
	for (i = 0; i < 4; i++) {
		a += (p.data[i]-q.data[i])*(p.data[i]-q.data[i]);
		b += (p.data[i]+q.data[i])*(p.data[i]+q.data[i]);
	}
	if (a > b) {
		for (i = 0; i < 4; i++) {
			q.data[i] = -q.data[i];
		}
	}

	cosom = p.data[0]*q.data[0] + p.data[1]*q.data[1] + p.data[2]*q.data[2] + p.data[3]*q.data[3];

	if ((1.0 + cosom) > 0.00000001) {
		if ((1.0 - cosom) > 0.00000001) {
			omega = acos( cosom );
			sinom = sin( omega );
			sclp = sin( (1.0 - t)*omega) / sinom;
			sclq = sin( t*omega ) / sinom;
		}
		else {
			sclp = 1.0 - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++) {
			qt[i] = sclp * p.data[i] + sclq * q.data[i];
		}
	}
	else {
		qt[0] = -p.data[1];
		qt[1] = p.data[0];
		qt[2] = -p.data[3];
		qt[3] = p.data[2];
		sclp = sin( (1.0 - t) * 0.5 * M_PIl);
		sclq = sin( t * 0.5 * M_PIl);
		for (i = 0; i < 3; i++) {
			qt[i] = sclp * p.data[i] + sclq * qt[i];
		}
	}
	rqt.parse(qt);

}
void gnLoadLtb:: DrawMesh()
{

	for (int x=0;x<m_nMesh;x++)

		for (int j=0;j<m_Mesh[x].m_nSubMesh ;j++)
		{
			glPushMatrix();
		    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
					
			 for( int i=0 ;i<  m_Mesh[x].SubMesh[j].nIdx; i++)
			 {
					if (i==0) glBegin( GL_TRIANGLE_FAN );
					else 
					glBegin( GL_TRIANGLE_STRIP );
					int gtr[3] =  { m_Mesh[x].SubMesh[j].triangles[i][0], m_Mesh[x].SubMesh[j].triangles[i][1], m_Mesh[x].SubMesh[j].triangles[i][2]};
					gnMatrix normal;
					gnMatrix newvert2;
					for (int k=0;k<3;k++)
					{
						int gbone =  m_Mesh[x].SubMesh[j].weights_out[gtr[k]];
						normal=  BoneTrans[gbone] * m_Mesh[x].SubMesh[j].normal[gtr[k]].toMatrix(0.0f);
						newvert2 = BoneTrans[gbone] * m_Mesh[x].SubMesh[j].vertices[gtr[k]].toMatrix(1.0f);
						glNormal3f(normal(0,0),normal(1,0),normal(2,0));
						glVertex3f(newvert2(0,0),newvert2(1,0),newvert2(2,0));
					}
					glEnd( );

			 }
			  glPopMatrix();
		}
	    

}
void gnLoadLtb::DrawBone()
{
	
	float size;
	size = 10.0f;
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glPointSize(4.0f);

		for (int i=0;i<m_nBone;i++)
	{
	glBegin(GL_POINTS);
	glVertex3f(BoneTrans[i](0,3),BoneTrans[i](1,3),BoneTrans[i](2,3));
	glEnd();

		if (m_Bone[i].par>-1)
		{
			glBegin(GL_LINES);
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex3f(BoneTrans[i](0,3),BoneTrans[i](1,3),BoneTrans[i](2,3));
			glVertex3f(BoneTrans[m_Bone[i].par](0,3),BoneTrans[m_Bone[i].par](1,3),BoneTrans[m_Bone[i].par](2,3));
			glEnd();
		}
	}
	glPointSize(1.0f);
}
void gnLoadLtb::Draw_GetAnim()
{
	
	if (m_nAnim<=0) return;
	if (m_Anim[m_iAnim].nkeyframe<0) return ;
	
		
	if (m_iFrame>=m_Anim[m_iAnim].nkeyframe-1 )
	{
		m_iFrame=0;
		m_iTime=0.0;
	}
		m_iTime= m_iFPS;
	
		float total_time = (float)(m_Anim[m_iAnim].listkeyframe[m_iFrame+1]-m_Anim[m_iAnim].listkeyframe[m_iFrame]-1 );
		float is_f = (float)m_iTime/total_time;
		
		for (int i=0;i<m_nBone;i++)
			{
			
				PostionSlerp(m_Anim[m_iAnim].frame[i].pos[m_iFrame],m_Anim[m_iAnim].frame[i].pos[m_iFrame+1],is_f,tmpFrame[i].pos[0]);
				
				QuaternionSlerp( m_Anim[m_iAnim].frame[i].quats[m_iFrame], m_Anim[m_iAnim].frame[i].quats[m_iFrame+1], is_f,tmpFrame[i].quats[0]);

			}
		if (is_f>1) 
			{
				m_iTime=0;		
				m_iFrame+=1;
		}
			Bone_Transform(tmpFrame);
}
void gnLoadLtb::convert_vertex()
{
	tmpFrame = new ltb_frame[m_nBone];
	BoneTrans = new gnMatrix[m_nBone];
	for (int i=0;i<m_nBone;i++)
	{
			tmpFrame[i].pos = new gnVector[1];
			tmpFrame[i].quats = new gnVector[1];
			BoneTrans[i] = m_Bone[i].matdata;
	}
	
	if (m_nAnim<=0) return;

	for (int i=0;i<m_nMesh;i++)
	{
		for (int x=0;x<m_Mesh[i].m_nSubMesh;x++)
		{
			for (int j=0;j< m_Mesh[i].SubMesh[x].nvertices;j++)
			{
					int indexBone  = m_Mesh[i].SubMesh[x].weights_out[j];
						
					gnMatrix MatBoneI = m_Bone[indexBone].matdata.Inverse();
					gnMatrix MatBoneI1 = m_Bone[indexBone].matdata;
					gnMatrix NormalM =MatBoneI*m_Mesh[i].SubMesh[x].normal[j].toMatrix(0.0f) ;
					gnMatrix VertexM =MatBoneI*m_Mesh[i].SubMesh[x].vertices[j].toMatrix(1.0f);
					m_Mesh[i].SubMesh[x].vertices[j].fromMatrix(VertexM);
					m_Mesh[i].SubMesh[x].normal[j].fromMatrix(NormalM);
			}
		}
	}
}

void gnLoadLtb::QuaternionMatrix(gnVector quaternion ,  gnMatrix &matrix )
{
	
matrix(0,0) = 1.0 - 2.0 * quaternion.data[1] * quaternion.data[1] - 2.0 * quaternion.data[2] * quaternion.data[2];
matrix(1,0) = 2.0 * quaternion.data[0] * quaternion.data[1] + 2.0 * quaternion.data[3] * quaternion.data[2];
matrix(2,0) = 2.0 * quaternion.data[0] * quaternion.data[2] - 2.0 * quaternion.data[3] * quaternion.data[1];
matrix(0,1) = 2.0 * quaternion.data[0] * quaternion.data[1] - 2.0 * quaternion.data[3] * quaternion.data[2];
matrix(1,1) = 1.0 - 2.0 * quaternion.data[0] * quaternion.data[0] - 2.0 * quaternion.data[2] * quaternion.data[2];
matrix(2,1) = 2.0 * quaternion.data[1] * quaternion.data[2] + 2.0 * quaternion.data[3] * quaternion.data[0];
matrix(0,2) = 2.0 * quaternion.data[0] * quaternion.data[2] + 2.0 * quaternion.data[3] * quaternion.data[1];
matrix(1,2) = 2.0 * quaternion.data[1] * quaternion.data[2] - 2.0 * quaternion.data[3] * quaternion.data[0];
matrix(2,2) = 1.0 - 2.0 * quaternion.data[0] * quaternion.data[0] - 2.0 * quaternion.data[1] * quaternion.data[1];
matrix(3,0)=0.0;
matrix(3,1)=0.0;
matrix(3,2)=0.0;
matrix(3,3)=1.0;


}
void gnLoadLtb::Bone_Transform(ltb_frame *frame)
{
	
	for (int i=0;i<m_nBone;i++)
	{
	
	//	gnMatrix matrix;
		if (BoneTrans[i].getcol() <4) 
		{
			float a[4][4];
			BoneTrans[i].parse(a);
			printf("aaaa\n");
		}
		gnMatrix matrix(4,4);
		QuaternionMatrix( frame[i].quats[0], matrix );
		
		matrix(0,3)= frame[i].pos[0][0];
		matrix(1,3)= frame[i].pos[0][1];
		matrix(2,3)= frame[i].pos[0][2];
		if (m_Bone[i].par<0)
			BoneTrans[i]=matrix;
		else
		{
		
			BoneTrans[i]=BoneTrans[m_Bone[i].par]*matrix;
				
		}

		matrix.clean_data();
		
	
	}
}
void gnLoadLtb::_load_bone()
{
    for (int n = 0; n < m_nBone; n++)
    {
		int nchar = 0;
		fread (&nchar,2 ,  1,pFile);
		memset(m_Bone[n].name,0,sizeof(m_Bone[n].name));
		fread (m_Bone[n].name,nchar ,  1,pFile);
		int isbone,num2;
		fread (&isbone ,1 ,  1,pFile);
		fread (& num2 ,2 ,  1,pFile);
		m_Bone[n].isbone =isbone;
		m_Bone[n].num2 =num2;
		float aMatrix[4][4];
	
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
				float value ;
				fread (&value ,4 ,  1,pFile);
				aMatrix[i][ j] = value;
            }
        }
		m_Bone[n].matdata.parse(aMatrix);
		int nsubbone;
		fread (& nsubbone  ,4 ,  1,pFile);
		m_Bone[n].nSubbone = nsubbone;
    }
}
void gnLoadLtb::_header_data()
{
	fsize();
	fseek(pFile,0,0);
	int nchar=0;
	uint8  temp[128] ;
    fseek(pFile,20 ,SEEK_CUR);
	fread (&m_version, sizeof(m_version) ,  1,pFile);
	fseek(pFile,8 ,SEEK_CUR);
    fread (&m_nBone, sizeof(m_nBone) ,  1,pFile);
	fseek(pFile,48 ,SEEK_CUR);
	fread (&nchar,2 ,  1,pFile);
	fread (temp,nchar+8 ,  1,pFile);
	fread (&m_nMesh, sizeof(m_nMesh) ,  1,pFile);
	m_Bone = new ltb_bone[m_nBone];
	m_Mesh = new ltb_mesh[m_nMesh];
	_load_mesh();
	_load_bone();
	___par();
	___toWeights();
	m_nAnim=0;
	if (m_fSize - ftell(pFile) >1024)
	{
	 _load_anim();
	}
}
void gnLoadLtb::_load_anim()
{
	int iData = ftell(pFile);
	//printf("  == %i \n",iData);
	int nskipdata = 0;
	fread (& nskipdata  ,4 ,  1,pFile);
    long skipzie = 0; ;
    for (int k = 0; k < nskipdata; k++)
    {
		int nchar = 0;
		fread (&nchar,2 ,  1,pFile);
		fseek(pFile,nchar ,SEEK_CUR);

		fread (& skipzie  ,4 ,  1,pFile);
		fseek(pFile, skipzie * 4 ,SEEK_CUR);
    }
    short CompAnim = 0;
    short CompAnim2 = 0;
    fread (& CompAnim  ,2 ,  1,pFile);
	fread (& CompAnim2  ,2 ,  1,pFile);
	fread (& m_nAnim  ,2 ,  1,pFile);
    m_Anim = new ltb_anim[m_nAnim];
    fseek(pFile, 2 ,SEEK_CUR);
    for (int i = 0; i < m_nAnim; i++)
    {
		fread (m_Anim[i].Dim  ,12,  1,pFile);
		short gnchar = 0;
		fread (&gnchar,2 ,  1,pFile);
		memset( m_Anim[i].name,0,sizeof( m_Anim[i].name));
		fread ( m_Anim[i].name,gnchar ,  1,pFile);
		fseek(pFile, 4 ,SEEK_CUR);
	
		fread (& m_Anim[i].interp_time  ,4 ,  1,pFile);
		fread (& m_Anim[i].nkeyframe  ,4 ,  1,pFile);

		m_Anim[i].listkeyframe = new int[m_Anim[i].nkeyframe];
		m_Anim[i].frame= new ltb_frame[m_nBone];
		
        for (int j = 0; j < m_Anim[i].nkeyframe; j++)
        {
			int temp = 0;
			fread (&temp,4 ,  1,pFile);
			m_Anim[i].listkeyframe[j]=temp;
       
			int nchar = 0;
			fread (&nchar,2 ,  1,pFile);
			fseek(pFile,nchar ,SEEK_CUR);
        }
        short nsup =0;
		fread (&nsup,2 ,  1,pFile);
        bool first = false;
        for (int k = 0; k < m_nBone; k++)
        {
			m_Anim[i].frame[k].pos = new gnVector [m_Anim[i].nkeyframe];
			m_Anim[i].frame[k].quats = new gnVector [m_Anim[i].nkeyframe];
			
			float p[3];
            float q[4];
				
            if (nsup != 0)
            {

                if (first == false)
                {
                   first = true;
                   fseek(pFile,ftell(pFile)-2 ,0);
                }
                unsigned short gframe_2;
                unsigned short gframe_1;
				fread (&gframe_1,2 ,  1,pFile);
                fseek(pFile,2 ,SEEK_CUR);
                for (int j = 0; j < gframe_1; j++)
                {
					//p.clear();
					short a[3]={0,0,0};
					fread (a,6 ,  1,pFile);
					p[0]=(UnpackFromInt16(a[0]));
                    p[1]=(UnpackFromInt16(a[1]));
                    p[2]=(UnpackFromInt16(a[2]));
				
					m_Anim[i].frame[k].pos[j].parse(p);
				
                }
                if (gframe_1 < m_Anim[i].nkeyframe)
                    for (int j = gframe_1; j < m_Anim[i].nkeyframe; j++)
                    {
						m_Anim[i].frame[k].pos[j].parse(p);
                    }

			    fread (&gframe_2,2 ,  1,pFile);
                fseek(pFile,2 ,SEEK_CUR);
				
                for (int j = 0; j < gframe_2; j++)
                {
					short a[4]={0,0,0,0};
					fread (a,8 ,  1,pFile);
					 q[0]=(a[0]/ 32767.0);
					 q[1]=(a[1]/ 32767.0);
					 q[2]=(a[2]/ 32767.0);
					 q[3]=(a[3]/ 32767.0);

                    m_Anim[i].frame[k].quats[j].parse(q);
                }
			
                if (gframe_2 < m_Anim[i].nkeyframe)
                    for (int j = gframe_2; j < m_Anim[i].nkeyframe; j++)
                    {
                        m_Anim[i].frame[k].quats[j].parse(q);
                    }		             																
            }
            else
            {
                if (k >= 2)
                {
                   fseek(pFile,1 ,SEEK_CUR);
                }

                for (int j = 0; j < m_Anim[i].nkeyframe; j++)
                {
                    if (k == 0)
                    {
						short a[6];
						fread (a,12 ,  1,pFile);
                        p[0]=(UnpackFromInt16(a[0]));
						p[1]=(UnpackFromInt16(a[2]));
						p[2]=(UnpackFromInt16(a[4]));
                    }
                    else
                    {
						float a[3];
						fread (a,12 ,  1,pFile);			
						p[0]=(a[0]);
                        p[1]=(a[1]);
					    p[2]=(a[2]);
                    }
                    m_Anim[i].frame[k].pos[j].parse(p);
                }
                for (int j = 0; j < m_Anim[i].nkeyframe; j++)
                {
                    if (k == 0)
                    {
						short a[8];
						fread (a,16,  1,pFile);
						q[0]=(a[0]/32767.0f);
						q[1]=(a[2]/32767.0f);
						q[2]=(a[4]/32767.0f);
						q[3]=(a[6]/32767.0f);
                    }
                    else
                    {
						float a[4];
						fread (a,16 ,  1,pFile);
                        q[0]=(a[0] );
						q[1]=(a[1] );
						q[2]=(a[2] );
						q[3]=(a[3] );
                    }

                     m_Anim[i].frame[k].quats[j].parse(q);
                         
                }

            }
        }
    }

}
void gnLoadLtb::_load_mesh()
{
	for (int i = 0; i < m_nMesh; i++)
     {
		int nchar = 0;
		fread (&nchar,2 ,  1,pFile);
		memset(m_Mesh[i].name,0,sizeof(m_Mesh[i].name));
		fread (m_Mesh[i].name,nchar ,  1,pFile);
		fread (&m_Mesh[i].m_nSubMesh,sizeof (m_Mesh[i].m_nSubMesh) ,  1,pFile);
		fseek(pFile,m_Mesh[i].m_nSubMesh*4 ,SEEK_CUR);
		fseek(pFile,8 ,SEEK_CUR);
		m_Mesh[i].SubMesh = new ltb_submesh[m_Mesh[i].m_nSubMesh];
		_load_submesh(m_Mesh[i] );
	}
}

 void gnLoadLtb::_load_submesh(ltb_mesh Mesh)
 {
	 for (int i = 0; i < Mesh.m_nSubMesh; i++)
       {
			int matnum =0,unk1=0, sectionSize=0;
				fseek(pFile,4 ,SEEK_CUR);
				fread (&matnum,sizeof (matnum) ,  1,pFile);
				fseek(pFile,17 ,SEEK_CUR);
				fread (&unk1,sizeof (unk1) ,  1,pFile);
				fread (&sectionSize,sizeof (sectionSize) ,  1,pFile);
                if (sectionSize != 0)
                {
					int numVerts,numIdx,meshType,a=0;
					fread (&numVerts,sizeof (numVerts) ,  1,pFile);
					fread (&numIdx,sizeof (numIdx) ,  1,pFile);
					fread (&meshType,sizeof (meshType) ,  1,pFile);
					Mesh.SubMesh[i].nvertices = numVerts;
					Mesh.SubMesh[i].nIdx = numIdx;
					Mesh.SubMesh[i].type = meshType;
					Mesh.SubMesh[i].vertices = new gnVector[numVerts];
					Mesh.SubMesh[i].weights = new fweight[numVerts];
					Mesh.SubMesh[i].normal = new gnVector[numVerts];
					Mesh.SubMesh[i].uv = new gnVector[numVerts];
					Mesh.SubMesh[i].triangles = new short3_t[numIdx];
					fseek(pFile,20 ,SEEK_CUR);
					Mesh.SubMesh[i].unk1=unk1;
					if (unk1 == 4)
                    {
						fread (&a,sizeof (a) ,  1,pFile);
						Mesh.SubMesh[i].weightsets = new iweights[1];
						Mesh.SubMesh[i].weightsets[0][0]=a;
					
					}
					else if (unk1 == 5)
                       fread (&a,2 ,  1,pFile);
					 _load_vertices(numVerts,meshType,Mesh.SubMesh[i]);

					 for (int j = 0; j < numIdx; j++)
                    {
                       short3_t indx;
					   fread (indx,sizeof(short3_t),  1,pFile);
					  Mesh.SubMesh[i].triangles[j][0] = indx[0]; Mesh.SubMesh[i].triangles[j][1] = indx[1];Mesh.SubMesh[i].triangles[j][2] = indx[2];
					}

					if (unk1 == 5)
                    {
                        int numWeight = 0;
						 fread (&numWeight,sizeof(numWeight),  1,pFile);

						 Mesh.SubMesh[i].weightsets = new iweights[numWeight];
						 Mesh.SubMesh[i].nWeight=numWeight;
                        for (long j = 0; j < numWeight; j++)
                        {
							short weightsets[2];
							byte weightsets2[4];
							int weightsets3;
							 fread (&weightsets[0],2,  1,pFile);
							 fread (&weightsets[1],2,  1,pFile);
							 fread (&weightsets2[0],1,  1,pFile);
							 fread (&weightsets2[1],1,  1,pFile);
							 fread (&weightsets2[2],1,  1,pFile);
							 fread (&weightsets2[3],1,  1,pFile);
							 fread (&weightsets3,4,  1,pFile);						 
							 Mesh.SubMesh[i].weightsets[j][0]=weightsets[0];
							 Mesh.SubMesh[i].weightsets[j][1]=weightsets[1];
						  	 Mesh.SubMesh[i].weightsets[j][2]=weightsets2[0];
							 Mesh.SubMesh[i].weightsets[j][3]=weightsets2[1];
							 Mesh.SubMesh[i].weightsets[j][4]=weightsets2[2];
							 Mesh.SubMesh[i].weightsets[j][5]=weightsets2[3];
							 Mesh.SubMesh[i].weightsets[j][6]=weightsets3;
							
                        }
                    }
					int unk2 =0;
					fread (&unk2,1,  1,pFile);
					fseek(pFile,unk2 ,SEEK_CUR);
				}
	   }
 }
 void gnLoadLtb:: _load_vertices(int numVerts,int meshType,ltb_submesh Sub ){
	  if (meshType == 3)
            meshType = LTB_MESHTYPE_TWOEXTRAFLOAT;
	bool IncludeWeights = false;
	if (meshType == LTB_MESHTYPE_NOTSKINNED)
        IncludeWeights = false;
    else
        IncludeWeights = true;
	 for (int i = 0; i < numVerts; i++)
     {
		 float vert[3];
		 float normal[3];
		 fread (vert,sizeof (vert) ,  1,pFile);

		 Sub.vertices[i].parse(vert);

		 if (IncludeWeights)
         {
			float f1=0.0f, f2 = 0.0f,f3 = 0.0f,f4=0.0f;
		    fread (&f1,sizeof (f1) ,  1,pFile);
            if (meshType != LTB_MESHTYPE_EXTRAFLOAT)  fread (&f2,sizeof (f2) ,  1,pFile);
            else f2 = 1.0f - f1;
            if (meshType != LTB_MESHTYPE_TWOEXTRAFLOAT && meshType != LTB_MESHTYPE_EXTRAFLOAT)  fread (&f3,sizeof (f3) ,  1,pFile);
            else f3 = 1.0f - (f2 + f1);

			f4=1.0f-(f1+f2+f3);
			
			Sub.weights[i][0]=f1;	Sub.weights[i][1]=f2;Sub.weights[i][2]=f3;
			Sub.weights[i][3]=f4;
		 }
		 fread (normal,sizeof (normal) ,  1,pFile);
		 Sub.normal[i].parse( normal);
		 float uv[2];
		 fread (uv,sizeof (uv) ,  1,pFile);
		//  uv[1]=1.0f-uv[1];
		 if (uv[0] > 1.0f) uv[0] -= 1.0f;
		 Sub.uv[i].parse(uv);
	 }
}
gnLoadLtb::gnLoadLtb(char file[])
{
	pFile =  fopen (file, "rb");
	 _header_data();
	 m_iAnim=0;
	 m_iTime=0;
	 m_iFrame=0;
};
gnLoadLtb::gnLoadLtb(wchar_t file[])
{
	pFile =  _wfopen (file, L"rb");
	 _header_data();
	 m_iAnim=0;
	 m_iTime=0;
	 m_iFrame=0;
};

void gnLoadLtb::___par()
{
    int nsubone[512];
    nsubone[0] = m_Bone[0].nSubbone;
    m_Bone[0].par = -1;
    for (int i = 1; i < m_nBone; i++)
    {
        nsubone[i] = m_Bone[i].nSubbone;
        for (int j = i - 1; j >= 0; j--)
            if (nsubone[j] > 0)
            {
                nsubone[j] -= 1;
                m_Bone[i].par = j;
                break;
            }
    }
}
int gnLoadLtb::___weight(int* num, float* size)
{
    float max = size[0];
    int boneWeight = num[1];
    if (num[0] > 4) num[0] = 4;
    for (int i = 1; i < num[0]; i++)
    {
        if (max < size[i])
        {
            max = size[i];
            boneWeight = num[i + 1];
        }
    }
    return boneWeight;
}
void gnLoadLtb::___toWeights()
{
    for (int i = 0; i < m_nMesh; i++)
    { 
		for (int x = 0 ;x< m_Mesh[i].m_nSubMesh;x++)
		{
			m_Mesh[i].SubMesh[x].weights_out = new int[m_Mesh[i].SubMesh[x].nvertices];
			if (m_Mesh[i].SubMesh[x].unk1!=4)
			{
				int pWeightset = 0;
				for (int j = 0; j < m_Mesh[i].SubMesh[x].nWeight ; j++)
				{
					int intWeightSet[5];
					int num = 0;
					for (int n = 0; n < 4; n++)
					{
						if (m_Mesh[i].SubMesh[x].weightsets[j][2 + n] > -1)
						{
							intWeightSet[n + 1] =m_Mesh[i].SubMesh[x].weightsets[j][2 + n];
							num += 1;
						}
						else break;
					}
					intWeightSet[0] = num;
						char a[32];
					for (int k = 0; k < m_Mesh[i].SubMesh[x].weightsets[j][1]; k++)
					{
						float WeightsetSize[7];
						for (int h =0;h<5;h++)
						WeightsetSize[h] = m_Mesh[i].SubMesh[x].weights[pWeightset][h];
						int outw = ___weight(intWeightSet, WeightsetSize);
						m_Mesh[i].SubMesh[x].weights_out[pWeightset]=outw;
						pWeightset += 1;
					}
				}
			}
			else
			{
				for (int j = 0; j < m_Mesh[i].SubMesh[x].nvertices; j++)
					 m_Mesh[i].SubMesh[x].weights_out[j]= m_Mesh[i].SubMesh[x].weightsets[0][0];
			}
		
		}


    }
}
#ifndef GNMODELCLASS
#define GNMODELCLASS
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <cstdint>
#include <math.h>
#include <cmath>  
#include "GnMatrix.h";
#include "GnVector.h";
#include "gl\glut.h"
#include "LmzaExtractor.h"
#define TIMESTEP   10
#ifndef M_PIl 
# define M_PIl          3.141592653589793238462643383279502884L 
#endif
#define LTB_MESHTYPE_NOTSKINNED  1
#define LTB_MESHTYPE_EXTRAFLOAT  2
#define LTB_MESHTYPE_SKINNED  4
#define LTB_MESHTYPE_SKINNEDALT  3
#define LTB_MESHTYPE_TWOEXTRAFLOAT  5
typedef unsigned int        uint;
typedef char			int8;
typedef short int		int16;
typedef int				int32;
typedef __int64         int64;
typedef unsigned char		uint8;
typedef unsigned short int	uint16;
typedef unsigned int		uint32;
typedef unsigned __int64    uint64;
typedef char	filename[128];
typedef short	short3_t[3];
typedef int	 iweights[7];
typedef float	 fweight[4];
#define UnpackFromInt16(a)((float)a/16.0)

typedef struct ltb_bone
{
	char name[128];
	int nSubbone;
	gnMatrix matdata;
	int isbone;
	int num2;
	int par;
} ltb_bone_t;

typedef struct ltb_frame
{
	int indexframe;
	gnVector *pos;
	gnVector *quats;
}ltb_frame_t;
typedef struct ltb_anim
{
	char name[128];
	int nkeyframe;
	int *listkeyframe;
	filename*  listsound;
	float Dim[3];
	int interp_time;
	ltb_frame* frame;

} ltb_anim_t;

typedef struct ltb_submesh
{
	int nvertices;
	int nIdx;
	gnVector *vertices;
	gnVector *normal;
	fweight * weights;
	gnVector *uv;
	iweights *weightsets;
	int *weights_out;
	short3_t *triangles;
	int textindex;
	int type;
	int unk1;
	int nWeight;
	gnVector *drvertices;
	gnVector *drnormal;
};
typedef struct ltb_mesh
{
	ltb_submesh *SubMesh;
	char name[128];
	int m_nSubMesh;

}ltb_mesh_t;




class Model
{
public:
	 void Draw_GetAnim() ;
	 void DrawMesh(int type = 0);
	 void DrawBone();
	 void Bone_Transform(ltb_frame *frame);
	 void convert_vertex();
	 void QuaternionMatrix(gnVector quaternion, gnMatrix &m);
	 void QuaternionMatrix(double * quaternion, gnMatrix &m);
	 void PostionSlerp(gnVector p, gnVector  p2, float t, gnVector &presual);
	 void QuaternionSlerp(gnVector p, gnVector  q, float t, gnVector &rqt);
	static int m_iFPS;
	int m_iAnim;
	float m_iTime;
	int m_iFrame;
	int m_version;
	int m_nBone;
	int m_nMesh;
	int m_nAnim;
	int m_TotalMesh;
	ltb_bone *m_Bone;
	ltb_mesh *m_Mesh;
	ltb_anim *m_Anim;

	gnMatrix *BoneTrans;
	private:
	ltb_frame *tmpFrame;
};


void Model::PostionSlerp(gnVector p, gnVector  p2, float t, gnVector &presual)
{
	presual.data[0] = p.data[0] * (1.0 - t) + p2.data[0] * t;
	presual.data[1] = p.data[1] * (1.0 - t) + p2.data[1] * t;
	presual.data[2] = p.data[2] * (1.0 - t) + p2.data[2] * t;


}
void Model::QuaternionSlerp(gnVector p, gnVector  q, float t, gnVector &rqt)
{
	float qt[4];
	int i;
	float omega, cosom, sinom, sclp, sclq;
	float a = 0;
	float b = 0;
	for (i = 0; i < 4; i++) {
		a += (p.data[i] - q.data[i])*(p.data[i] - q.data[i]);
		b += (p.data[i] + q.data[i])*(p.data[i] + q.data[i]);
	}
	if (a > b) {
		for (i = 0; i < 4; i++) {
			q.data[i] = -q.data[i];
		}
	}

	cosom = p.data[0] * q.data[0] + p.data[1] * q.data[1] + p.data[2] * q.data[2] + p.data[3] * q.data[3];

	if ((1.0 + cosom) > 0.00000001) {
		if ((1.0 - cosom) > 0.00000001) {
			omega = acos(cosom);
			sinom = sin(omega);
			sclp = sin((1.0 - t)*omega) / sinom;
			sclq = sin(t*omega) / sinom;
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
		sclp = sin((1.0 - t) * 0.5 * M_PIl);
		sclq = sin(t * 0.5 * M_PIl);
		for (i = 0; i < 3; i++) {
			qt[i] = sclp * p.data[i] + sclq * qt[i];
		}
	}
	rqt.parse(qt);

}

void Model::QuaternionMatrix(gnVector quaternion, gnMatrix &matrix)
{

	matrix(0, 0) = 1.0 - 2.0 * quaternion.data[1] * quaternion.data[1] - 2.0 * quaternion.data[2] * quaternion.data[2];
	matrix(1, 0) = 2.0 * quaternion.data[0] * quaternion.data[1] + 2.0 * quaternion.data[3] * quaternion.data[2];
	matrix(2, 0) = 2.0 * quaternion.data[0] * quaternion.data[2] - 2.0 * quaternion.data[3] * quaternion.data[1];
	matrix(0, 1) = 2.0 * quaternion.data[0] * quaternion.data[1] - 2.0 * quaternion.data[3] * quaternion.data[2];
	matrix(1, 1) = 1.0 - 2.0 * quaternion.data[0] * quaternion.data[0] - 2.0 * quaternion.data[2] * quaternion.data[2];
	matrix(2, 1) = 2.0 * quaternion.data[1] * quaternion.data[2] + 2.0 * quaternion.data[3] * quaternion.data[0];
	matrix(0, 2) = 2.0 * quaternion.data[0] * quaternion.data[2] + 2.0 * quaternion.data[3] * quaternion.data[1];
	matrix(1, 2) = 2.0 * quaternion.data[1] * quaternion.data[2] - 2.0 * quaternion.data[3] * quaternion.data[0];
	matrix(2, 2) = 1.0 - 2.0 * quaternion.data[0] * quaternion.data[0] - 2.0 * quaternion.data[1] * quaternion.data[1];
	matrix(3, 0) = 0.0;
	matrix(3, 1) = 0.0;
	matrix(3, 2) = 0.0;
	matrix(3, 3) = 1.0;


}


void Model::QuaternionMatrix(double* quaternion, gnMatrix &matrix)
{

	matrix(0, 0) = 1.0 - 2.0 * quaternion[1] * quaternion[1] - 2.0 * quaternion[2] * quaternion[2];
	matrix(1, 0) = 2.0 * quaternion[0] * quaternion[1] + 2.0 * quaternion[3] * quaternion[2];
	matrix(2, 0) = 2.0 * quaternion[0] * quaternion[2] - 2.0 * quaternion[3] * quaternion[1];
	matrix(0, 1) = 2.0 * quaternion[0] * quaternion[1] - 2.0 * quaternion[3] * quaternion[2];
	matrix(1, 1) = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[2] * quaternion[2];
	matrix(2, 1) = 2.0 * quaternion[1] * quaternion[2] + 2.0 * quaternion[3] * quaternion[0];
	matrix(0, 2) = 2.0 * quaternion[0] * quaternion[2] + 2.0 * quaternion[3] * quaternion[1];
	matrix(1, 2) = 2.0 * quaternion[1] * quaternion[2] - 2.0 * quaternion[3] * quaternion[0];
	matrix(2, 2) = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[1] * quaternion[1];
	matrix(3, 0) = 0.0;
	matrix(3, 1) = 0.0;
	matrix(3, 2) = 0.0;
	matrix(3, 3) = 1.0;


}


void Model::Bone_Transform(ltb_frame *frame)
{

	for (int i = 0; i<m_nBone; i++)
	{

		//	gnMatrix matrix;
		if (BoneTrans[i].getcol() <4)
		{
			float a[4][4];
			BoneTrans[i].parse(a);
		}
		gnMatrix matrix(4, 4);
		QuaternionMatrix(frame[i].quats[0], matrix);

		matrix(0, 3) = frame[i].pos[0][0];
		matrix(1, 3) = frame[i].pos[0][1];
		matrix(2, 3) = frame[i].pos[0][2];
		if (m_Bone[i].par<0)
			BoneTrans[i] = matrix;
		else
		{

			BoneTrans[i] = BoneTrans[m_Bone[i].par] * matrix;

		}

		matrix.clean_data();


	}
}
void Model::DrawMesh(int type)
{

	for (int x = 0; x<m_nMesh; x++)

		for (int j = 0; j<m_Mesh[x].m_nSubMesh; j++)
		{
			glPushMatrix();
			if (type == 0 || type == 1)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			else  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (type == 1 || type == 2)
				glDisable(GL_TEXTURE_2D);
			else
			{
				if (this->m_Mesh[x].SubMesh[j].textindex > 0)
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, this->m_Mesh[x].SubMesh[j].textindex);
				}
				else glDisable(GL_TEXTURE_2D);
			}
			for (int i = 0; i< m_Mesh[x].SubMesh[j].nIdx; i++)
			{
				if (i == 0) glBegin(GL_TRIANGLE_FAN);
				else
					glBegin(GL_TRIANGLE_STRIP);
				int gtr[3] = { m_Mesh[x].SubMesh[j].triangles[i][0], m_Mesh[x].SubMesh[j].triangles[i][1], m_Mesh[x].SubMesh[j].triangles[i][2] };
				gnMatrix normal;
				gnMatrix newvert2;
				for (int k = 0; k<3; k++)
				{
					int gbone = m_Mesh[x].SubMesh[j].weights_out[gtr[k]];


					if (m_nAnim>0)
					{

						normal = BoneTrans[gbone] * m_Mesh[x].SubMesh[j].drnormal[gtr[k]].toMatrix(0.0f);
						newvert2 = BoneTrans[gbone] * m_Mesh[x].SubMesh[j].drvertices[gtr[k]].toMatrix(1.0f);

					}
					else
					{
						//printf("%f %f %f\n", m_Mesh[x].SubMesh[j].vertices[gtr[k]][0], m_Mesh[x].SubMesh[j].vertices[gtr[k]][1], m_Mesh[x].SubMesh[j].vertices[gtr[k]][2]);
						normal = m_Mesh[x].SubMesh[j].normal[gtr[k]].toMatrix(0.0f);
						newvert2 = m_Mesh[x].SubMesh[j].vertices[gtr[k]].toMatrix(1.0f);

					}

				 
					glTexCoord2f(m_Mesh[x].SubMesh[j].uv[gtr[k]][0], m_Mesh[x].SubMesh[j].uv[gtr[k]][1]);
					glNormal3f(normal(0, 0), normal(1, 0), normal(2, 0));
					glVertex3f(newvert2(0, 0), newvert2(1, 0), newvert2(2, 0));

				}
				glEnd();

			}
			glPopMatrix();
		}


}
void Model::DrawBone()
{

	float size;
	size = 10.0f;
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glPointSize(4.0f);

	for (int i = 0; i<m_nBone; i++)
	{
		glBegin(GL_POINTS);
		glVertex3f(BoneTrans[i](0, 3), BoneTrans[i](1, 3), BoneTrans[i](2, 3));
		glEnd();

		if (m_Bone[i].par>-1)
		{
			glBegin(GL_LINES);
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex3f(BoneTrans[i](0, 3), BoneTrans[i](1, 3), BoneTrans[i](2, 3));
			glVertex3f(BoneTrans[m_Bone[i].par](0, 3), BoneTrans[m_Bone[i].par](1, 3), BoneTrans[m_Bone[i].par](2, 3));
			glEnd();
		}
	}
	glPointSize(1.0f);
}
void Model::Draw_GetAnim()
{

	if (m_nAnim <= 0) return;
	if (m_Anim[m_iAnim].nkeyframe<0) return;


	if (m_iFrame >= m_Anim[m_iAnim].nkeyframe - 1)
	{
		m_iFrame = 0;
		m_iTime = 0.0;
	}
	m_iTime += this->m_iFPS;
	float total_time = (float)(m_Anim[m_iAnim].listkeyframe[m_iFrame + 1] - m_Anim[m_iAnim].listkeyframe[m_iFrame] - 1);
	float is_f = (float)m_iTime / total_time;

	for (int i = 0; i<m_nBone; i++)
	{

		PostionSlerp(m_Anim[m_iAnim].frame[i].pos[m_iFrame], m_Anim[m_iAnim].frame[i].pos[m_iFrame + 1], is_f, tmpFrame[i].pos[0]);
		QuaternionSlerp(m_Anim[m_iAnim].frame[i].quats[m_iFrame], m_Anim[m_iAnim].frame[i].quats[m_iFrame + 1], is_f, tmpFrame[i].quats[0]);
	}
	if (is_f>1)
	{
		m_iTime = 0;
		m_iFrame += 1;
	}
	Bone_Transform(tmpFrame);



}
void Model::convert_vertex()
{
	tmpFrame = new ltb_frame[m_nBone];
	BoneTrans = new gnMatrix[m_nBone];
	for (int i = 0; i<m_nBone; i++)
	{
		tmpFrame[i].pos = new gnVector[1];
		tmpFrame[i].quats = new gnVector[1];
		BoneTrans[i] = m_Bone[i].matdata;
	}

	if (m_nAnim <= 0) return;

	for (int i = 0; i<m_nMesh; i++)
	{
		for (int x = 0; x<m_Mesh[i].m_nSubMesh; x++)
		{
			m_Mesh[i].SubMesh[x].drvertices = new gnVector[m_Mesh[i].SubMesh[x].nvertices];
			m_Mesh[i].SubMesh[x].drnormal = new gnVector[m_Mesh[i].SubMesh[x].nvertices];

			for (int j = 0; j< m_Mesh[i].SubMesh[x].nvertices; j++)
			{
				int indexBone = m_Mesh[i].SubMesh[x].weights_out[j];

				gnMatrix MatBoneI = m_Bone[indexBone].matdata.Inverse();
				gnMatrix MatBoneI1 = m_Bone[indexBone].matdata;
				gnMatrix NormalM = MatBoneI*m_Mesh[i].SubMesh[x].normal[j].toMatrix(0.0f);
				gnMatrix VertexM = MatBoneI*m_Mesh[i].SubMesh[x].vertices[j].toMatrix(1.0f);

				m_Mesh[i].SubMesh[x].drvertices[j].fromMatrix(VertexM);
				m_Mesh[i].SubMesh[x].drnormal[j].fromMatrix(NormalM);

			}
		}
	}
}







void  GetRotation(gnMatrix m, double * quat);
void quaternionToRotation(double* quaternion, double *rotation);
void quaternionToEuler(double* q, double *rotation);
void rotationMatrixToEulerAngles(gnMatrix matrix, double * euler);
gnMatrix *BoneTrans;
void WorldToLocal(gnMatrix matrix, int par, gnMatrix * listMatrix, gnMatrix *out)
{
	if (par == -1) {
		*out = matrix;
		return;
	}
	gnMatrix parMat = listMatrix[par].Inverse();
	*out = parMat*matrix;
}
void writeSmdMesh(Model *ltb, wchar_t *file)
{

	FILE *fSmd = _wfopen(file, L"wt");

	BoneTrans = new gnMatrix[ltb->m_nBone];

	char Data[20480];
	memset(Data, 0, 20480);
	sprintf(Data, "version 1\nnodes");
	for (int i = 0; i < ltb->m_nBone; i++)
	{

		sprintf(Data, "%s\n%d \"%s\" %d", Data, i, ltb->m_Bone[i].name, ltb->m_Bone[i].par);
	}
	sprintf(Data, "%s\nend\nskeleton", Data);
	sprintf(Data, "%s\ntime 0", Data);


	for (int i = 0; i < ltb->m_nBone; i++)
	{
		//BoneTrans[i] = ltb->m_Bone[i].matdata;
		if (BoneTrans[i].getcol() <4)
		{
			float a[4][4];
			BoneTrans[i].parse(a);
		}
		gnMatrix matrix = ltb->m_Bone[i].matdata;
		gnMatrix BoneTran;
		BoneTrans[i] = matrix;

		if (ltb->m_Bone[i].par<0)
			BoneTran = matrix;
		else
		{
			WorldToLocal(matrix, ltb->m_Bone[i].par, BoneTrans, &BoneTran);
		}

		double *Quat = new double[4];
		double *Rot = new double[3];

		//	GetRotation(BoneTran, Quat);
		//quaternionToRotation(Quat, Rot);
		rotationMatrixToEulerAngles(BoneTran, Rot);
		char tmpStr[1024];

		sprintf(tmpStr, "\n%d %.6f %.6f %.6f %.6f %.6f %.6f", i, BoneTran(0, 3), BoneTran(1, 3), BoneTran(2, 3), Rot[0], Rot[1], Rot[2]);
		strcat(Data, tmpStr);

		delete Quat;
		delete Rot;
	}
	sprintf(Data, "%s\nend\ntriangles", Data);
	fwrite(Data, 1, strlen(Data), fSmd);

	for (int i = 0; i < ltb->m_nMesh; i++)
	{
		for (int j = 0; j < ltb->m_Mesh[i].m_nSubMesh; j++)
		{
			for (int k = 0; k < ltb->m_Mesh[i].SubMesh[j].nIdx; k++)
			{
				short * tr = ltb->m_Mesh[i].SubMesh[j].triangles[k];
				gnVector * vert = ltb->m_Mesh[i].SubMesh[j].vertices;
				gnVector * normal = ltb->m_Mesh[i].SubMesh[j].normal;
				gnVector * uv = ltb->m_Mesh[i].SubMesh[j].uv;
				char tmpStr[1024];
				sprintf(tmpStr, "\n%s_%d.bmp", ltb->m_Mesh[i].name, j);
				for (int n = 0; n<3; n++)
					sprintf(tmpStr, "%s\n%d %0.6f %0.6f %0.6f %0.6f %0.6f %0.6f %0.6f %0.6f", tmpStr, ltb->m_Mesh[i].SubMesh[j].weights_out[tr[n]], vert[tr[n]][0], vert[tr[n]][1], vert[tr[n]][2], normal[tr[n]][0], normal[tr[n]][1], normal[tr[n]][2], uv[tr[n]][0], 1.0-uv[tr[n]][1]);

				fwrite(tmpStr, 1, strlen(tmpStr), fSmd);

			}

		}
	}
	fwrite("\nend\n", 1, 5, fSmd);
	fclose(fSmd);


}

void writeSmdAnim(Model *ltb, int index, wchar_t *file)
{
	FILE *fSmd = _wfopen(file, L"wt");
	ltb_anim anim = ltb->m_Anim[index];
	char tmpStr[4024];
	sprintf(tmpStr, "version 1\nnodes");

	for (int i = 0; i < ltb->m_nBone; i++)
	{

		sprintf(tmpStr, "%s\n %d  \"%s\" %d", tmpStr, i, ltb->m_Bone[i].name, ltb->m_Bone[i].par);
	}
	sprintf(tmpStr, "%s\nend\nskeleton", tmpStr);

	fwrite(tmpStr, strlen(tmpStr), 1, fSmd);
	for (int k = 0; k < anim.nkeyframe; k++)
	{
		memset(tmpStr, 0, 4024);
		sprintf(tmpStr, "\ntime %d", anim.listkeyframe[k]);
		fwrite(tmpStr, strlen(tmpStr), 1, fSmd);
		for (int i = 0; i < ltb->m_nBone; i++)
		{
			
			memset(tmpStr, 0, 4024);
			double * quat = new double[4]{ anim.frame[i].quats[k][0], anim.frame[i].quats[k][1], anim.frame[i].quats[k][2], anim.frame[i].quats[k][3] };
			double * pos = new double[3]{ anim.frame[i].pos[k][0], anim.frame[i].pos[k][1], anim.frame[i].pos[k][2] };
			double  rotation[3];
			double length = sqrt(quat[0] * quat[0] + quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3]);
			quat[0] /= length;
			quat[1] /= length;
			quat[2] /= length;
			quat[3] /= length;
			gnMatrix matrix(4, 4);
		 
			ltb->QuaternionMatrix(quat, matrix);
			//quaternionToEuler(quat, rotation);
			rotationMatrixToEulerAngles(matrix, rotation);
			// eulerAngles(matrix, rotation);
			sprintf(tmpStr, "\n%d  %0.6f %0.6f %0.6f %0.6f %0.6f %0.6f", i, pos[0], pos[1], pos[2], rotation[0], rotation[1], rotation[2]);

			fwrite(tmpStr, strlen(tmpStr), 1, fSmd);

		}

	}
	fwrite("\nend", 4, 1, fSmd);
	fclose(fSmd);
}
void  GetRotation(gnMatrix m, double * quat)
{
	double tr = m(0, 0) + m(1, 1) + m(2, 2);
	double x, y, z, w;
	if (tr > 0.0)
	{
		double s = sqrt(1.0 + tr) * 2.0;
		w = 0.25f * s;
		x = (m(2, 1) - m(1, 2)) / s;
		y = (m(0, 2) - m(2, 0)) / s;
		z = (m(1, 0) - m(0, 1)) / s;
	}
	else if ((m(0, 0) > m(1, 1)) && (m(0, 0) > m(2, 2)))
	{
		double s = sqrt(1.0 + m(0, 0) - m(1, 1) - m(2, 2)) * 2.0;
		w = (m(2, 1) - m(1, 2)) / s;
		x = 0.25f * s;
		y = (m(0, 1) + m(1, 0)) / s;
		z = (m(0, 2) + m(2, 0)) / s;
	}
	else if (m(1, 1) > m(2, 2))
	{
		double s = sqrt(1.0 + m(1, 1) - m(0, 0) - m(2, 2)) * 2.0;
		w = (m(0, 2) - m(2, 0)) / s;
		x = (m(0, 1) + m(1, 0)) / s;
		y = 0.25f * s;
		z = (m(1, 2) + m(2, 1)) / s;
	}
	else
	{
		double s = sqrt(1.0 + m(2, 2) - m(0, 0) - m(1, 1)) * 2.0;
		w = (m(1, 0) - m(0, 1)) / s;
		x = (m(0, 2) + m(2, 0)) / s;
		y = (m(1, 2) + m(2, 1)) / s;
		z = 0.25f * s;
	}

	quat[0] = x; quat[1] = y; quat[2] = z; quat[3] = w;

}
void normal_rotate(double &angle)
{
	while (angle > 2 * (M_PIl)) angle -= 2 * (M_PIl);
	while (angle < 2 * (M_PIl)) angle += 2 * (M_PIl);

}
void rotationMatrixToEulerAngles(gnMatrix matrix, double * euler)
{


 
	double sy = sqrt(matrix(0, 0) * matrix(0, 0) + matrix(1, 0) *matrix(1, 0));

	bool singular = sy < 1e-6; // If

	double x, y, z;
	if (!singular)
	{
		x = atan2(matrix(2, 1), matrix(2, 2));
		y = atan2(-matrix(2, 0), sy);
		z = atan2(matrix(1, 0), matrix(0, 0));
	}
	else
	{
		 
			x = atan2(matrix(1, 2), matrix(1, 1));
			y = atan2(-matrix(2, 0), sy);
			z = 0;
		 
		if (matrix(2, 0) >0 && matrix(0, 1)>0)
		{
			x = atan2(matrix(1, 2), matrix(1, 1));
			y = atan2(-matrix(2, 0), sy);
			z = M_PIl;
		}
	}
	 
	normal_rotate(x);
	normal_rotate(y);
	normal_rotate(z);
	euler[0] = x;
	euler[1] = y;
	euler[2] = z;


}

bool closeEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon()) {
	return (epsilon >  abs(a - b));
}

void quaternionToRotation(double* quaternion, double *rotation)
{
	double length = quaternion[0] * quaternion[0] + quaternion[1] * quaternion[1] + quaternion[2] * quaternion[2] + quaternion[2] * quaternion[2];
	if (length > 1)
	{
		quaternion[1] /= length;
		quaternion[0] /= length;
		quaternion[2] /= length;
		quaternion[3] /= length;
	}
	double testw = 2.0*(quaternion[3] * quaternion[1] - quaternion[2] * quaternion[0]);
	testw = (testw > 1.0) ? 1.0 : testw;
	testw = (testw < -1.0) ? -1.0 : testw;
	rotation[0] = atan2(2 * (quaternion[3] * quaternion[0] + quaternion[1] * quaternion[2]), 1 - 2 * (quaternion[0] * quaternion[0] + quaternion[1] * quaternion[1]));
	rotation[1] = asin(2 * (testw));
	rotation[2] = atan2(2 * (quaternion[3] * quaternion[2] + quaternion[0] * quaternion[1]), 1 - 2 * (quaternion[1] * quaternion[1] + quaternion[2] * quaternion[2]));

	//if (isnan(rotation[1])) rotation[1] = (M_PIl / 2.0f) * (testw / abs(testw));
}
void quaternionToEuler(double* q, double *pitchYawRoll)
{
	// Store the Euler angles in radians

	double sqw = q[3] * q[3];
	double sqx = q[0] * q[0];
	double sqy = q[1] * q[1];
	double sqz = q[2] * q[2];

	double unit = sqx + sqy + sqz + sqw;
	double test = q[0] * q[1] + q[2] * q[3];
	if (test > 0.499*unit) { // singularity at north pole
		pitchYawRoll[2] = 2 * atan2(q[0], q[3]);
		pitchYawRoll[1] = M_PIl / 2.0;
		pitchYawRoll[0] = 0;
		return;
	}
	if (test < -0.499*unit) { // singularity at south pole
		pitchYawRoll[2] = -2 * atan2(q[0], q[3]);
		pitchYawRoll[1] = -M_PIl / 2.0;
		pitchYawRoll[0] = 0;
		return;
	}
	pitchYawRoll[2] = atan2(2 * (q[1] * q[3] - q[0] * q[2]), sqx - sqy - sqz + sqw);
	pitchYawRoll[1] = asin(2 * test / unit);
	pitchYawRoll[0] = atan2(2 * (q[0] * q[3] - q[1] * q[2]), -sqx + sqy - sqz + sqw);
}






#endif
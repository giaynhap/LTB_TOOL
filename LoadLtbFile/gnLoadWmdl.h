
#ifndef GNWMDLFILE 
#define GNWMDLFILE
#include "Model.h"
class gnLoadWMDL : public Model
{

public :
	gnLoadWMDL(wchar_t * file);
	gnLoadWMDL(char * file);
	void  calc_normalvector(int mesh);
	int _loadData(int start=0);
	void _parse_mesh()
	{
		m_iAnim = 0;
		m_iTime = 0;
		m_iFrame = 0;
		m_nMesh = 0;
		m_Bone = new ltb_bone[1];
		m_Bone[0].matdata.clean_data();
		sprintf(m_Bone[0].name, "%s", "root");
		m_Bone[0].par = -1;
		
		m_Mesh = new ltb_mesh[20];

		int nextstart = 0;
		do
		{ 
			nextstart = _loadData(nextstart);
		} while (nextstart != 0);
	}
private :
FILE  *pFile;
long m_fSize;
};


gnLoadWMDL::gnLoadWMDL(wchar_t * file)
{
	pFile = _wfopen(file, L"rb");
 
	_parse_mesh();
}
gnLoadWMDL::gnLoadWMDL(char * file)
{
	pFile = fopen(file,"rb");

	_parse_mesh();
	
}
void clac_vec(float* vector_1, float* vector_2, float scale ,float *normal)
{
 
	normal[0] = vector_1[1] * vector_2[2] - vector_1[2] * vector_2[1];
	normal[1] = vector_1[2] * vector_2[0] - vector_1[0] * vector_2[2];
	normal[2] = vector_1[0] * vector_2[1] - vector_1[1] * vector_2[0];
	float lenght = (float)sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	normal[0] /= lenght / scale; normal[1] /= lenght / scale; normal[2] /= lenght / scale;
 
}
void  gnLoadWMDL::calc_normalvector(int mesh)
{
	ltb_submesh *fmesh = &(m_Mesh[mesh].SubMesh[0]);
 
	for (int j = 0; j < 	fmesh->nIdx; j++)
	{
		int tr[3] = { fmesh->triangles[j][0],fmesh->triangles[j][1],fmesh->triangles[j][2] };

		gnVector vert_0 = fmesh->vertices[tr[0]];
		gnVector vert_1 = fmesh->vertices[tr[1]];
		gnVector vert_2 = fmesh->vertices[tr[2]];

		float vector_1[3] ={vert_0[0] - vert_1[0], vert_0[1] - vert_1[1], vert_0[2] - vert_1[2]};
		float vector_2[3] = { vert_0[0] - vert_2[0], vert_0[1] - vert_2[1], vert_0[2] - vert_2[2] };

		float  vnormal[3];
		clac_vec(vector_1, vector_2, 1.0f, vnormal);
		fmesh->normal [tr[0]].parse(vnormal);
		fmesh->normal[tr[1]].parse(vnormal);
		fmesh->normal[tr[2]].parse(vnormal);
	}
}

int convert_bit(int num)
{
	uint32_t b0, b1, b2, b3;
	uint32_t res;

	b0 = (num & 0x000000ff) << 24u;
	b1 = (num & 0x0000ff00) << 8u;
	b2 = (num & 0x00ff0000) >> 8u;
	b3 = (num & 0xff000000) >> 24u;

	res = b0 | b1 | b2 | b3;
	return res;
}
int gnLoadWMDL::_loadData(int start)
{
	unsigned int unk0;
	unsigned int numVerts, sizeofvec, ENDVERBUFF, START_NumIndx, EndMESH, EndFILE, STARTNEXTMESH;
	unsigned int numIdx, FileSize, skipsize;
	unsigned char tsub;
	fread(&m_version, 4, 1, pFile);
	fread(&unk0, 4, 1, pFile);
	fread(&numVerts, 4, 1, pFile);
	fseek(pFile, 4, SEEK_CUR);
	fread(&sizeofvec, 4, 1, pFile);
	fseek(pFile, 8, SEEK_CUR);
	fread(&ENDVERBUFF, 4, 1, pFile);
	fseek(pFile, 4, SEEK_CUR);
	fread(&START_NumIndx, 4, 1, pFile);

	fread(&numIdx, 4, 1, pFile);
	fread(&FileSize, 4, 1, pFile);
	fread(&skipsize, 4, 1, pFile);
	fseek(pFile, 8, SEEK_CUR);

	fread(&EndMESH, 4, 1, pFile);
	fseek(pFile, 12, SEEK_CUR);
	fread(&EndFILE, 4, 1, pFile);
	fseek(pFile, 4, SEEK_CUR);
	 
	fread(&STARTNEXTMESH, 4, 1, pFile);
	fseek(pFile, unk0+1, SEEK_SET);
	fread(&tsub, 1, 1, pFile);
	fseek(pFile, (int)tsub, SEEK_CUR);
	m_nBone = 0;
	
 
	 
	m_Mesh[m_nMesh].m_nSubMesh = 1;
	m_Mesh[m_nMesh].SubMesh = new ltb_submesh[1];
	m_Mesh[m_nMesh].SubMesh[0].nIdx = numIdx;
	m_Mesh[m_nMesh].SubMesh[0].nvertices = numVerts;
	m_Mesh[m_nMesh].SubMesh[0].vertices = new gnVector[numVerts];
	m_Mesh[m_nMesh].SubMesh[0].weights = NULL;
	m_Mesh[m_nMesh].SubMesh[0].normal = new gnVector[numVerts];
	m_Mesh[m_nMesh].SubMesh[0].uv = new gnVector[numVerts];
	m_Mesh[m_nMesh].SubMesh[0].triangles = new short3_t[numIdx];
	m_Mesh[m_nMesh].SubMesh[0].weights_out = new int[numVerts];
	 
	int   numvar = int((sizeofvec / numVerts) / 4) - 5;
	printf("%d", numVerts);
	for (int i = 0; i < numVerts; i++)
	{
		
		float normal[3] = { 0,0,0 };
		float vertex[3];
		float uv[2];
		fread(vertex, 4, 3, pFile);
		fread(uv, 4, 2, pFile);
		fseek(pFile, numvar * 4, 1);
		m_Mesh[m_nMesh].SubMesh[0].weights_out[i] = 0;
		m_Mesh[m_nMesh].SubMesh[0].vertices[i].parse(vertex);
	 
		m_Mesh[m_nMesh].SubMesh[0].normal[i].parse(normal);
		m_Mesh[m_nMesh].SubMesh[0].uv[i].parse(uv);
	 
	}

	fseek(pFile, EndMESH- numIdx * 3 * 4,0);


	for (int i = 0; i < numIdx; i++)
	{
		int tri[3];
		fread(tri, 4, 3, pFile);
		 
		m_Mesh[m_nMesh].SubMesh[0].triangles[i][0] = convert_bit(tri[0]);
		m_Mesh[m_nMesh].SubMesh[0].triangles[i][1] = convert_bit(tri[1]);
		m_Mesh[m_nMesh].SubMesh[0].triangles[i][2] = convert_bit(tri[2]);
	}

	sprintf(m_Mesh[m_nMesh].name, "Mesh-%d", m_nMesh);
	calc_normalvector(m_nMesh);
	m_nMesh++;
	 
	return STARTNEXTMESH;
}

 
#endif
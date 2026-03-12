#include "model.h"

void Model::InitMaterialBuffer() {
	HRESULT hr;
	D3D11_BUFFER_DESC materialBufferDesc = { 0 };
	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(MaterialBuffer);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&materialBufferDesc, nullptr, &m_material_buffer));
}

void Model::UpdateMaterialBuffer(vec4f ambient, vec4f diffuse, vec4f specular) const {
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_material_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	MaterialBuffer* materialBuffer = (MaterialBuffer*)resource.pData;
	materialBuffer->Ambient = ambient;
	materialBuffer->Diffuse = diffuse;
	materialBuffer->Specular = specular;
	m_dxdevice_context->Unmap(m_material_buffer, 0);
}

void Model::compute_TB(Vertex& v0, Vertex& v1, Vertex& v2)
{
	//based on modified source code from lengyels blog
	//www.terathon.com/blog/tangent-space.html

	vec3f tangent, binormal;

	//3d vector from v0 --> v1 (model space)
	float x1 = v1.Position.x - v0.Position.x; //delta x between v1, v2
	float y1 = v1.Position.y - v0.Position.y;
	float z1 = v1.Position.z - v0.Position.z;

	//3d vector from v0 --> v2 (model space)
	float x2 = v2.Position.x - v0.Position.x; //delta x between v3, v1
	float y2 = v2.Position.y - v0.Position.y;
	float z2 = v2.Position.z - v0.Position.z;

	//2d vector from v0 --> v1 (texture space)
	float s1 = v1.TexCoord.x - v0.TexCoord.x;
	float t1 = v1.TexCoord.y - v0.TexCoord.y;

	//2d vector from v0 --> v2 (texture space)
	float s2 = v2.TexCoord.x - v0.TexCoord.x;
	float t2 = v2.TexCoord.y - v0.TexCoord.y;

	//this term appears on the right side of the equation
	//after multiplying both sides with inverse of (s, t) matrix
	float r = 1.0F / (s1 * t2 - s2 * t1);

	//matrix multiplication for T and B (with r)
	tangent = vec3f((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
	binormal = vec3f((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);
	
	//Compound assignment of computed vectors to the vertices
	//Since some vertices are part of multiple triangles
	//These vectors become weighted averages when normalized
	v0.Tangent += tangent;
	v1.Tangent += tangent;
	v2.Tangent += tangent;
		
	v0.Binormal += binormal;
	v1.Binormal += binormal;
	v2.Binormal += binormal;
}


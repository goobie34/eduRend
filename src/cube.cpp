#include "cube.h"

Cube::Cube(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context)
	: Model(dxdevice, dxdevice_context)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	//Create temporary vertex array
	Vertex v[24];

	//Front face
	v[0].Position = {-0.5, -0.5f, 0.5f};
	v[1].Position = { 0.5, -0.5f, 0.5f };
	v[2].Position = { 0.5, 0.5f, 0.5f };
	v[3].Position = { -0.5, 0.5f, 0.5f };

	//Right face
	v[4].Position = { 0.5, -0.5f, 0.5f };
	v[5].Position = { 0.5, -0.5f, -0.5f };
	v[6].Position = { 0.5, 0.5f, -0.5f };
	v[7].Position = { 0.5, 0.5f, 0.5f };

	//Back face
	v[8].Position = { 0.5, -0.5f, -0.5f };
	v[9].Position = { -0.5, -0.5f, -0.5f };
	v[10].Position = { -0.5, 0.5f, -0.5f };
	v[11].Position = { 0.5, 0.5f, -0.5f };

	//Left Face
	v[12].Position = { -0.5, -0.5f, -0.5f };
	v[13].Position = { -0.5, -0.5f, 0.5f };
	v[14].Position = { -0.5, 0.5f, 0.5f };
	v[15].Position = { -0.5, 0.5f, -0.5f };
	
	//Top Face
	v[16].Position = { -0.5, 0.5f, 0.5f };
	v[17].Position = { 0.5, 0.5f, 0.5f };
	v[18].Position = { 0.5, 0.5f, -0.5f };
	v[19].Position = { -0.5, 0.5f, -0.5f };
	
	//Bottom Face
	v[20].Position = { -0.5, -0.5f, 0.5f };
	v[21].Position = { 0.5, -0.5f, 0.5f };
	v[22].Position = { 0.5, -0.5f, -0.5f };
	v[23].Position = { -0.5, -0.5f, -0.5f };

	//Array of texture coordinates for each corner of cube face
	vec2f corner_tex_coords[4] = {{0,0}, {0,1}, {1, 1}, {1, 0}};

	//Set normals and texture coordinates, then add to vertex buffer
	for (int i = 0; i < 24; i++) {
		v[i].Normal = { 0, 0, 1 };
		v[i].TexCoord = corner_tex_coords[i % 4]; //ensures that corners of cube a face matches corner of texture space
		vertices.push_back(v[i]);
	}

	// Populate the index array with triangles
	// Triangle #1
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);
	//Triangle #2
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	// Triangle #3
	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(7);
	// Triangle #4
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(7);
	// Triangle #5
	indices.push_back(8);
	indices.push_back(9);
	indices.push_back(11);
	// Triangle #6
	indices.push_back(9);
	indices.push_back(10);
	indices.push_back(11);
	// Triangle #7
	indices.push_back(12);
	indices.push_back(13);
	indices.push_back(15);
	// Triangle #8
	indices.push_back(13);
	indices.push_back(14);
	indices.push_back(15);
	// Triangle #9
	indices.push_back(16);
	indices.push_back(17);
	indices.push_back(19);
	// Triangle #10
	indices.push_back(17);
	indices.push_back(18);
	indices.push_back(19);
	// Triangle #11
	indices.push_back(23);
	indices.push_back(22);
	indices.push_back(20);
	// Triangle #12
	indices.push_back(22);
	indices.push_back(21);
	indices.push_back(20);

	// Vertex array descriptor
	D3D11_BUFFER_DESC vertexbufferDesc{ 0 };
	vertexbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferDesc.CPUAccessFlags = 0;
	vertexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferDesc.MiscFlags = 0;
	vertexbufferDesc.ByteWidth = (UINT)(vertices.size() * sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = &vertices[0];
	// Create vertex buffer on device using descriptor & data
	dxdevice->CreateBuffer(&vertexbufferDesc, &vertexData, &m_vertex_buffer);
	SETNAME(m_vertex_buffer, "VertexBuffer");

	//  Index array descriptor
	D3D11_BUFFER_DESC indexbufferDesc = { 0 };
	indexbufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferDesc.CPUAccessFlags = 0;
	indexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferDesc.MiscFlags = 0;
	indexbufferDesc.ByteWidth = (UINT)(indices.size() * sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA indexData{ 0 };
	indexData.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	dxdevice->CreateBuffer(&indexbufferDesc, &indexData, &m_index_buffer);
	SETNAME(m_index_buffer, "IndexBuffer");

	m_number_of_indices = (unsigned int)indices.size();
}

void Cube::Render() const
{
	// Bind our vertex buffer
	const UINT32 stride = sizeof(Vertex); //  sizeof(float) * 8;
	const UINT32 offset = 0;
	m_dxdevice_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	// Bind our index buffer
	m_dxdevice_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Make the drawcall
	m_dxdevice_context->DrawIndexed(m_number_of_indices, 0, 0);
}
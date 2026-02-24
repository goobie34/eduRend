
#include "Scene.h"
#include "QuadModel.h"
#include "cube.h"
#include "OBJModel.h"

Scene::Scene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	m_dxdevice(dxdevice),
	m_dxdevice_context(dxdevice_context),
	m_window_width(window_width),
	m_window_height(window_height)
{ }

void Scene::OnWindowResized(
	int new_width,
	int new_height)
{
	m_window_width = new_width;
	m_window_height = new_height;
}

OurTestScene::OurTestScene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	Scene(dxdevice, dxdevice_context, window_width, window_height)
{ 
	InitTransformationBuffer();
	// + init other CBuffers
	InitLightCamBuffer();

	InitSamplerState();
}

//
// Called once at initialization
//
void OurTestScene::Init()
{
	m_camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)m_window_width / m_window_height,	// aspect ratio
		1.0f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)

	// Move camera to (0,0,5)
	m_camera->MoveTo({ 0, 0, 5 });

	//Create light sources
	m_light_pos = { 0, 0,-4 };
	m_light_debug_model = new OBJModel("assets/sphere/sphere.obj", m_dxdevice, m_dxdevice_context);
	m_sun = new OBJModel("assets/carbody/carbody.obj", m_dxdevice, m_dxdevice_context);

	// Create objects
	m_quad = new QuadModel(m_dxdevice, m_dxdevice_context);
	m_cube = new Cube(m_dxdevice, m_dxdevice_context);
	m_sponza = new OBJModel("assets/crytek-sponza/sponza.obj", m_dxdevice, m_dxdevice_context);

	//Solar system model objects
	//m_sun = new OBJModel("assets/sphere/sphere.obj", m_dxdevice, m_dxdevice_context);
	m_sun = new OBJModel("assets/carbody/carbody.obj", m_dxdevice, m_dxdevice_context);
	m_earth = new OBJModel("assets/sphere/sphere.obj", m_dxdevice, m_dxdevice_context);
	m_moon = new OBJModel("assets/sphere/sphere.obj", m_dxdevice, m_dxdevice_context);
}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(
	float dt,
	const InputHandler& input_handler)
{
	//converting camera direction vectors to world space
	vec4f cam_forward_local_homogenous = { 0, 0, -1, 0};
	vec4f cam_forward_world_homogenous = m_camera->ViewToWorldMatrix() * cam_forward_local_homogenous;
	vec3f cam_forward_world = { cam_forward_world_homogenous.x, cam_forward_world_homogenous.y, cam_forward_world_homogenous.z };
	
	vec4f cam_right_local_homogenous = { 1, 0, 0, 0};
	vec4f cam_right_world_homogenous = m_camera->ViewToWorldMatrix() * cam_right_local_homogenous;
	vec3f cam_right_world = { cam_right_world_homogenous.x, cam_right_world_homogenous.y, cam_right_world_homogenous.z };

	// Camera control
	if (input_handler.IsKeyPressed(Keys::LShift))
		m_camera_velocity = m_camera_velocity_sprint;
	else
		m_camera_velocity = m_camera_velocity_default;

	if (input_handler.IsKeyPressed(Keys::W))
		m_camera->Move(cam_forward_world * m_camera_velocity * dt);
	if (input_handler.IsKeyPressed(Keys::S))
		m_camera->Move(cam_forward_world * -m_camera_velocity * dt);
	if (input_handler.IsKeyPressed(Keys::D))
		m_camera->Move(cam_right_world * m_camera_velocity * dt);
	if (input_handler.IsKeyPressed(Keys::A))
		m_camera->Move(cam_right_world * -m_camera_velocity * dt);
	if(input_handler.IsKeyPressed(Keys::Space))
		m_camera->Move({ 0.0f, m_camera_velocity * dt, 0.0f });
	if(input_handler.IsKeyPressed(Keys::LCtrl))
		m_camera->Move({ 0.0f, -m_camera_velocity * dt, 0.0f });
	if(input_handler.IsKeyPressed(Keys::Esc))
		PostQuitMessage(0);


	if (input_handler.IsKeyPressed(Keys::Left)) {
		m_light_pos.x -= m_light_speed * 0.2;
		m_light_debug_model_transform = mat4f::translation(m_light_pos);
	}
	if (input_handler.IsKeyPressed(Keys::Right)) {
		m_light_pos.x += m_light_speed * 0.2;
		m_light_debug_model_transform = mat4f::translation(m_light_pos);
	}
	if (input_handler.IsKeyPressed(Keys::Up)) {
		m_light_pos.z -= m_light_speed * 0.2;
		m_light_debug_model_transform = mat4f::translation(m_light_pos);
	}
	if (input_handler.IsKeyPressed(Keys::Down)) {
		m_light_pos.z += m_light_speed * 0.2;
		m_light_debug_model_transform = mat4f::translation(m_light_pos);
	}

	//light animation
	/*if (m_light_pos.x < -m_light_anim_range) {
		m_light_speed *= -1;
		m_light_pos.x = -m_light_anim_range;
	}
	else if (m_light_pos.x > m_light_anim_range) {
		m_light_speed *= -1;
		m_light_pos.x = m_light_anim_range;
	}
	else {
		m_light_pos.x += m_light_speed * dt;
	}*/

	//std::cout << m_light_pos << std::endl;

	//rotate camera
	linalg:vec2f mouse_delta_xy = {
		static_cast<float>(input_handler.GetMouseDeltaX()),
		static_cast<float>(input_handler.GetMouseDeltaY())};

	//m_camera->Rotate(mouse_delta_xy * m_camera_sensitivity * (m_inverted_camera ? -1.0f : 1.0f));
	m_camera->RotateWithConstraint(mouse_delta_xy * m_camera_sensitivity * (m_inverted_camera ? -1.0f : 1.0f), m_camera_pitch_angle, -m_camera_pitch_angle);

	//Change sampler states during runtime with number keys/numpad
	if (input_handler.IsKeyPressed(Keys::Key1) || input_handler.IsKeyPressed(Keys::Num1))
		InitSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, 1, m_sampler_states_demo[0]);
	if (input_handler.IsKeyPressed(Keys::Key2) || input_handler.IsKeyPressed(Keys::Num2))
		InitSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_MIRROR, 1, m_sampler_states_demo[1]);
	if (input_handler.IsKeyPressed(Keys::Key3) || input_handler.IsKeyPressed(Keys::Num3))
		InitSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, 1, m_sampler_states_demo[2]);
	if (input_handler.IsKeyPressed(Keys::Key4) || input_handler.IsKeyPressed(Keys::Num4))
		InitSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, 1, m_sampler_states_demo[3]);
	if (input_handler.IsKeyPressed(Keys::Key5) || input_handler.IsKeyPressed(Keys::Num5))
		InitSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MIRROR, 1, m_sampler_states_demo[4]);
	if (input_handler.IsKeyPressed(Keys::Key6) || input_handler.IsKeyPressed(Keys::Num6))
		InitSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, 1, m_sampler_states_demo[5]);
	if (input_handler.IsKeyPressed(Keys::Key7) || input_handler.IsKeyPressed(Keys::Num7))
		InitSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, 4, m_sampler_states_demo[6]);
	if (input_handler.IsKeyPressed(Keys::Key8) || input_handler.IsKeyPressed(Keys::Num8))
		InitSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, 8, m_sampler_states_demo[7]);
	if (input_handler.IsKeyPressed(Keys::Key9) || input_handler.IsKeyPressed(Keys::Num9))
		InitSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, 16, m_sampler_states_demo[8]);
	if (input_handler.IsKeyPressed(Keys::Key0) || input_handler.IsKeyPressed(Keys::Num0))
		InitSamplerState(); //defualt

	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	//Solar system transformations
	m_sun_transform = mat4f::translation(0, 10, -7) *
		mat4f::rotation(-m_angle, 0.0f, 0.0f, 1.0f) * 
		mat4f::scaling(1, 1, 1);

	m_earth_transform = m_sun_transform *
		(mat4f::translation(3.5, 0, 0) *
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *
		mat4f::scaling(0.5, 0.5, 0.5));
	
	m_moon_transform = m_earth_transform *
		(mat4f::translation(2.5, 0, 0) *
		mat4f::rotation(0, 0.0f, 0.0f, 0.0f) *
		mat4f::scaling(0.25, 0.25, 0.25));

	// Quad model-to-world transformation
	m_quad_transform = mat4f::translation(0, 0, 0) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%
	
	// Cube model-to-world transformation
	m_cube_transform = mat4f::translation(0, 0, 0) *			// No translation
		/*mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) **/	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	// Sponza model-to-world transformation
	m_sponza_transform = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(0.05f);						 // The scene is quite large so scale it down to 5%

	// Increment the rotation angle.
	m_angle += m_angular_velocity * dt;

	// Print fps
	m_fps_cooldown -= dt;
	if (m_fps_cooldown < 0.0)
	{
		std::cout << "fps " << (int)(1.0f / dt) << std::endl;
//		printf("fps %i\n", (int)(1.0f / dt));
		m_fps_cooldown = 2.0;
	}
}

//
// Called every frame, after update
//
void OurTestScene::Render()
{
	// Bind transformation_buffer to slot b0 of the VS
	m_dxdevice_context->VSSetConstantBuffers(0, 1, &m_transformation_buffer);
	m_dxdevice_context->PSSetConstantBuffers(0, 1, &m_lightcam_buffer);

	UpdateLightCamBuffer(vec4f(m_camera->Position(), 1), vec4f(m_light_pos, 1));

	// Obtain the matrices needed for rendering from the camera
	m_view_matrix = m_camera->WorldToViewMatrix();
	m_projection_matrix = m_camera->ProjectionMatrix();

	//// Load matrices + the Quad's transformation to the device and render it
	//UpdateTransformationBuffer(m_quad_transform, m_view_matrix, m_projection_matrix);
	//m_quad->Render();

	// Load matrices + the cube's transformation to the device and render it
	UpdateTransformationBuffer(m_cube_transform, m_view_matrix, m_projection_matrix);
	m_cube->Render();

	// Load matrices + Sponza's transformation to the device and render it
	UpdateTransformationBuffer(m_sponza_transform, m_view_matrix, m_projection_matrix);
	m_sponza->Render();

	// Solar system render
	UpdateTransformationBuffer(m_sun_transform, m_view_matrix, m_projection_matrix);
	m_sun->Render();

	UpdateTransformationBuffer(m_earth_transform, m_view_matrix, m_projection_matrix);
	m_earth->Render();

	UpdateTransformationBuffer(m_moon_transform, m_view_matrix, m_projection_matrix);
	m_moon->Render();

	// Light debug model
	UpdateTransformationBuffer(m_light_debug_model_transform, m_view_matrix, m_projection_matrix);
	m_light_debug_model->Render();
}

void OurTestScene::Release()
{
	SAFE_DELETE(m_sun);
	SAFE_DELETE(m_earth);
	SAFE_DELETE(m_moon);

	SAFE_DELETE(m_quad);
	SAFE_DELETE(m_cube);
	SAFE_DELETE(m_sponza);
	SAFE_DELETE(m_camera);
	SAFE_DELETE(m_light_debug_model);

	SAFE_RELEASE(m_transformation_buffer);
	// + release other CBuffers

	SAFE_RELEASE(m_lightcam_buffer);
	SAFE_RELEASE(m_sampler_state);

	for (auto& sampler : m_sampler_states_demo) {
		SAFE_RELEASE(sampler);
	}
}

void OurTestScene::OnWindowResized(
	int new_width,
	int new_height)
{
	if (m_camera)
		m_camera->SetAspect(float(new_width) / new_height);

	Scene::OnWindowResized(new_width, new_height);
}

void OurTestScene::InitSamplerState() {
	HRESULT hr;
	D3D11_SAMPLER_DESC sampler_desc = {
		D3D11_FILTER_MIN_MAG_MIP_LINEAR, // Filter
		D3D11_TEXTURE_ADDRESS_MIRROR, // AddressU
		D3D11_TEXTURE_ADDRESS_MIRROR, // AddressV
		D3D11_TEXTURE_ADDRESS_CLAMP, // AddressW
		0.0f, // MipLODBias
		1, // MaxAnisotropy
		D3D11_COMPARISON_NEVER, // ComapirsonFunc
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // BorderColor
		-FLT_MAX, // MinLOD
		FLT_MAX // MaxLOD
	};
	m_dxdevice->CreateSamplerState(&sampler_desc, &m_sampler_state);
	m_dxdevice_context->PSSetSamplers(0, 1, &m_sampler_state);
}
void OurTestScene::InitSamplerState(D3D11_FILTER filter_type,
	D3D11_TEXTURE_ADDRESS_MODE adress_mode,
	int anisotropic_samples,
	ID3D11SamplerState* out_sampler_state)
{
	std::cout << "init sampler state" << std::endl;

	HRESULT hr;
	D3D11_SAMPLER_DESC sampler_desc = {
		filter_type, // Filter
		adress_mode, // AddressU
		adress_mode, // AddressV
		D3D11_TEXTURE_ADDRESS_CLAMP, // AddressW
		0.0f, // MipLODBias
		anisotropic_samples, // MaxAnisotropy
		D3D11_COMPARISON_NEVER, // ComapirsonFunc
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // BorderColor
		-FLT_MAX, // MinLOD
		FLT_MAX // MaxLOD
	};
	ASSERT(hr = m_dxdevice->CreateSamplerState(&sampler_desc, &out_sampler_state));
	m_dxdevice_context->PSSetSamplers(0, 1, &out_sampler_state);
}

void OurTestScene::InitTransformationBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(TransformationBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_transformation_buffer));
}

void OurTestScene::InitLightCamBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC lightCamBufferDesc = { 0 };
	lightCamBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightCamBufferDesc.ByteWidth = sizeof(LightCamBuffer);
	lightCamBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightCamBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightCamBufferDesc.MiscFlags = 0;
	lightCamBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&lightCamBufferDesc, nullptr, &m_lightcam_buffer));
}

void OurTestScene::UpdateTransformationBuffer(
	mat4f ModelToWorldMatrix,
	mat4f WorldToViewMatrix,
	mat4f ProjectionMatrix)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformationBuffer* matrixBuffer = (TransformationBuffer*)resource.pData;
	matrixBuffer->ModelToWorldMatrix = ModelToWorldMatrix;
	matrixBuffer->WorldToViewMatrix = WorldToViewMatrix;
	matrixBuffer->ProjectionMatrix = ProjectionMatrix;
	m_dxdevice_context->Unmap(m_transformation_buffer, 0);
}

void OurTestScene::UpdateLightCamBuffer(
	vec4f camera_position,
	vec4f light_position)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_lightcam_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	LightCamBuffer* lightCamBuffer = (LightCamBuffer*)resource.pData;
	lightCamBuffer->CameraPosition = camera_position;
	lightCamBuffer->LightPosition = light_position;
	m_dxdevice_context->Unmap(m_lightcam_buffer, 0);
}
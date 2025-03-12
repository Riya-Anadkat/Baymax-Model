// Termm-Fall 2020

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"
#include <imgui/imgui.h>
#include <random>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <array>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;
bool use_depth_buffering = true;
bool use_backface_culling = false;
bool use_frontface_culling = false;
int interactionMode = 0;
bool isLeftDragging = false;
bool isMiddleDragging = false;
bool isRightDragging = false;
double previousXPos = 0.0;
double previousYPos = 0.0;
double initialYPos = 0.0;
BatchInfoMap m_batchInfoMap;
glm::mat4 rotationMat4;


std::vector<glm::mat4> xforms;
std::vector<glm::vec3> cols;

bool show_circle = false;


std::vector<std::vector<std::pair<int, glm::mat4>>> undoStack;
std::vector<std::vector<std::pair<int, glm::mat4>>> redoStack;


glm::mat4 rotateYMatrix(float angle) {
	// glm::mat4 rotatedMatrix = glm::mat4(cos(angle), 0, sin(angle), 0,
	// 									0, 1, 0, 0,
	// 									-sin(angle), 0, cos(angle), 0,
	// 									0, 0, 0, 1);
	glm::mat4 rotatedMatrix = glm::mat4(1.0f);
	rotatedMatrix[0]=glm::vec4(cos(angle), 0, sin(angle), 0);
	rotatedMatrix[1]=glm::vec4(0, 1, 0, 0);
	rotatedMatrix[2]=glm::vec4(-sin(angle), 0, cos(angle), 0);
	rotatedMatrix[3]=glm::vec4(0, 0, 0,1);
	return rotatedMatrix;
}
glm::mat4 rotateZMatrix(float angle) {
	glm::mat4 rotatedMatrix = glm::mat4(1.0f);
	rotatedMatrix[0]=glm::vec4(cos(angle), -sin(angle), 0, 0);
	rotatedMatrix[1]=glm::vec4(sin(angle), cos(angle), 0, 0);
	rotatedMatrix[2]=glm::vec4(0, 0, 1, 0);
	rotatedMatrix[3]=glm::vec4(0,0,0,1);
	return rotatedMatrix;
}
glm::mat4 translateMatrix(float x, float y, float z) {
	glm::mat4 translatedMatrix = glm::mat4(1.0f);
	translatedMatrix[3]=glm::vec4(x,y,z,1.0f);
	return translatedMatrix;
}
bool do_picking = false;
std::vector<bool> selected;

#include <cmath>

void vCalcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter,
                 float &fVecX, float &fVecY, float &fVecZ) {
    float fNewVecX, fNewVecY, fNewVecZ, fOldVecX, fOldVecY, fOldVecZ, fLength;

    fNewVecX = fNewX * 2.0f / fDiameter;
    fNewVecY = fNewY * 2.0f / fDiameter;
    fNewVecZ = (1.0f - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

    if (fNewVecZ < 0.0f) {
        fLength = sqrt(1.0f - fNewVecZ);
        fNewVecZ = 0.0f;
        fNewVecX /= fLength;
        fNewVecY /= fLength;
    } else {
        fNewVecZ = sqrt(fNewVecZ);
    }

    fOldVecX = fOldX * 2.0f / fDiameter;
    fOldVecY = fOldY * 2.0f / fDiameter;
    fOldVecZ = (1.0f - fOldVecX * fOldVecX - fOldVecY * fOldVecY);

    if (fOldVecZ < 0.0f) {
        fLength = sqrt(1.0f - fOldVecZ);
        fOldVecZ = 0.0f;
        fOldVecX /= fLength;
        fOldVecY /= fLength;
    } else {
        fOldVecZ = sqrt(fOldVecZ);
    }

 
    fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
    fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
    fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;
}

void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, std::array<std::array<float, 4>, 4> &mNewMat) {
    float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;

    fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);


    if (fRadians < 0.000001f) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mNewMat[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
        return;
    }

  
    fInvLength = 1.0f / fRadians;
    fNewVecX = fVecX * fInvLength;
    fNewVecY = fVecY * fInvLength;
    fNewVecZ = fVecZ * fInvLength;

    float dSinAlpha = sin(fRadians);
    float dCosAlpha = cos(fRadians);
    float dT = 1.0f - dCosAlpha;

    mNewMat[0][0] = dCosAlpha + fNewVecX * fNewVecX * dT;
    mNewMat[0][1] = fNewVecX * fNewVecY * dT + fNewVecZ * dSinAlpha;
    mNewMat[0][2] = fNewVecX * fNewVecZ * dT - fNewVecY * dSinAlpha;
    mNewMat[0][3] = 0.0f;

    mNewMat[1][0] = fNewVecX * fNewVecY * dT - fNewVecZ * dSinAlpha;
    mNewMat[1][1] = dCosAlpha + fNewVecY * fNewVecY * dT;
    mNewMat[1][2] = fNewVecY * fNewVecZ * dT + fNewVecX * dSinAlpha;
    mNewMat[1][3] = 0.0f;

    mNewMat[2][0] = fNewVecZ * fNewVecX * dT + fNewVecY * dSinAlpha;
    mNewMat[2][1] = fNewVecZ * fNewVecY * dT - fNewVecX * dSinAlpha;
    mNewMat[2][2] = dCosAlpha + fNewVecZ * fNewVecZ * dT;
    mNewMat[2][3] = 0.0f;

    mNewMat[3][0] = 0.0f;
    mNewMat[3][1] = 0.0f;
    mNewMat[3][2] = 0.0f;
    mNewMat[3][3] = 1.0f;
}

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	// glClearColor(0.85, 0.85, 0.85, 1.0);
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	// for( size_t idx = 0; idx < 100; ++idx ) {
	// 	selected.push_back( false );
	// }
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(0.0,1.0);

	for( size_t idx = 0; idx < 100; ++idx ) {
		glm::mat4 T = glm::mat4(1.0f); // No initial transformation
		glm::vec3 col( distribution(generator), distribution(generator), distribution(generator) );

		xforms.push_back( T );
		cols.push_back( col );
		selected.push_back( false );
	}

	do_picking = false;
	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(5.0f, 10.0f, 15.0f);
	// m_light.rgbIntensity = vec3(0.0f); // light
	m_light.rgbIntensity = vec3(0.8f, 0.8f, 0.8f);

}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");

		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		
		location = m_shader.getUniformLocation("picking");
		glUniform1i( location, do_picking ? 1 : 0 );


		//-- Set LightSource uniform for the scene:
		if( !do_picking ) {
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		

		//-- Set background light ambient intensity
		
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.25f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Menu", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...
		if( ImGui::TreeNode( "Application" ) ) {
			// Create Button, and check if it was clicked:
			if( ImGui::Button( "Reset Position (I)" ) ) {
				glm::vec3 modelOrigin = glm::vec3(m_rootNode->trans[3]);
				m_view = glm::translate(glm::mat4(1.0f), modelOrigin) * m_view;
			}
			if( ImGui::Button( "Reset Orientation (O)" ) ) {
				m_view = glm::mat4(1.0f);
			}
			if( ImGui::Button( "Reset Joints (S)" ) ) {
				for (size_t i = 0; i < xforms.size(); ++i) {
					xforms[i] = glm::mat4(1.0f);
				}
				undoStack.clear();
				redoStack.clear();
			}
			if( ImGui::Button( "Reset All (A)" ) ) {
				m_view = glm::mat4(1.0f);
				for (size_t i = 0; i < xforms.size(); ++i) {
				xforms[i] = glm::mat4(1.0f);
				}
				undoStack.clear();
				redoStack.clear();
				}
			if( ImGui::Button( "Quit Application (Q)" ) ) {
				glfwSetWindowShouldClose(m_window, GL_TRUE);
			}
			ImGui::TreePop();
		}
		if( ImGui::TreeNode( "Edit" ) ) {
			if( ImGui::Button( "Undo (U)" ) ) {
				if (!undoStack.empty()) {
					std::vector<std::pair<int, glm::mat4>> tempList;
					for (size_t i = 0; i < selected.size(); ++i) {
						if (selected[i]) {
							tempList.push_back(std::make_pair(i, xforms[i]));
						}
					}
					redoStack.push_back(tempList);

					auto lastActionsList = undoStack.back();
					for (const auto &action : lastActionsList) {
						int nodeId = action.first;
						glm::mat4 lastTransform = action.second;
						xforms[nodeId] = lastTransform;
					}
					undoStack.pop_back();
				
				}
			}
			if( ImGui::Button( "Redo (R)" ) ) {
				if (!redoStack.empty()) {
					std::vector<std::pair<int, glm::mat4>> tempList;
					for (size_t i = 0; i < selected.size(); ++i) {
						if (selected[i]) {
							tempList.push_back(std::make_pair(i, xforms[i]));
						}
					}
					undoStack.push_back(tempList);

					auto lastActionsList = redoStack.back();
					redoStack.pop_back();
					for (const auto &action : lastActionsList) {
						int nodeId = action.first;
						glm::mat4 lastTransform = action.second;
						xforms[nodeId] = lastTransform;
					}
					
				}
				
			}
			ImGui::TreePop();
		}
		if( ImGui::TreeNode( "Options" ) ) {
			if( ImGui::Checkbox( "Circle (C)", &show_circle ) ) {
				
			}
			if( ImGui::Checkbox( "Z-buffer (Z)", &use_depth_buffering ) ) {
			if( use_depth_buffering ) {
				glEnable( GL_DEPTH_TEST );
			} else {
				glDisable( GL_DEPTH_TEST );
			}
			}
			if( ImGui::Checkbox( "Backface culling (B)", &use_backface_culling ) ) {
				
			}
			if( ImGui::Checkbox( "Frontface culling (F)", &use_frontface_culling ) ) {
				
			}
			ImGui::TreePop();
		}	
		ImGui::PushID( 0 );
		if( ImGui::RadioButton( "Position/Orientation (P)", &interactionMode, 0 ) ) {
			// Select this.
		}
		ImGui::PopID();
		ImGui::PushID( 1 );
		if( ImGui::RadioButton( "Joints (J)", &interactionMode, 1 ) ) {
			// Select this.
		}
		ImGui::PopID();
		
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
		
	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		if(do_picking) {
			unsigned int idx = node.m_nodeId;
			float r = float(idx&0xff) / 255.0f;
			float g = float((idx>>8)&0xff) / 255.0f;
			float b = float((idx>>16)&0xff) / 255.0f;

			location = shader.getUniformLocation("material.kd");
			glUniform3f( location, r, g, b );
			CHECK_GL_ERRORS;
		}
		else {
			//-- Set NormMatrix:
			location = shader.getUniformLocation("NormalMatrix");
			mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;


			//-- Set Material values:
			location = shader.getUniformLocation("material.kd");
			vec3 kd = node.material.kd;
			if(selected[node.m_nodeId]){
				// printf("objId: %d\n", node.m_nodeId);
				kd = vec3(0.5f, 0.7f, 1.0f);
			}
			glUniform3fv(location, 1, value_ptr(kd));

			location = shader.getUniformLocation("material.ks");
			vec3 ks = node.material.ks;
			glUniform3fv(location, 1, value_ptr(ks));

			location = shader.getUniformLocation("material.shininess");
			float shininess = node.material.shininess;
			glUniform1f(location, shininess);

		}

		CHECK_GL_ERRORS;
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	// if( do_picking ) {
	// 	glClearColor( 1.0, 1.0, 1.0, 1.0 );
	// 	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// } else {
	// 	glClearColor( 0.35, 0.35, 0.35, 1.0 );
	// }

	if( use_depth_buffering ) {
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LESS );
	} else {
		glDisable( GL_DEPTH_TEST );
	}

	if( use_backface_culling && use_frontface_culling) {
		glEnable( GL_CULL_FACE );
		glCullFace( GL_FRONT_AND_BACK );
	}
	else if( use_backface_culling ) {
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
	} 
	else if( use_frontface_culling ) {
		glEnable( GL_CULL_FACE );
		glCullFace( GL_FRONT );
	} else {
		glDisable( GL_CULL_FACE );
	}

	glBindVertexArray(m_vao_meshData);

	renderSceneGraph(*m_rootNode, m_view, m_rootNode->trans, 0, 0, 0, 0);
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
	glDisable( GL_DEPTH_TEST );
	if(show_circle){
		renderArcCircle();
	}
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root, glm::mat4 transformation, glm::mat4 jointOrigin, float minX, float maxX, float minY, float maxY) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	// glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.
	glm::mat4 nodeTransformation = transformation;
	for (const SceneNode * node : root.children) {
		if (node->m_nodeType == NodeType::JointNode){
			const JointNode * jointNode = static_cast<const JointNode *>(node);
			jointOrigin = jointNode->trans;
			minX = jointNode->m_joint_x.min;
			maxX = jointNode->m_joint_x.max;
			minY = jointNode->m_joint_y.min;
			maxY = jointNode->m_joint_y.max;
			continue;
		}

		if (node->m_nodeType != NodeType::GeometryNode)
			continue;

		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);

		glm::mat4 rotationMatrix = xforms[geometryNode->m_nodeId];
		glm::vec3 rotationAxis;
		float rotationAngle;
		glm::vec3 eulerAngles = glm::eulerAngles(glm::quat_cast(rotationMatrix));
		rotationAxis = eulerAngles;
		rotationAngle = glm::degrees(glm::length(rotationAxis));

		if (rotationAngle < 12.0f) {
			nodeTransformation = nodeTransformation * jointOrigin * rotationMatrix * glm::inverse(jointOrigin);
		} else {
			rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(12.0f), glm::normalize(rotationAxis));
			nodeTransformation = nodeTransformation * jointOrigin * rotationMatrix * glm::inverse(jointOrigin);
		}


		updateShaderUniforms(m_shader, *geometryNode, nodeTransformation);			
		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
	}

	for (const SceneNode * node : root.children) {
		renderSceneGraph(*node, nodeTransformation, jointOrigin, minX, maxX, minY, maxY);
	}
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		double changeInX = xPos - previousXPos;
		double changeInY = yPos - previousYPos;
		if (isLeftDragging == true) {
			if (interactionMode == 0) {
				m_view = translateMatrix(changeInX*0.01f, changeInY * -0.01f, 0) * m_view;

			}
		}
		if(isMiddleDragging == true){
			if(interactionMode == 0){
				m_view = translateMatrix(0, 0, changeInY * 0.01f) * m_view;
			}
			if (interactionMode == 1){



				for (size_t i = 0; i < selected.size(); ++i) {
					if (selected[i]) {
					
						xforms[i] = rotateZMatrix(changeInY * 0.01) * xforms[i];
			
				}

					
					
				}
			}
		}
		
		if(isRightDragging == true){
			if(interactionMode == 0){
		
				float newX = static_cast<float>(xPos) - m_windowWidth / 2.0f; 
				float newY = m_windowHeight / 2.0f - static_cast<float>(yPos);
				float oldX = static_cast<float>(previousXPos) - m_windowWidth / 2.0f;
				float oldY = m_windowHeight / 2.0f - static_cast<float>(previousYPos);


				float rotationAxisX, rotationAxisY, rotationAxisZ;
				float diameter = m_windowHeight * 0.5;

				vCalcRotVec(newX, newY, oldX, oldY, diameter, rotationAxisX, rotationAxisY, rotationAxisZ);

				std::array<std::array<float, 4>, 4> rotationMatrix;
				vAxisRotMatrix(rotationAxisX, rotationAxisY, rotationAxisZ, rotationMatrix);
				
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						rotationMat4[i][j] = rotationMatrix[i][j];
					}
				}
				glm::vec3 modelOrigin = glm::vec3(m_rootNode->trans[3]);
				m_view = glm::translate(glm::mat4(1.0f), modelOrigin) * rotationMat4 * glm::translate(glm::mat4(1.0f), -modelOrigin) * m_view;
                
			}
			if(interactionMode ==1){
				for (size_t i = 0; i < selected.size(); ++i) {
					if (selected[i]) {
						xforms[i] = rotateYMatrix(changeInX * 0.01) * xforms[i];
					}
				}
			}
		}
	}
	previousXPos = xPos;
	previousYPos = yPos;
	eventHandled=true;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		double xpos, ypos;
      	glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);

		if(button==GLFW_MOUSE_BUTTON_LEFT){
			if (actions == 1){
				//hold
				isLeftDragging = true;
				glfwGetCursorPos( m_window, &xpos, &ypos );
				if (interactionMode == 1){
				do_picking = true;

				uploadCommonSceneUniforms();
				glClearColor(1.0, 1.0, 1.0, 1.0 );
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
				glClearColor(0.35, 0.35, 0.35, 1.0);

				draw();

				// I don't know if these are really necessary anymore.
				// glFlush();
				// glFinish();

				CHECK_GL_ERRORS;

				// Ugly -- FB coordinates might be different than Window coordinates
				// (e.g., on a retina display).  Must compensate.
				xpos *= double(m_framebufferWidth) / double(m_windowWidth);
				// WTF, don't know why I have to measure y relative to the bottom of
				// the window in this case.
				ypos = m_windowHeight - ypos;
				ypos *= double(m_framebufferHeight) / double(m_windowHeight);

				GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
				// A bit ugly -- don't want to swap the just-drawn false colours
				// to the screen, so read from the back buffer.
				glReadBuffer( GL_BACK );
				// Actually read the pixel at the mouse location.
				glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
				CHECK_GL_ERRORS;

				// Reassemble the object ID.
				unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

				if( what < xforms.size() ) {
					selected[what] = !selected[what];
				}

				do_picking = false;

				CHECK_GL_ERRORS;
				}

			}
			if (actions == 0){
				//let go
				isLeftDragging = false;
				previousYPos = ypos;
				previousXPos = xpos;	
			}
			eventHandled = true;
		}
		else if(button==GLFW_MOUSE_BUTTON_RIGHT){
			if(actions == 1){
				//hold
				isRightDragging = true;
				
			}
			if(actions == 0){
				//let go
				isRightDragging = false;
				previousYPos = ypos;
				previousXPos = xpos;
			}	
			eventHandled = true;
		} 
		else if(button==GLFW_MOUSE_BUTTON_MIDDLE) {
			if(actions == 1){
				//hold
				initialYPos = ypos;
				isMiddleDragging = true;
				std::vector<std::pair<int, glm::mat4>> tempList;
				for (size_t i = 0; i < selected.size(); ++i) {
					if (selected[i]) {
						tempList.push_back(std::make_pair(i, xforms[i]));
					}
				}
				undoStack.push_back(tempList);
			}
			if(actions == 0){
				//let go
				isMiddleDragging = false;
				previousYPos = ypos;
				previousXPos = xpos;

				// std::vector<std::pair<int, glm::mat4>> tempList;
				// for (size_t i = 0; i < selected.size(); ++i) {
				// 	if (selected[i]) {
				// 		tempList.push_back(std::make_pair(i, xforms[i]));
				// 	}
				// }
				// redoStack.push_back(tempList);
			}	
			eventHandled = true;
		}

	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_P ) {
			interactionMode = 0;
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_J ) {
			interactionMode = 1;
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_I ) {
			m_view = glm::mat4(1.0f);
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_O ) {
			m_view = glm::mat4(1.0f);
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_S ) {
			for (size_t i = 0; i < xforms.size(); ++i) {
				xforms[i] = glm::mat4(1.0f);
			}
			undoStack.clear();
			redoStack.clear();
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_A ) {
			m_view = glm::mat4(1.0f);
			for (size_t i = 0; i < xforms.size(); ++i) {
				xforms[i] = glm::mat4(1.0f);
			}
			undoStack.clear();
			redoStack.clear();
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_Q ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_C ) {
			show_circle = !show_circle;
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_Z ) {
			use_depth_buffering = !use_depth_buffering;
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_B ) {
			use_backface_culling = !use_backface_culling;
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_F ) {
			use_frontface_culling = !use_frontface_culling;
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_U ) {
			if (!undoStack.empty()) {
				std::vector<std::pair<int, glm::mat4>> tempList;
				for (size_t i = 0; i < selected.size(); ++i) {
					if (selected[i]) {
						tempList.push_back(std::make_pair(i, xforms[i]));
					}
				}
				redoStack.push_back(tempList);

				auto lastActionsList = undoStack.back();
				for (const auto &action : lastActionsList) {
					int nodeId = action.first;
					glm::mat4 lastTransform = action.second;
					xforms[nodeId] = lastTransform;
				}
				undoStack.pop_back();
			}
			eventHandled = true;
		}
	}
	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_R ) {
			if (!redoStack.empty()) {
				std::vector<std::pair<int, glm::mat4>> tempList;
				for (size_t i = 0; i < selected.size(); ++i) {
					if (selected[i]) {
						tempList.push_back(std::make_pair(i, xforms[i]));
					}
				}
				undoStack.push_back(tempList);

				auto lastActionsList = redoStack.back();
				redoStack.pop_back();
				for (const auto &action : lastActionsList) {
					int nodeId = action.first;
					glm::mat4 lastTransform = action.second;
					xforms[nodeId] = lastTransform;
				}
			}
			eventHandled = true;
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}

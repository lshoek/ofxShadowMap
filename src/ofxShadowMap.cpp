#include "ofxShadowMap.h"
#include "ofLight.h"
#include "ofMaterial.h"
#include "ofGraphics.h"
#include "ofVectorMath.h"

bool ofxShadowMap::setup(int size, Resolution resolution)
{
	bool success = depthShader.load("../../../../../addons/ofxShadowMap/shaders/depth");

	ofFbo::Settings fboSettings;
	fboSettings.depthStencilAsTexture = true;

	if(resolution == _32) {
		fboSettings.depthStencilInternalFormat = GL_DEPTH_COMPONENT32;
	}
	else if (resolution == _24) {
		fboSettings.depthStencilInternalFormat = GL_DEPTH_COMPONENT24;
	}
	else {
		fboSettings.depthStencilInternalFormat = GL_DEPTH_COMPONENT16;
	}

	fboSettings.width = size;
	fboSettings.height = size;
	fboSettings.minFilter = GL_NEAREST;
	fboSettings.maxFilter = GL_NEAREST;
	fboSettings.numColorbuffers = 0;
	fboSettings.textureTarget = GL_TEXTURE_2D;
	fboSettings.useDepth = true;
	fboSettings.useStencil = false;
	fboSettings.wrapModeHorizontal = GL_CLAMP;
	fboSettings.wrapModeVertical = GL_CLAMP;
	fbo.allocate(fboSettings);
	fbo.getDepthTexture().setRGToRGBASwizzles(true);

	// Fix oversampling
	fbo.getDepthTexture().bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	fbo.getDepthTexture().unbind();

	return success;
}

void ofxShadowMap::begin(ofLight& light, float fustrumSize, float nearClip, float farClip)
{
	float left = -fustrumSize / 2.f;
	float right = fustrumSize / 2.f;
	float top = fustrumSize / 2.f;
	float bottom = -fustrumSize / 2.f;

	glm::mat4 lightProjection = glm::ortho(left, right, bottom, top, nearClip, farClip);
	glm::mat4 lightView = glm::inverse(light.getGlobalTransformMatrix());
	_lightViewProjection = lightProjection * lightView;

	depthShader.begin();
	fbo.begin();

	ofPushView();
	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(lightProjection);
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);

	ofLoadViewMatrix(lightView);
	ofViewport(ofRectangle(0, 0, fbo.getWidth(), fbo.getHeight()));

	ofClear(0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void ofxShadowMap::end()
{
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	ofPopView();

	fbo.end();
	depthShader.end();
}

void ofxShadowMap::updateShader(const std::shared_ptr<ofShader>& shader)
{
	shader->begin();
	shader->setUniformMatrix4f("lightSpaceMatrix", _lightViewProjection);
	shader->setUniformTexture("shadowMap", fbo.getDepthTexture(), 1);
	shader->end();
}

const ofTexture& ofxShadowMap::getDepthTexture() const {
	return fbo.getDepthTexture();
}

ofTexture& ofxShadowMap::getDepthTexture(){
	return fbo.getDepthTexture();
}

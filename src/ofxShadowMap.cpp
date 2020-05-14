#include "ofxShadowMap.h"
#include "ofLight.h"
#include "ofMaterial.h"
#include "ofGraphics.h"
#include "ofVectorMath.h"

bool ofxShadowMap::setup(int size, Resolution resolution)
{
	bool bSuccess = depthShader.load("../../../../../addons/ofxShadowMap/shaders/utils/depth");

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

	glGenSamplers(1, samplerId.get());
	glSamplerParameteri(*samplerId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(*samplerId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(*samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(*samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glSamplerParameterfv(*samplerId, GL_TEXTURE_BORDER_COLOR, borderColor);

	return bSuccess;
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
	const int texUnit = 1;

	shader->begin();
	shader->setUniformMatrix4f("lightSpaceMatrix", _lightViewProjection);

	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, fbo.getDepthTexture().getTextureData().textureID);
	glBindSampler(texUnit, *samplerId);

	shader->setUniformTexture("shadowMap", fbo.getDepthTexture(), 1);
	glActiveTexture(GL_TEXTURE0);

	shader->end();
}

const ofTexture& ofxShadowMap::getDepthTexture() const {
	return fbo.getDepthTexture();
}

ofTexture& ofxShadowMap::getDepthTexture(){
	return fbo.getDepthTexture();
}

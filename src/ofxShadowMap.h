#pragma once

#include "ofConstants.h"
#include "ofShader.h"
#include "ofFbo.h"

class ofxShadowMap
{
public:
	bool setup(int size);

	void begin(ofLight & light, float fustrumSize, float nearClip, float farClip);
	void end();

	void updateShader(const std::shared_ptr<ofShader>& shader);

	const ofTexture & getDepthTexture() const;
	ofTexture & getDepthTexture();

private:
	ofShader depthShader;
	ofShader blurShader;

	ofFbo fbo;

	glm::mat4 _normalizeCoordsMatrix;
	glm::mat4 _lightViewProjection;

	static void deleteSampler(GLuint* sampler) {
		glDeleteSamplers(1, sampler);
	}
	std::unique_ptr<GLuint, decltype(&deleteSampler)> samplerId{ new GLuint, &deleteSampler };

	const std::string OFXSHADOW_SHADER_DIR = "../../../../../addons/ofxShadowMap/shaders/";
};

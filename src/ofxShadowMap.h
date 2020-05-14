#pragma once

#include "ofConstants.h"
#include "ofShader.h"
#include "ofFbo.h"

class ofxShadowMap
{
public:
	enum Resolution{ _32, _24, _16 };

	bool setup(int size, Resolution resolution = _32);

	void begin(ofLight & light, float fustrumSize, float nearClip, float farClip);
	void end();

	void updateShader(const std::shared_ptr<ofShader>& shader);

	const ofTexture & getDepthTexture() const;
	ofTexture & getDepthTexture();

private:
	ofShader depthShader;
	ofFbo fbo;

	glm::mat4 _lightViewProjection;

	static void deleteSampler(GLuint* sampler) {
		glDeleteSamplers(1, sampler);
	}
	std::unique_ptr<GLuint, decltype(&deleteSampler)> samplerId{ new GLuint, &deleteSampler };
};
